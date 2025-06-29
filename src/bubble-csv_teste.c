#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_LINHA 1024
#define MAX_ELEMENTOS 100

void bubble_sort(char *ary[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (strcmp(ary[j], ary[j + 1]) > 0) {
                char *temp = ary[j];
                ary[j] = ary[j + 1];
                ary[j + 1] = temp;
            }
        }
    }
}

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
    char *elementos[MAX_ELEMENTOS];
    int contador = 0;

    if (fgets(linha, TAMANHO_LINHA, arquivo)) {
        //Strtok vai dividir a linha em elementos
        char *sub = strtok(linha, ",");
        while (sub && contador < MAX_ELEMENTOS) {
            elementos[contador++] = strdup(sub);
            sub = strtok(NULL, ",");
        }
    }

    fclose(arquivo);

   bubble_sort(elementos,contador);

   printf("Elementos ordenados:\n");
   for (int i = 0; i < contador; i++) {
       printf("%s\n", elementos[i]);
       free(elementos[i]); // isso vai liberar a memória alocada, segundo o golgue.
   }
   return 0;
}