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

void ler_arquivo(char *arqv)
{
/*FILE *arquivo;


arquivo = fopen(arqv, "r+");*/

printf("Thread do arquivo %s\n", arqv);
    
}

int main(int arqc, char *arqv[])
{

    int qnt = 0, i = 0;
    
    qnt = atoi(arqv[1]); //quantidade de threads que serão usadas

if(qnt == 2) {

pthread_create(&t1, NULL, (void *) ler_arquivo, (void *) arqv[2]);
pthread_create(&t2, NULL, (void *) ler_arquivo, (void *) arqv[3]);


pthread_join(t1, NULL);
pthread_join(t2, NULL);
printf("Fim\n");

}

else if(qnt == 4){

pthread_create(&t1, NULL, (void *)ler_arquivo, (void *) arqv[2]);
pthread_create(&t2, NULL, (void *)ler_arquivo, (void *) arqv[3]);
pthread_create(&t3, NULL, (void *)ler_arquivo, (void *) arqv[4]);
pthread_create(&t4, NULL, (void *)ler_arquivo, (void *) arqv[5]);

pthread_join(t1, NULL);
pthread_join(t2, NULL);
pthread_join(t3, NULL);
pthread_join(t4, NULL);
printf("Fim\n");
}


else if(qnt == 8){

pthread_create(&t1, NULL, (void *)ler_arquivo, (void *) arqv[2]);
pthread_create(&t2, NULL, (void *)ler_arquivo, (void *) arqv[3]);
pthread_create(&t3, NULL, (void *)ler_arquivo, (void *) arqv[4]);
pthread_create(&t4, NULL, (void *)ler_arquivo, (void *) arqv[5]);
pthread_create(&t5, NULL, (void *)ler_arquivo, (void *) arqv[6]);
pthread_create(&t6, NULL, (void *)ler_arquivo, (void *) arqv[7]);
pthread_create(&t7, NULL, (void *)ler_arquivo, (void *) arqv[8]);
pthread_create(&t8, NULL, (void *)ler_arquivo, (void *) arqv[9]);

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
    



    return 0;
}