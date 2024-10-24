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


int *ler_arquivo(FILE *arquivo, char *posidofile, int *valores, int *tamanhoatual) // passar o arquivo e o vetor para encontar o nome do arquivo
{
    int capacidade = 100;
    int i, valor = 0;


    // tamanhoatual diz a posição atual do vetor, e conta para saber se ele chegou ao final

    valores = (int *)malloc(capacidade * sizeof(int)); // Não sei quantos valores vão ter dentro do arquivo, por isso a alocação
    if (valores == NULL)
    {
        printf("Erro ao alocar memoria.\n");
        exit(1);
    }

    arquivo = fopen(posidofile, "r"); // nome do arquivo vai estar na posição passada pela função
    if (arquivo == NULL)
    {

        printf("Nao foi possivel abrir o arquivo\n");
        exit(1);
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
                exit(1);
            }
        }
        valores[*tamanhoatual] = valor;
        (*tamanhoatual)++;
    }

    fclose(arquivo);

    return valores;
}

// Função que realiza o merge de dois subarrays
void merge(int vet[], int esq, int meio, int dir) {
    int tamVet1 = meio - esq + 1;
    int tamVet2 = dir - meio;

    // Alocação dinâmica para os vetores temporários
    int *E = (int *)malloc(tamVet1 * sizeof(int));
    int *D = (int *)malloc(tamVet2 * sizeof(int));

    if (E == NULL || D == NULL) {
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
    while (i < tamVet1 && j < tamVet2) {
        if (E[i] <= D[j]) {
            vet[k] = E[i];
            i++;
        } else {
            vet[k] = D[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes do vetor E, se houver
    while (i < tamVet1) {
        vet[k] = E[i];
        i++;
        k++;
    }

    // Copia os elementos restantes do vetor D, se houver
    while (j < tamVet2) {
        vet[k] = D[j];
        j++;
        k++;
    }

    // Libera a memória alocada dinamicamente
    free(E);
    free(D);
}


// Função principal do Merge Sort
void organizacao(int vet[], int esq, int dir) {
    if (esq < dir) {
        int meio = esq + (dir - esq) / 2;

        // Ordena a primeira e a segunda metade
        organizacao(vet, esq, meio);
        organizacao(vet, meio + 1, dir);

        // Junta as metades ordenadas
        merge(vet, esq, meio, dir);
    }
}

int *executar_funcoes(FILE *arquivo, char *posidofile, int *valores, int *tamanhoatual) // essa função vai unir a de ler o arquivo e organizar os valores, para a thread fazer ambas
{

int *organizado = ler_arquivo(arquivo, posidofile, valores, tamanhoatual);


organizacao(organizado, 0, tamanhoatual - 1);

return organizado;

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

        pthread_create(&t1, NULL, (void *)executar_funcoes, (void *)arqv[2]);
        pthread_create(&t2, NULL, (void *)executar_funcoes, (void *)arqv[3]);

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