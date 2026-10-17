#include <stdio.h>
#include <stdlib.h>

// TODO: escrever headers, código e testes de
// 1. resolvedor de sistema Ax=b
// 2. cálculo de custos relativos
// 3. teste de otimalidade
// 4. direção simplex
// 5. tamanho do passo
// 6. nova partição básica

// os testes é fácil escrever, basta pegar um exemplo resolvido
// e pegar as entradas e saídas
// mas para isso precisa ter o header bonitinho.

int inicializar_input(double **c, double ***A, double **b, int *m, int *n) {
    FILE *input = fopen("./input.txt", "r");

    if (input == NULL) {
        perror("Error reading input.txt");
    }
    fscanf(input, "%d %d", m, n);


    *c = malloc(*n * sizeof(double));
    for (int j = 0; j < *n; j++) {
        fscanf(input, "%lf", &(*c)[j]);
    }


    *A = malloc(*m * sizeof(double *));
    for (int i = 0; i < *m; i++) {
        (*A)[i] = malloc(*n * sizeof(double));
        for (int j = 0; j < *n; j++) {
            fscanf(input, "%lf", &(*A)[i][j]);
        }
    }


    *b = malloc(*m * sizeof(double));
    for (int i = 0; i < *m; i++) {
        fscanf(input, "%lf", &(*b)[i]);
    }


    fclose(input);
    return 0;
}

int liberar_memoria(double *c, double **A, double *b, int m) {
    free(c);
    for (int i = 0; i < m; i++) {
        free(A[i]);
    }
    free(A);
    free(b);
    return 0;
}
// Vou implementar a fase I aqui, guardando quem está na base (B) e quem está fora (N)
void inicializador_fase1(int m, int n, int *indice_B, int *indice_N) {
    // As variáveis originais (0 até n-1) começam fora da base
    for(int i = 0; i < n; i++){
        indice_N[i] = i;
    }
    for(int i = 0; i < m; i++) {
        indice_B[i] = n + i;
    }
}
// Constrói a matriz inicial da Fase I juntando a Matriz A com a Matriz Identidade
void montar_quadro(int m, int n, double **quadro, double **A) {
    for(int i = 0; i < m; i++){
        for(int j = 0; j < (n + m); j++) {
            if(j < n){
                // Copia os coeficientes originais e converte de inteiro para decimal 
                quadro[i][j] = A[i][j]; // Converte o int dele para o teu float
            } else if(j == n + i){
                // 1.0 na diagonal da área das variáveis artificiais (Matriz Identidade)
                quadro[i][j] = 1.0;
            } else {
                quadro[i][j] = 0.0;
            }
        }
    }   
}

int main() {
    // a entrada será feita pelo arquivo input.txt
    // 1° entrada: 1 linha dizendo qual tamanho mxn da matriz
    // m são as restrições
    // n são a quantidade de variáveis
    // 2° entrada: 1 linha da matriz c^T
    // 3° entrada: a matriz A linha a linha
    // 4° entrada: 1 linha para a matriz b^T

    double *c, *b;
    double **A;
    int m, n;

    inicializar_input(&c, &A, &b, &m, &n);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf ", A[i][j]); // just to check, delete later
        }
        printf("\n");
    }
    // Alocação de memória para o quadro (m linhas por n+m colunas)
    double **quadro = malloc(m * sizeof(double *));
    for (int i = 0; i < m; i++) {
        quadro[i] = malloc((n + m) * sizeof(double));
    }
    // Alocação dos vetores de índices Básicos (B) e Não-Básicos (N)
    int *indice_B = malloc(m * sizeof(int));
    int *indice_N = malloc(n * sizeof(int));

    // Chama as funções para preencher os índices e montar a matriz
    inicializador_fase1(m, n, indice_B, indice_N);
    montar_quadro(m, n, quadro, A);

    // Gerando os custos, a função objetivo
    double *custos_fase1 = malloc((n + m) * sizeof(double));
    for (int j = 0; j < (n + m); j++) {
        // Custo 0.0 para variáveis
        if (j < n) {
            custos_fase1[j] = 0.0; 
        } else {
            // Custo 1.0 para variáveis artificiais
            custos_fase1[j] = 1.0; 
        }
    }

    // Verificar se o resultado deu certo
    printf("\n--- Quadro Inicial da Fase I ---\n");
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < (n + m); j++) {
            printf("%.1f\t", quadro[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < m; i++) free(quadro[i]);
    free(quadro);
    free(indice_B);
    free(indice_N);
    free(custos_fase1);

    liberar_memoria(c, A, b, m);
    return 0;

}
