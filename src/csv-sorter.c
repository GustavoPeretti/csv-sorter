#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constantes para limitar tamanhos máximos
#define TAMANHO_LINHA 1024
#define MAX_LINHAS 40000
#define MAX_COLUNAS 10000

// Estrutura que representa uma linha do CSV
typedef struct {
    char **campos;       // Vetor de strings, cada campo da linha
    int num_campos;      // Quantos campos existem na linha
    int cabecalho;       // Marca se esta linha é o cabeçalho
} LinhaCSV;

// Função para remover espaços em branco no início e fim de uma string
char* trim_whitespace(char* str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;  // avança até o primeiro não-espaço

    if(*str == 0) return str; // string toda em branco

    end = str + strlen(str) - 1;

    while(end > str && isspace((unsigned char)*end)) end--; // retrocede até o último não-espaço

    *(end+1) = 0; // termina a string

    return str;
}

// Verifica se uma string é um número válido (int ou float)
int eh_numero(const char *str) {
    if (str == NULL || *str == '\0') return 0;

    int pontos = 0;
    int digitos = 0;
    const char *ptr = str;

    if (*ptr == '-' || *ptr == '+') ptr++; // ignora sinal

    while (*ptr) {
        if (*ptr == '.') {
            if (++pontos > 1) return 0; // só pode um ponto decimal
        } else if (!isdigit(*ptr)) {
            return 0;
        } else {
            digitos = 1;
        }
        ptr++;
    }
    return digitos;
}

// Compara dois campos levando em conta tipo e ordem (asc/desc)
int comparar_campos(const char *str1, const char *str2, int ordem_crescente) {
    if (str1 == NULL || str2 == NULL) return 0;

    // Caso uma das strings esteja vazia
    if (strlen(str1) == 0 && strlen(str2) > 0) return ordem_crescente ? 1 : -1;
    if (strlen(str2) == 0 && strlen(str1) > 0) return ordem_crescente ? -1 : 1;

    int str1_numero = eh_numero(str1);
    int str2_numero = eh_numero(str2);

    // Comparação numérica
    if (str1_numero && str2_numero) {
        double num1 = atof(str1);
        double num2 = atof(str2);
        if (num1 > num2) return ordem_crescente ? 1 : -1;
        if (num1 < num2) return ordem_crescente ? -1 : 1;
        return 0;
    }
    // Números sempre têm prioridade sobre texto
    else if (str1_numero) return -1;
    else if (str2_numero) return 1;

    // Comparação alfabética
    return ordem_crescente ? strcmp(str1, str2) : strcmp(str2, str1);
}

// Função que aplica Bubble Sort nas linhas (exceto cabeçalho)
void bubble_sort_dados_linhas(LinhaCSV *linhas, int n, int coluna_alvo, int ordem_crescente) {
    for (int i = 1; i < n - 1; i++) {
        for (int j = 1; j < n - i; j++) {
            // Ignora se alguma linha não tem a coluna desejada
            if (linhas[j].num_campos <= coluna_alvo || linhas[j + 1].num_campos <= coluna_alvo) {
                continue;
            }

            const char *v1 = linhas[j].campos[coluna_alvo];
            const char *v2 = linhas[j + 1].campos[coluna_alvo];

            // Faz troca se fora da ordem
            if (comparar_campos(v1, v2, ordem_crescente) > 0) {
                LinhaCSV temp = linhas[j];
                linhas[j] = linhas[j + 1];
                linhas[j + 1] = temp;
            }
        }
    }
}

// Imprime uma linha formatada na tela (alinhada)
void imprimir_linha_formatada(const LinhaCSV *linha) {
    for (int i = 0; i < linha->num_campos; i++) {
        printf("%s", linha->campos[i]);
        if (i < linha->num_campos - 1) printf(",");
    }
    printf("\n");
}

// Libera toda a memória usada
void liberar_memoria(LinhaCSV *linhas, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < linhas[i].num_campos; j++) {
            free(linhas[i].campos[j]);
        }
        free(linhas[i].campos);
    }
    free(linhas);
}

// Função principal
int main(int argc, char *argv[]) {
    // Verifica argumentos: nome do programa + 3 argumentos
    if (argc != 4) {
        printf("Uso: %s <arquivo.csv> <coluna_para_ordenar> <ordem: asc|desc>\n", argv[0]);
        return 1;
    }

    int coluna_alvo = atoi(argv[2]); // coluna a ordenar
    int ordem_crescente;

    // Interpreta o terceiro argumento
    if (strcmp(argv[3], "asc") == 0) {
        ordem_crescente = 1;
    } else if (strcmp(argv[3], "desc") == 0) {
        ordem_crescente = 0;
    } else {
        printf("Erro: ordem inválida. Use 'asc' ou 'desc'.\n");
        return 1;
    }

    // Abre o arquivo
    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    // Aloca estrutura de linhas
    LinhaCSV *linhas = (LinhaCSV *)malloc(MAX_LINHAS * sizeof(LinhaCSV));
    if (linhas == NULL) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        return 1;
    }

    char linha_buffer[TAMANHO_LINHA];
    int num_linhas = 0;

    // Lê o cabeçalho
    if (fgets(linha_buffer, TAMANHO_LINHA, arquivo) == NULL) {
        printf("Erro: Arquivo vazio\n");
        fclose(arquivo);
        free(linhas);
        return 1;
    }

    // Remove o '\n' e separa os campos do cabeçalho
    linha_buffer[strcspn(linha_buffer, "\n")] = '\0';
    linhas[num_linhas].num_campos = 0;
    linhas[num_linhas].campos = malloc(MAX_COLUNAS * sizeof(char *));
    linhas[num_linhas].cabecalho = 1;

    char *token = strtok(linha_buffer, ",");
    while (token != NULL && linhas[num_linhas].num_campos < MAX_COLUNAS) {
        linhas[num_linhas].campos[linhas[num_linhas].num_campos] = strdup(trim_whitespace(token));
        linhas[num_linhas].num_campos++;
        token = strtok(NULL, ",");
    }
    num_linhas++;

    // Lê cada linha de dados
    while (num_linhas < MAX_LINHAS && fgets(linha_buffer, TAMANHO_LINHA, arquivo) != NULL) {
        linha_buffer[strcspn(linha_buffer, "\n")] = '\0';

        if (strlen(linha_buffer) > 0) {
            linhas[num_linhas].num_campos = 0;
            linhas[num_linhas].campos = malloc(MAX_COLUNAS * sizeof(char *));
            linhas[num_linhas].cabecalho = 0;

            token = strtok(linha_buffer, ",");
            while (token != NULL && linhas[num_linhas].num_campos < MAX_COLUNAS) {
                linhas[num_linhas].campos[linhas[num_linhas].num_campos] = strdup(trim_whitespace(token));
                linhas[num_linhas].num_campos++;
                token = strtok(NULL, ",");
            }
            num_linhas++;
        }
    }

    fclose(arquivo); // Fecha o arquivo após leitura

    // Verifica se a coluna é válida
    if (coluna_alvo < 0 || coluna_alvo >= linhas[0].num_campos) {
        printf("Erro: Coluna %d inválida. O arquivo tem %d colunas.\n", coluna_alvo, linhas[0].num_campos);
        liberar_memoria(linhas, num_linhas);
        return 1;
    }

    // Ordena as linhas (exceto cabeçalho)
    bubble_sort_dados_linhas(linhas, num_linhas, coluna_alvo, ordem_crescente);

    // Imprime o resultado
    for (int i = 0; i < num_linhas; i++) {
        imprimir_linha_formatada(&linhas[i]);
    }

    liberar_memoria(linhas, num_linhas); // Libera memória
    return 0;
}