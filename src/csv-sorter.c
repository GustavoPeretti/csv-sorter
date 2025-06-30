#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TAMANHO_LINHA 1024
#define MAX_LINHAS 40000
#define MAX_COLUNAS 10000

typedef struct {
    char **campos;
    int num_campos;
    bool eh_cabecalho;
} LinhaCSV;

void liberar_memoria(LinhaCSV *linhas, int n);

char* trim_whitespace(char* str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = 0;
    return str;
}

bool eh_numero(const char *str) {
    if (str == NULL || *str == '\0') return false;

    int pontos = 0;
    bool digitos = false;
    const char *ptr = str;

    if (*ptr == '-' || *ptr == '+') ptr++;

    while (*ptr) {
        if (*ptr == '.') {
            if (++pontos > 1) return false;
        } else if (!isdigit(*ptr)) {
            return false;
        } else {
            digitos = true;
        }
        ptr++;
    }
    return digitos;
}

int comparar_campos(const char *str1, const char *str2) {
    bool str1_numero = eh_numero(str1);
    bool str2_numero = eh_numero(str2);

    if (str1_numero && str2_numero) {
        double num1 = atof(str1);
        double num2 = atof(str2);
        if (num1 > num2) return -1; // decrescente
        if (num1 < num2) return 1;
        return 0;
    } else if (str1_numero) {
        return -1;
    } else if (str2_numero) {
        return 1;
    } else {
        return strcmp(str2, str1); // ordem alfabética decrescente
    }
}

void bubble_sort_dados_linhas(LinhaCSV *linhas, int n, int coluna_alvo) {
    for (int i = 1; i < n - 1; i++) { // começa no índice 1, pula o cabeçalho
        for (int j = 1; j < n - i; j++) {
            if (comparar_campos(linhas[j].campos[coluna_alvo],
                                linhas[j+1].campos[coluna_alvo]) > 0) {
                LinhaCSV temp = linhas[j];
                linhas[j] = linhas[j + 1];
                linhas[j + 1] = temp;
            }
        }
    }
}

void imprimir_linha_formatada(const LinhaCSV *linha) {
    for (int i = 0; i < linha->num_campos; i++) {
        printf("%-20s ", linha->campos[i]);
    }
    printf("\n");
}

void liberar_memoria(LinhaCSV *linhas, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < linhas[i].num_campos; j++) {
            free(linhas[i].campos[j]);
        }
        free(linhas[i].campos);
    }
    free(linhas);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo.csv> <coluna_para_ordenar>\n", argv[0]);
        printf("Colunas começam em 0 (0 = primeira coluna)\n");
        return 1;
    }

    int coluna_alvo = atoi(argv[2]);
    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    LinhaCSV *linhas = (LinhaCSV *)malloc(MAX_LINHAS * sizeof(LinhaCSV));
    if (linhas == NULL) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        return 1;
    }

    char linha_buffer[TAMANHO_LINHA];
    int num_linhas = 0;

    // Processa o cabeçalho
    if (fgets(linha_buffer, TAMANHO_LINHA, arquivo) == NULL) {
        printf("Erro: Arquivo vazio\n");
        fclose(arquivo);
        free(linhas);
        return 1;
    }

    linha_buffer[strcspn(linha_buffer, "\n")] = '\0';
    linhas[num_linhas].num_campos = 0;
    linhas[num_linhas].campos = malloc(MAX_COLUNAS * sizeof(char *));
    linhas[num_linhas].eh_cabecalho = true;

    char *token = strtok(linha_buffer, ",");
    while (token != NULL && linhas[num_linhas].num_campos < MAX_COLUNAS) {
        linhas[num_linhas].campos[linhas[num_linhas].num_campos] = strdup(trim_whitespace(token));
        linhas[num_linhas].num_campos++;
        token = strtok(NULL, ",");
    }
    num_linhas++;

    // Processa as linhas de dados
    while (num_linhas < MAX_LINHAS && fgets(linha_buffer, TAMANHO_LINHA, arquivo) != NULL) {
        linha_buffer[strcspn(linha_buffer, "\n")] = '\0';
        if (strlen(linha_buffer) > 0) {
            linhas[num_linhas].num_campos = 0;
            linhas[num_linhas].campos = malloc(MAX_COLUNAS * sizeof(char *));
            linhas[num_linhas].eh_cabecalho = false;

            token = strtok(linha_buffer, ",");
            while (token != NULL && linhas[num_linhas].num_campos < MAX_COLUNAS) {
                linhas[num_linhas].campos[linhas[num_linhas].num_campos] = strdup(trim_whitespace(token));
                linhas[num_linhas].num_campos++;
                token = strtok(NULL, ",");
            }
            num_linhas++;
        }
    }
    fclose(arquivo);

    if (coluna_alvo < 0 || coluna_alvo >= linhas[0].num_campos) {
        printf("Erro: Coluna %d inválida. O arquivo tem %d colunas.\n", coluna_alvo, linhas[0].num_campos);
        liberar_memoria(linhas, num_linhas);
        return 1;
    }

    bubble_sort_dados_linhas(linhas, num_linhas, coluna_alvo);

    // Imprime todas as linhas (cabeçalho primeiro)
    for (int i = 0; i < num_linhas; i++) {
        imprimir_linha_formatada(&linhas[i]);
    }

    liberar_memoria(linhas, num_linhas);
    return 0;
}
