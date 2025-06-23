#include <stdio.h>
#include <string.h>

#define TAMANHO_LINHA 1024

int main(int argc, char *argv[]) {
    FILE *arquivo;

    if (!argv[1]) {
        fprintf(stderr, "Uso: bubble-csv <nome do arquivo>\n");
        return 1;
    }

    arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo\n");
        return 1;
    }

    char linha[TAMANHO_LINHA];

    while (fgets(linha, TAMANHO_LINHA, arquivo)) printf("%s", linha);

    puts("");

    printf("%s\n", linha);

    char *sub = strtok(linha, ",");

    while (sub) {
        puts(sub);   
        sub = strtok(NULL, ",");
    }
}