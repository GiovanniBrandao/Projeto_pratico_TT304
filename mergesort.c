#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

pthread_t t[8];

// Estrutura que será passada para as threads que contém os dados necessários para execução do programa
typedef struct argumentos
{
    char **posidofile; // Nome dos arquivos de entrada para cada thread
    int numdearqs;     // Número de arquivos que determinada thread processa
    char *outputfile;  // Nome do arquivo de saída
    int *valores;      // Armazena os valores lidos dos arquivos
    int tamanhoatual;  // Quantidade de valores de um arquivo
    int thread_id;     // Identificador da thread
} ThreadArg;

// função que trata os parametros de entrada. Lê e divide a quantidade de arquivos
void tratar_parametros_entrada(int arqc, char *arqv[], int *qnt, int *numarqs, char **outputfile)
{
    *qnt = atoi(arqv[1]);

    // Procura pelo argumento "-o" para definir o arquivo de saída
    for (int i = 2; i < arqc; i++)
    {
        if (strcmp(arqv[i], "-o") == 0 && i + 1 < arqc)
        {
            *outputfile = arqv[i + 1];
            break;
        }
    }

    if (*outputfile == NULL)
    {
        printf("Erro: Nome do arquivo de saída não especificado.\n");
        exit(1);
    }

    // Conta a quantidade de arquivos de entrada antes do "-o"
    for (int i = 2; i < arqc && strcmp(arqv[i], "-o") != 0; i++)
    {
        (*numarqs)++;
    }

    if (*numarqs == 0)
    {
        printf("Erro: Nenhum arquivo de entrada especificado.\n");
        exit(1);
    }

    // verificação da quantidade de threads
    if (*qnt > *numarqs)
    {
        printf("Erro: O número de threads não pode ser maior que o número de arquivos.\n");
        exit(1);
    }
}

void merge(int vet[], int esq, int meio, int dir)
{
    int tamVet1 = meio - esq + 1;
    int tamVet2 = dir - meio;
    int *E = malloc(tamVet1 * sizeof(int));
    int *D = malloc(tamVet2 * sizeof(int));

    for (int i = 0; i < tamVet1; i++)
        E[i] = vet[esq + i];
    for (int j = 0; j < tamVet2; j++)
        D[j] = vet[meio + 1 + j];

    int i = 0, j = 0, k = esq;
    while (i < tamVet1 && j < tamVet2)
    {
        vet[k++] = (E[i] <= D[j]) ? E[i++] : D[j++];
    }
    while (i < tamVet1)
        vet[k++] = E[i++];
    while (j < tamVet2)
        vet[k++] = D[j++];

    free(E);
    free(D);
}

// Função que organiza um vetor passado
void organizacao(int vet[], int esq, int dir)
{
    if (esq < dir)
    {
        int meio = esq + (dir - esq) / 2;
        organizacao(vet, esq, meio);
        organizacao(vet, meio + 1, dir);
        merge(vet, esq, meio, dir);
    }
}

// Função que le os aruqivos de entrada
void *ler_arquivo(void *arg)
{
    ThreadArg *argholder = (ThreadArg *)arg; // auxiliar para os arquivos da estrutura. Transforma de void para tipo ThreadArg
    int capacidade = 100;                    // Capacidade inicial do vetor
    int valor;                               // auxiciliar que vai conter o inteiro lido no momento
    struct timespec tempo_inicio, tempo_fim; // calcular inicio e fim da thread
    double tempothread;                      // tempo total da thread

    argholder->valores = malloc(capacidade * sizeof(int));
    if (argholder->valores == NULL)
    {
        printf("Erro ao alocar memória para valores\n");
        pthread_exit(NULL);
    }

    argholder->tamanhoatual = 0;
    for (int i = 0; i < argholder->numdearqs; i++)
    {
        FILE *arquivo = fopen(argholder->posidofile[i], "r"); // abre o arquivo para leitura
        if (arquivo == NULL)
        {
            printf("Erro ao abrir arquivo %s\n", argholder->posidofile[i]);
            pthread_exit(NULL);
        }

        // Lê e armazena valores do arquivo
        while (fscanf(arquivo, "%d", &valor) == 1)
        {
            if (argholder->tamanhoatual >= capacidade) // aumenta a capacidade do vetor caso seja menor que o arquivo
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
            argholder->valores[argholder->tamanhoatual++] = valor; // armazena o valor lido detro da estrutura
        }
        fclose(arquivo);
    }

    clock_gettime(CLOCK_MONOTONIC, &tempo_inicio);

    // Ordena os valores lidos pela thread após a leitura de todos os arquivos
    organizacao(argholder->valores, 0, argholder->tamanhoatual - 1);

    clock_gettime(CLOCK_MONOTONIC, &tempo_fim);
    tempothread = (tempo_fim.tv_sec - tempo_inicio.tv_sec) + (tempo_fim.tv_nsec - tempo_inicio.tv_nsec) / 1e9;
    printf("Thread %d: Tempo de execução = %f segundos\n", argholder->thread_id, tempothread);

    pthread_exit(NULL);
}

// Cria a quantidade de threads requisitada pelo usuário
void iniciar_threads(ThreadArg *args, int qnt)
{
    for (int i = 0; i < qnt; i++)
    {
        args[i].thread_id = i + 1;
        if (pthread_create(&t[i], NULL, ler_arquivo, (void *)&args[i]) != 0)
        {
            printf("Erro ao criar a thread %d\n", i + 1);
            exit(1);
        }
    }
}

// Espera cada thread terminar
void esperar_threads(int qnt)
{
    for (int i = 0; i < qnt; i++)
    {
        pthread_join(t[i], NULL);
    }
}

// Mescla os vetores organizados de cada thread em um único vetor
int *mesclar_vetores(ThreadArg *args, int qnt, int *tamanhototal)
{

    int i;

    *tamanhototal = 0;
    for (i = 0; i < qnt; i++)
    {
        *tamanhototal += args[i].tamanhoatual; // leitura do tamanho de todas as threads
    }

    int *vetorfinal = malloc(*tamanhototal * sizeof(int)); // tamanho do vetor total
    int posicao = 0;

    // copia dos valores armazenados de cada thread para o vetor final, indicando sua posição contínua
    for (i = 0; i < qnt; i++)
    {
        memcpy(vetorfinal + posicao, args[i].valores, args[i].tamanhoatual * sizeof(int));
        posicao += args[i].tamanhoatual;
    }

    return vetorfinal;
}

// Grava o vetor final em um arquivo de saída
void gravar_vetor_ordenado(int *vetor, int tamanho, const char *outputfile)
{
    FILE *output = fopen(outputfile, "w"); // cria um arquivo em modo de saída
    if (output == NULL)
    {
        printf("Erro ao abrir arquivo de saída\n");
        exit(1);
    }
    for (int i = 0; i < tamanho; i++)
    {
        fprintf(output, "%d\n", vetor[i]);
    }
    fclose(output);
}

int main(int arqc, char *arqv[])
{

    int qnt;                             // quantidade de threads
    int numarqs = 0;                     // quantidade de arquivos
    int tamanhototal;                    // quantidade de inteiros lidos de um arquivo
    int *vetorfinal;                     // vetor final ordenado
    char *outputfile = NULL;             // Onde será guardado o arquivod e saída
    struct timespec inicioprog, fimprog; // variáveis para calcular, respectivamente, o inicio e o fim do programa principal
    double tempoprogtot;                 // variável para calcular o tempo total do programa
    ThreadArg *args;                     // ponteiro para a estrutura com os parametros

    clock_gettime(CLOCK_MONOTONIC, &inicioprog);

    tratar_parametros_entrada(arqc, arqv, &qnt, &numarqs, &outputfile);

    args = malloc(numarqs * sizeof(ThreadArg));
    int arqsporthread = numarqs / qnt; // divisão de arquivos por thread
    int arqsextras = numarqs % qnt;    // arquivos extras caso a divisão não seja inteira
    int posiatualdoarq = 2;            // posição do vetor de entrada (arqv) que começam os nomes dos arquivos

    for (int i = 0; i < numarqs; i++)
    {
        args[i].outputfile = outputfile;
        args[i].numdearqs = arqsporthread + (i < arqsextras ? 1 : 0); // comparação que atribui um arquivo extra caso uma thread não tenha ainda
        args[i].posidofile = malloc(args[i].numdearqs * sizeof(char *));
        for (int j = 0; j < args[i].numdearqs; j++)
        {
            args[i].posidofile[j] = arqv[posiatualdoarq++];
        }
    }

    iniciar_threads(args, qnt);
    esperar_threads(qnt);

    vetorfinal = mesclar_vetores(args, qnt, &tamanhototal);
    organizacao(vetorfinal, 0, tamanhototal - 1); // Ordenação final para garantir a organização completa
    gravar_vetor_ordenado(vetorfinal, tamanhototal, outputfile);

    // libera a memória alocada dinamicamente
    for (int i = 0; i < qnt; i++)
        free(args[i].posidofile);
    free(args);
    free(vetorfinal);

    clock_gettime(CLOCK_MONOTONIC, &fimprog);
    tempoprogtot = (fimprog.tv_sec - inicioprog.tv_sec) + (fimprog.tv_nsec - inicioprog.tv_nsec) / 1e9;
    printf("Tempo total de processamento: %.6f segundos\n", tempoprogtot);

    return 0;
}
