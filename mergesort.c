#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

pthread_t t[8];

typedef struct argumentos
{
    char **posidofile; // Nome do arquivo de entrada (cada thread pode ter +1 arquivo)
    int numdearqs;     // número de arquivos que determinada thread irá processar
    char *outputfile;  // Nome do arquivo de saída
    int *valores;      // Armazenar os valores lidos dos arquivos
    int tamanhoatual;  // Quantidade de valores de um arquivo
    int thread_id;     // identificador da thread

} ThreadArg;

void *ler_arquivo(void *arg)
{
    ThreadArg *argholder;
    int capacidade = 100; // Capacidade inicial do vetor
    int valor, i = 0;
    double tempothread;
    FILE *arquivo;
    struct timespec tempo_inicio, tempo_fim;

    clock_gettime(CLOCK_MONOTONIC, &tempo_inicio);

    argholder = (ThreadArg *)arg;

    argholder->valores = malloc(capacidade * sizeof(int));
    if (argholder->valores == NULL)
    {
        printf("Erro ao alocar memória para valores\n");
        pthread_exit(NULL);
    }

    argholder->tamanhoatual = 0;
    for (int i = 0; i < argholder->numdearqs; i++)
    {
        arquivo = fopen(argholder->posidofile[i], "r");
        if (arquivo == NULL)
        {
            printf("Erro ao abrir arquivo %s\n", argholder->posidofile[i]);
            pthread_exit(NULL);
        }

        // Lê e armazena valores do arquivo
        while (fscanf(arquivo, "%d", &valor) == 1)
        {
            if (argholder->tamanhoatual >= capacidade)
            {
                capacidade *= 2;
                argholder->valores = realloc(argholder->valores, capacidade * sizeof(int));
                if (argholder->valores == NULL)
                {
                    printf("Erro ao realocar memória\n");
                    fclose(arquivo);
                    pthread_exit(NULL);
                }
            }
            argholder->valores[argholder->tamanhoatual++] = valor;
        }
        fclose(arquivo);
    }

    clock_gettime(CLOCK_MONOTONIC, &tempo_fim);

    tempothread = (tempo_fim.tv_sec - tempo_inicio.tv_sec) + (tempo_fim.tv_nsec - tempo_inicio.tv_nsec) / 1e9;
    printf("Thread %d: Tempo de execução = %f segundos\n", argholder->thread_id, tempothread);

    pthread_exit(NULL);
}

int *mesclar_vetores(ThreadArg *args, int num_threads, int *tamanhototal)
{

    int i, j, k = 0;
    int *vetor_final;

    *tamanhototal = 0;

    for (i = 0; i < num_threads; i++)
    {
        *tamanhototal += args[i].tamanhoatual;
    }

    vetor_final = malloc((*tamanhototal) * sizeof(int));
    if (vetor_final == NULL)
    {
        printf("Erro ao alocar vetor final");
        exit(1);
    }

    for (i = 0; i < num_threads; i++)
    {
        for (j = 0; j < args[i].tamanhoatual; j++)
        {
            vetor_final[k++] = args[i].valores[j]; //valor j lido na posição da thread i
        }
    }

    return vetor_final;
}

void merge(int vet[], int esq, int meio, int dir)
{
    int tamVet1, tamVet2, i, j, k;
    int *E, *D;

    tamVet1 = meio - esq + 1;
    tamVet2 = dir - meio;

    // Alocação dinâmica para os vetores temporários
    E = (int *)malloc(tamVet1 * sizeof(int));
    D = (int *)malloc(tamVet2 * sizeof(int));

    if (E == NULL || D == NULL)
    {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    // Copia os dados para os vetores temporários
    for (i = 0; i < tamVet1; i++)
        E[i] = vet[esq + i];

    for (j = 0; j < tamVet2; j++)
        D[j] = vet[meio + 1 + j];

    i = 0, j = 0, k = esq;

    // Combina os dois subarrays em ordem crescente
    while (i < tamVet1 && j < tamVet2)
    {
        if (E[i] <= D[j])
        {
            vet[k] = E[i];
            i++;
        }
        else
        {
            vet[k] = D[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes do vetor E, se houver
    while (i < tamVet1)
    {
        vet[k] = E[i];
        i++;
        k++;
    }

    // Copia os elementos restantes do vetor D, se houver
    while (j < tamVet2)
    {
        vet[k] = D[j];
        j++;
        k++;
    }

    // Libera a memória alocada dinamicamente
    free(E);
    free(D);
}

void organizacao(int vet[], int esq, int dir)
{
    if (esq < dir)
    {
        int meio;

        meio = esq + (dir - esq) / 2;

        // Ordena a primeira e a segunda metade
        organizacao(vet, esq, meio);
        organizacao(vet, meio + 1, dir);

        // Junta as metades ordenadas
        merge(vet, esq, meio, dir);
    }
}

int main(int arqc, char *arqv[])
{
    int qnt, i, j, tamanhototal, numarqs = 0;
    int arqsporthread, arqsextras, posiatualdoarq = 2;
    int *vetorfinal;
    char *outputfile = NULL;
    double tempoprogtot;
    ThreadArg *arg;
    FILE *output;
    struct timespec inicioprog, fimprog;

    clock_gettime(CLOCK_MONOTONIC, &inicioprog);

    qnt = atoi(arqv[1]); //quantidade de threads

    // Identificar o arquivo de saída na linha de comando
    for (i = 2; i < arqc; i++)
    {
        if (strcmp(arqv[i], "-o") == 0 && i + 1 < arqc)
        {
            outputfile = arqv[i + 1]; // Pega o próximo argumento como nome do arquivo de saída
            break;
        }
    }

    if (outputfile == NULL)
    {
        printf("Erro: Nome do arquivo de saída não especificado.\n");
        return 1;
    }

// conta quantos arquivo tem no vetor
    for (i = 2; i < arqc; i++)
    {
        if (strcmp(arqv[i], "-o") == 0)
        {
            break;
        }
        numarqs++;
    }

    if (numarqs == 0)
    {
        printf("Erro: Nenhum arquivo de entrada especificado.\n");
        return 1;
    }

    if (qnt > numarqs)
    {
        printf("Erro: O número de threads não pode ser maior que o número de arquivos.\n");
        return 1;
    }

    arg = malloc(numarqs * sizeof(ThreadArg)); //alocação da struct para passar valores

    arqsporthread = numarqs / qnt; //numero de arquivos por thread
    arqsextras = numarqs % qnt; //caso os arquivos não sejam divididos igualmente

    for (i = 0; i < numarqs; i++)
    {
        arg[i].outputfile = outputfile;
        arg[i].valores = NULL;
        arg[i].tamanhoatual = 0;
        arg[i].numdearqs = arqsporthread + (i < arqsextras ? 1 : 0); //atribui os arquivos extras caso aquela thread não tenha

        arg[i].posidofile = malloc(arg[i].numdearqs * sizeof(char *)); // Aloca memória para os nomes dos arquivos
        for (int j = 0; j < arg[i].numdearqs; j++)
        {
            arg[i].posidofile[j] = arqv[posiatualdoarq++]; // Atribui os nomes dos arquivos
        }
    }

    for (i = 0; i < qnt; i++)
    {
        arg[i].thread_id = i + 1; //apenas para visualizar qual thread foi executada

        if (pthread_create(&t[i], NULL, ler_arquivo, (void *)&arg[i]) != 0) // cria cada thread
        {
            printf("Erro ao criar a thread %d\n", i + 1);
            free(arg);
            exit(1);
        }
    }

    // Espera cada thread terminar
    for (i = 0; i < qnt; i++)
    {
        pthread_join(t[i], NULL);
    }

    vetorfinal = mesclar_vetores(arg, qnt, &tamanhototal);

    organizacao(vetorfinal, 0, tamanhototal - 1);

    // Escreve o vetor ordenado no arquivo de saída
    output = fopen(outputfile, "w");
    if (output == NULL)
    {
        printf("Erro ao abrir arquivo de saída");
        exit(1);
    }
    for (int i = 0; i < tamanhototal; i++)
    {
        fprintf(output, "%d\n", vetorfinal[i]);
    }

    fclose(output);

    // Libera memória alocada
    free(vetorfinal);
    for (i = 0; i < qnt; i++)
    {
        free(arg[i].valores);
            free(arg[i].posidofile);

    }
    free(arg);

    clock_gettime(CLOCK_MONOTONIC, &fimprog);

    tempoprogtot = (fimprog.tv_sec - inicioprog.tv_sec) + (fimprog.tv_nsec - inicioprog.tv_nsec) / 1e9;

    printf("Tempo total do programa: %.6f segundos\n", tempoprogtot);

    return 0;
}
