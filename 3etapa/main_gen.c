/**
 * @file main_gen.c
 * @brief Ponto de entrada do programa genérico de paciências.
 */
#include "paciencia.h"
#include "parser.h"
#define PASTA_PACIENCIAS "paciencias"
#define MAX_FICHEIROS     32

/* ── funções existentes (sem alterações) ─────────────────────────── */

static void construir_path(char *buf, int tam, const char *nome) {
    snprintf(buf, tam, "%s/%s", PASTA_PACIENCIAS, nome);
}

static int carregar_paciencia(char lista[][MAX_NOME], int idx, Jogo *j) {
    char path[256];
    construir_path(path, sizeof(path), lista[idx]);
    if (!parser_ler(j, path)) return 0;
    logica_distribuir(j);
    logica_executar_autos(j);
    return 1;
}

static void mostrar_erro_pasta(void) {
    printf("Erro: pasta '%s' não encontrada ou sem ficheiros .paciencia\n",
           PASTA_PACIENCIAS);
    printf("Cria a pasta e coloca lá ficheiros .paciencia.\n");
}

/* ── novas funções auxiliares da main ───────────────────────────── */

/**
 * @brief Executa uma ronda: pede escolha, carrega e joga.
 * @return 1 para continuar, 0 para sair.
 */
static int executar_ronda(char lista[][MAX_NOME], int n, Jogo *j) {
    int escolha = ui_menu_selecao(lista, n);
    if (escolha < 0) return 0;
    if (carregar_paciencia(lista, escolha, j))
        ui_executar_jogo(j);
    else
        printf(RED "Erro ao carregar '%s'.\n" RESET, lista[escolha]);
    return 1;
}

/**
 * @brief Aloca o jogo e corre o ciclo principal.
 * @return 0 em sucesso, 1 em erro de alocação.
 */
static int correr_ciclo(char lista[][MAX_NOME], int n) {
    Jogo *j = (Jogo *)malloc(sizeof(Jogo));
    if (j == NULL) {
        printf(RED "Erro fatal: Falha ao alocar memória para o jogo.\n" RESET);
        return 1;
    }
    while (executar_ronda(lista, n, j));
    free(j);
    printf(GRN "Até à próxima!\n" RESET);
    return 0;
}

/* ── main ────────────────────────────────────────────────────────── */

/**
 * @brief Ponto de entrada. Configura o ambiente e gere o ciclo principal.
 * @return 0 em caso de sucesso.
 */
int main(void) {
    setlocale(LC_ALL, "en_US.UTF-8");
    srand((unsigned)time(NULL));
    char lista[MAX_FICHEIROS][MAX_NOME];
    int n = parser_listar(PASTA_PACIENCIAS, lista, MAX_FICHEIROS);
    if (n <= 0) { mostrar_erro_pasta(); return 1; }
    return correr_ciclo(lista, n);
}
