#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_t t[8];

typedef struct argumentos
{
    char *posidofile; // Nome do arquivo de entrada
    char *outputfile; // Nome do arquivo de saída
    int *valores;     // Armazenar os valores lidos dos arquivos
    int tamanhoatual; // Quantidade de valores de um arquivo

} ThreadArg;

void *ler_arquivo(void *arg)
{

    ThreadArg *args;
    int capacidade = 100; // Capacidade inicial do vetor
    int valor;
    FILE *arquivo;

    args = (ThreadArg *)arg;

    args->valores = malloc(capacidade * sizeof(int));
    if (args->valores == NULL)
    {
        printf("Erro ao alocar memória para valores\n");
        pthread_exit(NULL);
    }

    arquivo = fopen(args->posidofile, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir arquivo\n");
        pthread_exit(NULL);
    }

    args->tamanhoatual = 0;
    while (fscanf(arquivo, "%d", &valor) == 1)
    {
        if (args->tamanhoatual >= capacidade)
        {
            capacidade *= 2;
            args->valores = realloc(args->valores, capacidade * sizeof(int));
            if (args->valores == NULL)
            {
                printf("Erro ao realocar memória\n");
                fclose(arquivo);
                pthread_exit(NULL);
            }
        }
        args->valores[args->tamanhoatual++] = valor;
    }

    fclose(arquivo);
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

    vetor_final = malloc(*tamanhototal * sizeof(int));
    if (vetor_final == NULL)
    {
        printf("Erro ao alocar vetor final");
        exit(1);
    }

    for (i = 0; i < num_threads; i++)
    {
        for (j = 0; j < args[i].tamanhoatual; j++)
        {
            vetor_final[k++] = args[i].valores[j];
        }
    }

    return vetor_final;
}

void merge(int vet[], int esq, int meio, int dir)
{
    int tamVet1 = meio - esq + 1;
    int tamVet2 = dir - meio;

    // Alocação dinâmica para os vetores temporários
    int *E = (int *)malloc(tamVet1 * sizeof(int));
    int *D = (int *)malloc(tamVet2 * sizeof(int));

    if (E == NULL || D == NULL)
    {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    // Copia os dados para os vetores temporários
    for (int i = 0; i < tamVet1; i++)
        E[i] = vet[esq + i];

    for (int j = 0; j < tamVet2; j++)
        D[j] = vet[meio + 1 + j];

    int i = 0, j = 0, k = esq;

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
        int meio = esq + (dir - esq) / 2;

        // Ordena a primeira e a segunda metade
        organizacao(vet, esq, meio);
        organizacao(vet, meio + 1, dir);

        // Junta as metades ordenadas
        merge(vet, esq, meio, dir);
    }
}

int main(int arqc, char *arqv[])
{
    int qnt, i, tamanhototal, numarqs;
    int *vetorfinal;
    char *outputfile = NULL;
    ThreadArg *arg;
    FILE *output;

    qnt = atoi(arqv[1]);

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

    numarqs = arqc - 2;

    arg = malloc(numarqs * sizeof(ThreadArg));

    for (i = 0; i < qnt; i++)
    {
        arg[i].posidofile = arqv[i + 2]; 
        arg[i].outputfile = outputfile;  
        arg[i].valores = NULL;           
        arg[i].tamanhoatual = 0;         

        if (pthread_create(&t[i], NULL, ler_arquivo, (void *)&arg[i]) != 0) //cria cada thread
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
        fprintf(output, "%d ", vetorfinal[i]);

        // Se 5 números já foram escritos, pula para a próxima linha
        if ((i + 1) % 5 == 0)
        {
            fprintf(output, "\n"); // Nova linha
        }
    }

        // Adiciona uma nova linha no final, se não tiver terminado em uma nova linha
    if (tamanhototal % 5 != 0)
    {
        fprintf(output, "\n");
    }

    fclose(output);

    // Libera memória alocada
    free(vetorfinal);
    for (i = 0; i < qnt; i++)
    {
        free(arg[i].valores);
    }
    free(arg);

    return 0;
}
