#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 100

int main() {
    FILE *file;

    char linha[MAX_LINE_LENGTH];
    char campos[MAX_FIELDS];
    int i;


    file = fopen("nome do arquivo", "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo\n");
        return 1;
    }


    while (fgets(linha, sizeof(linha), file)) {
        for (i = 0; i < MAX_FIELDS; i++) {
            campos[i] = NULL;
        }
    }

    char *token = strtok(linha, ",");
    i = 0;
    while (token != NULL && i < MAX_FIELDS) {
        campos[i++] = token;
        token = strtok(NULL, ",");
    }

    for(int j; j < i; j++) {
        printf("%s\n", campos[j]);
    }
    
    printf("\n");

    fclose(file);
    return 0;
}