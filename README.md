**===Trabalho prático sobre a utilização de threads para disciplina TT304 turma A===**

Grupo GyGa Byte

Integrantes:

Giovanni da Silva Virginio Brandão

Gabriel Aleksandro de Paula


**===Sistema de Ordenação com Múltiplas Threads===**

**===Descrição do Projeto:===**

O projeto consiste em um programa em C que utiliza POSIX Threads para criar um sistema de ordenação com o algoritmo Mergesort. O programa distribui o trabalho entre múltiplas threads para ler, ordenar e mesclar dados de arquivos de entrada, buscando avaliar o impacto da quantidade de threads na eficiência do processamento.

**===Funcionalidades===**

* Leitura de Arquivos: As threads leem valores numéricos de múltiplos arquivos de entrada especificados pelo usuário.
* Ordenação Paralela: Cada thread utiliza o algoritmo Mergesort para ordenar sua porção de dados.
* Mesclagem Final: Os dados ordenados por cada thread são combinados em um único vetor e reordenados para garantir a sequência correta.
* Medição de Desempenho: O tempo de execução de cada thread e o tempo total do programa são registrados para análise de desempenho.

**===Pré-requisitos===**

* Sistema Operacional: Linux (requer POSIX Threads).
* Compilador C: GCC (GNU Compiler Collection).

**===Compilação===**

Para compilar o programa, abra o terminal na pasta do projeto e execute sem as aspas:

"make"

**===Execução===**

O programa requer os seguintes argumentos de linha de comando:

"./mergesort_multithread <número_de_threads> <arquivos_de_entrada> -o <arquivo_de_saida>"

* <número_de_threads>: Quantidade de threads a serem usadas.
* <arquivos_de_entrada>: Lista de arquivos contendo números a serem ordenados.
* -o <arquivo_de_saida>: Nome do arquivo onde o resultado final será salvo.

**Exemplo:**
"./mergesort_multithread 4 input1.txt input2.txt input3.txt -o output.txt"

**Uso:**
1. Prepare arquivos de entrada contendo números inteiros, cada número em uma linha.
2. Execute o programa conforme os comandos de exemplo acima.
3. O programa exibirá o tempo de execução de cada thread e salvará os dados ordenados no arquivo de saída especificado.

**Estrutura de Arquivos:**
* mergesort.c: Código principal do projeto.
* input*.txt: Arquivos de entrada (exemplo).
* output.txt: Arquivo de saída, onde os dados ordenados serão salvos.
