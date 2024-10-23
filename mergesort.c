#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_t t1;
pthread_t t2;
pthread_t t3;
pthread_t t4;
pthread_t t5;
pthread_t t6;
pthread_t t7;
pthread_t t8;

int executar_funcoes() // essa função vai unir a de ler o arquivo e organizar os valores, para a thread fazer ambas
{
}

int *ler_arquivo(FILE *arquivo, char *posidofile, int *valores, int *tamanhoatual) // passar o arquivo e o vetor para encontar o nome do arquivo
{
    int capacidade = 100;
    int i, valor = 0;
    int *tamanhoatual = 0;

    // tamanhoatual diz a posição atual do vetor, e conta para saber se ele chegou ao final

    valores = (int *)malloc(capacidade * sizeof(int)); // Não sei quantos valores vão ter dentro do arquivo, por isso a alocação
    if (valores == NULL)
    {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    arquivo = fopen(posidofile, "r"); // nome do arquivo vai estar na posição passada pela função
    if (arquivo == NULL)
    {

        printf("Nao foi possivel abrir o arquivo\n");
        return 1;
    }

    while (fscanf(arquivo, "%d", &valor) == 1)
    {
        if (*tamanhoatual >= capacidade)
        {
            capacidade *= 2; // Dobrar a capacidade se necessário
            valores = (int *)realloc(valores, capacidade * sizeof(int));
            if (valores == NULL)
            {
                printf("Erro ao realocar memória.\n");
                return 1;
            }
        }
        valores[*tamanhoatual] = valor;
        (*tamanhoatual)++;
    }

    fclose(posidofile);

    return valores;
}

int *ler_arquivo(FILE *arquivo, int *tamanho) //essa função veio do chat, usei de exemplo
{
    int capacidade = 100; // Capacidade inicial do vetor
    int *valores;
    int valor; // está variável irá guardar o valor atual lido para guardar em "valores"

    valores = (int *)malloc(capacidade * sizeof(int)); // Alocação inicial
    if (valores == NULL)
    {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    *tamanho = 0; // Tamanho atual do vetor

    while (fscanf(arquivo, "%d", &valor) == 1)
    {
        if (*tamanho >= capacidade)
        {
            capacidade *= 2; // Dobrar a capacidade se necessário
            valores = (int *)realloc(valores, capacidade * sizeof(int));
            if (valores == NULL)
            {
                printf("Erro ao realocar memória.\n");
                return 1;
            }
        }
        valores[*tamanho] = valor;
        (*tamanho)++;
    }

    return valores;
}

//Implementação do MergeSort
void merge(int vet[], int esq, int meio, int dir)
{
    int tamVet1 = meio - esq + 1;
    int tamVet2 = dir - meio;

    int E[tamVet1], D[tamVet2]; //Vetores temporarios, mudar para alocacao dinamica

    for (int i = 0; i < tamVet1; i++)
        E[i] = vet[esq + i];
    
    for (int j = 0; j < tamVet2; j++)
        D[j] = vet[meio + 1 + j];
    
    int i = 0, j = 0, k = esq;

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

    while (i < tamVet1)
    {
        vet[k] = E[i];
        i++;
        k++;
    }

    while (j < tamVet2)
    {
        vet[k] = D[j];
        j++;
        k++;
    }
        
    }
    

}

void mergeSort(int vet[], int esq, int dir)
{
    if (esq < dir)
    {
        int meio = esq + (dir -1) / 2; 

        mergeSort(vet, l, dir);
        mergeSort(vet, meio + 1, dir);

        merge(vet, esq, meio, dir);

    }


}

struct thread_data {
    int *vet;      // Ponteiro para o array de inteiros
    int inicio;     // Índice inicial
    int fim;       // Índice final
};

void *thread_merge_sort(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    mergeSort(data->arr, data->start, data->end);
    pthread_exit(NULL);
}



int organizacao() //onde vai ter o mergesort
{
    // Cria as threads e inicia o Merge Sort em cada uma
    for (int i = 0; i < T; i++) {
        td[i].arr = vetor;            // Vetor de inteiros
        td[i].start = i * (N / T);    // Índice inicial
        td[i].end = (i + 1) * (N / T) - 1; // Índice final
        pthread_create(&threads[i], NULL, thread_merge_sort, &td[i]);
    }

    // Aguarda a conclusão de todas as threads
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);

    merge(vetor, 0, (N / T) - 1, N - 1);
}

}

int main(int arqc, char *arqv[])
{

    int qnt = 0, i = 0, contador = 0, tamanho = 0;
    int *valores, *tamanhoatual;
    FILE *arquivo;
    float inicio, fim;

    while (arqv[contador + 2] != NULL) // os arquivos começaram da posição 2 do arqv
    {
        contador++; // verificar quantos arquivo que são para ser lidos, tem dentro do vetor arqv.
        printf("contado: %d\n", contador);
    }

    // arquivo = (FILE **)malloc(contador * sizeof(FILE *)); - a ideia era um vetor de file para conter a leitura de cada file separadamente

    for (i = 0; i < contador; i++)
    {

        arquivo = fopen(arqv[i + 2], "r");

        if (arquivo == NULL)
        {

            printf("Nao foi possivel abrir o arquivo\n");
            return 1;
        }
    }

    for (i = 0; i < contador; i++)
    {
        fclose(arquivo);
    }

    qnt = atoi(arqv[1]); // quantidade de threads que serão usadas

    if (qnt == 2)
    {

        pthread_create(&t1, NULL, (void *)ler_arquivo, (void *)arqv[2]);
        pthread_create(&t2, NULL, (void *)ler_arquivo, (void *)arqv[3]);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        printf("Fim\n");
    }

    else if (qnt == 4)
    {

        pthread_create(&t1, NULL, (void *)ler_arquivo, (void *)arqv[2]);
        pthread_create(&t2, NULL, (void *)ler_arquivo, (void *)arqv[3]);
        pthread_create(&t3, NULL, (void *)ler_arquivo, (void *)arqv[4]);
        pthread_create(&t4, NULL, (void *)ler_arquivo, (void *)arqv[5]);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
        printf("Fim\n");
    }

    else if (qnt == 8)
    {

        pthread_create(&t1, NULL, (void *)ler_arquivo, (void *)arqv[2]);
        pthread_create(&t2, NULL, (void *)ler_arquivo, (void *)arqv[3]);
        pthread_create(&t3, NULL, (void *)ler_arquivo, (void *)arqv[4]);
        pthread_create(&t4, NULL, (void *)ler_arquivo, (void *)arqv[5]);
        pthread_create(&t5, NULL, (void *)ler_arquivo, (void *)arqv[6]);
        pthread_create(&t6, NULL, (void *)ler_arquivo, (void *)arqv[7]);
        pthread_create(&t7, NULL, (void *)ler_arquivo, (void *)arqv[8]);
        pthread_create(&t8, NULL, (void *)ler_arquivo, (void *)arqv[9]);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
        pthread_join(t5, NULL);
        pthread_join(t6, NULL);
        pthread_join(t7, NULL);
        pthread_join(t8, NULL);
        printf("Fim\n");
    }

    free(arquivo);
    free(valores);

    return 0;
}