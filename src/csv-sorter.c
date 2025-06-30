#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_LINHA 1024
#define MAX_LINHAS 40000

// Estrutura para armazenar os dados de uma linha de forma mais organizada
typedef struct {
    char *gender;
    double hours_studied;
    char *tutoring;
    char *region;
    double attendance_percent;
    char *parent_education;
    double exam_score;
} StudentData;

// Função para remover espaços em branco do início e fim de uma string
char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end+1) = 0;

    return str;
}

// Função para extrair um valor numérico de uma coluna específica da linha
// A linha original não é modificada (const char*)
double extrair_valor_coluna(const char *linha, int coluna_alvo) {
    if (linha == NULL || strlen(linha) == 0) {
        return 0.0;
    }

    // Cria uma cópia da linha para que strtok possa modificá-la
    char temp_linha[TAMANHO_LINHA];
    strncpy(temp_linha, linha, TAMANHO_LINHA - 1);
    temp_linha[TAMANHO_LINHA - 1] = '\0'; // Garante terminação nula

    char *token;
    double valor = 0.0;
    int coluna_atual = 0;

    // Percorre os tokens da linha
    token = strtok(temp_linha, ",");
    while (token != NULL) {
        if (coluna_atual == coluna_alvo) {
            valor = atof(token); // Converte o token para double
            break; // Encontrou o valor, pode sair
        }
        token = strtok(NULL, ","); // Pega o próximo token
        coluna_atual++;
    }
    return valor;
}

// Função para ordenar os dados usando Bubble Sort
// Agora recebe um ponteiro para ponteiros de char (char**) para as linhas
// e um ponteiro para double (double*) para os valores
void bubble_sort_dados_alunos(char **dados, double *valores, int n) {
    if (n <= 1) return; // Não há nada para ordenar se houver 0 ou 1 linha

    for (int i = 0; i < n - 1; i++) {
        int trocado = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (valores[j] > valores[j + 1]) {
                // Troca os valores numéricos
                double temp_valor = valores[j];
                valores[j] = valores[j + 1];
                valores[j + 1] = temp_valor;

                // Troca os PONTEIROS para as linhas correspondentes
                // Isso é muito mais eficiente do que copiar strings grandes
                char *temp_linha_ptr = dados[j];
                dados[j] = dados[j + 1];
                dados[j + 1] = temp_linha_ptr;

                trocado = 1; // Indica que houve uma troca nesta passagem
            }
        }
        if (!trocado) {
            break; // Se nenhuma troca foi feita, o array já está ordenado
        }
    }
}

// Função para imprimir uma linha formatada em colunas
void imprimir_linha_formatada(const char *linha_csv) {
    char temp_linha[TAMANHO_LINHA];
    strncpy(temp_linha, linha_csv, TAMANHO_LINHA - 1);
    temp_linha[TAMANHO_LINHA - 1] = '\0';

    char *token;
    char *fields[7]; // Array para armazenar os campos da linha
    int field_count = 0;

    // Extrai todos os campos da linha
    token = strtok(temp_linha, ",");
    while (token != NULL && field_count < 7) {
        fields[field_count] = trim_whitespace(token); // Remove espaços em branco
        field_count++;
        token = strtok(NULL, ",");
    }

    // Imprime as 7 primeiras colunas formatadas
    // Ajuste os %-Xs para o tamanho desejado de cada coluna
    // Ex: %-10s para string alinhada à esquerda com 10 caracteres
    // Ex: %-8.1f para float com 1 casa decimal, alinhado à esquerda com 8 caracteres
    if (field_count >= 7) { // Garante que há pelo menos 7 campos
        printf("%-20s %-20s %-20s %-20s %-20s %-20s %-20s\n",
               fields[0], // Gender
               fields[1], // HoursStudied/Week (como string, para manter o alinhamento)
               fields[2], // Tutoring
               fields[3], // Region
               fields[4], // Attendance(%) (como string, para manter o alinhamento)
               fields[5], // Parent Education
               fields[6]  // Exam Score
              );
    } else {
        // Caso a linha não tenha campos suficientes, imprime a linha original ou um erro
        printf("Linha mal formatada: %s\n", linha_csv);
    }
}


int main(int argc, char *argv[]) {
    // Verifica se o número correto de argumentos foi fornecido
    if (argc != 3) {
        printf("Uso: %s <arquivo.csv> <coluna_para_ordenar>\n", argv[0]);
        printf("Exemplo: %s alunos.csv 6 (para ordenar por nota)\n", argv[0]);
        return 1; // Retorna código de erro
    }

    int coluna_alvo = atoi(argv[2]); // Converte o argumento da coluna para inteiro
    FILE *arquivo = fopen(argv[1], "r"); // Tenta abrir o arquivo

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro: Arquivo '%s' não encontrado ou não pode ser aberto.\n", argv[1]);
        return 1; // Retorna código de erro
    }

    char linha_buffer[TAMANHO_LINHA]; // Buffer temporário para ler cada linha do arquivo

    // ALOCAÇÃO DINÂMICA DE MEMÓRIA PARA AS LINHAS E VALORES
    // 'dados' será um array de ponteiros para char (char**), onde cada ponteiro apontará para uma linha (string)
    char **dados = (char **)malloc(MAX_LINHAS * sizeof(char *));
    // 'valores' será um array de doubles (double*)
    double *valores = (double *)malloc(MAX_LINHAS * sizeof(double));

    // Verifica se a alocação de memória foi bem-sucedida
    if (dados == NULL || valores == NULL) {
        perror("Erro ao alocar memoria dinamica");
        // Libera qualquer memória que possa ter sido alocada antes de sair
        if (dados != NULL) free(dados);
        if (valores != NULL) free(valores);
        fclose(arquivo);
        return 1; // Retorna código de erro
    }

    int num_linhas = 0; // Contador de linhas lidas e armazenadas

    // Lê e armazena o cabeçalho do arquivo (primeira linha)
    if (fgets(linha_buffer, TAMANHO_LINHA, arquivo) == NULL) {
        printf("Erro: Arquivo vazio ou erro de leitura do cabeçalho.\n");
        // Libera a memória alocada antes de sair
        free(dados);
        free(valores);
        fclose(arquivo);
        return 1; // Retorna código de erro
    }
    // Remove a quebra de linha do cabeçalho
    linha_buffer[strcspn(linha_buffer, "\n")] = '\0';

    // Imprime o cabeçalho formatado
    imprimir_linha_formatada(linha_buffer); // Imprime o cabeçalho formatado

    // Lê cada linha restante do arquivo até o fim ou até MAX_LINHAS
    while (fgets(linha_buffer, TAMANHO_LINHA, arquivo) != NULL) {
        // Remove o caractere de nova linha ('\n') do final da string, se existir
        linha_buffer[strcspn(linha_buffer, "\n")] = '\0';

        // Processa a linha apenas se não for vazia
        if (strlen(linha_buffer) > 0) {
            // Aloca memória para a string da linha atual
            // strlen(linha_buffer) + 1 para incluir o caractere nulo de terminação '\0'
            dados[num_linhas] = (char *)malloc(strlen(linha_buffer) + 1);

            // Verifica se a alocação para a linha individual foi bem-sucedida
            if (dados[num_linhas] == NULL) {
                perror("Erro ao alocar memoria para linha individual");
                // Libera toda a memória alocada até o momento antes de sair
                for (int i = 0; i < num_linhas; i++) {
                    free(dados[i]);
                }
                free(dados);
                free(valores);
                fclose(arquivo);
                return 1; // Retorna código de erro
            }

            // Copia o conteúdo do buffer temporário para a memória alocada dinamicamente
            strcpy(dados[num_linhas], linha_buffer);

            // Extrai o valor da coluna alvo da linha e armazena no array 'valores'
            valores[num_linhas] = extrair_valor_coluna(linha_buffer, coluna_alvo);

            num_linhas++; // Incrementa o contador de linhas processadas
        }
    }
    fclose(arquivo); // Fecha o arquivo após a leitura

    // Ordena os dados usando o Bubble Sort
    bubble_sort_dados_alunos(dados, valores, num_linhas);

    // Exibe os resultados ordenados, linha por linha, formatados
    for (int i = 0; i < num_linhas; i++) {
        imprimir_linha_formatada(dados[i]);
    }

    // LIBERAÇÃO DA MEMÓRIA ALOCADA DINAMICAMENTE
    // É crucial liberar toda a memória para evitar vazamentos (memory leaks)
    for (int i = 0; i < num_linhas; i++) {
        free(dados[i]); // Libera a memória de cada string de linha individual
    }
    free(dados);   // Libera a memória do array de ponteiros para as linhas
    free(valores); // Libera a memória do array de valores numéricos

    return 0; // Retorna 0 para indicar sucesso
}
