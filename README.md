Trabalho prático sobre a utilização de threads para disciplina TT304 turma A

Grupo GyGa Byte

Integrantes:

Giovanni da Silva Virginio Brandão

Gabriel Aleksandro

Descrição do problema

Foi proposto a criação de um programa em C, que por meio de multiplas threads conseguisse orderar os valores N números inteiros de N arquivos de entrada, em um único arquivo de saída.

Para compilação do programa, é necessário especificar o nome do arquivo, a quantidade de threads, o nome dos arquivos e suas extensões, o especificador "-o" e o nome do arquivo de saída também com sua extensão.

Exemplo:

./mergesort 4 arquivo1.txt arquivo2.txt arquivo3.txt ... arquivoN.txt -o saida.txt

O arquivo de saída deve conter todos os números presentes nos arquivos inseridos, ordenanos de forma crescente e separados por uma quebra de linha.