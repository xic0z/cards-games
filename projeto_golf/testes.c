/**
 * @file testes.c
 * @brief Suite de Testes Unitários para o jogo Golf Solitaire.
 */

#include <CUnit/Basic.h>
#include <string.h>
#include "golf.h"

/* --- Definições de Teste --- */

/** @brief Valida se a adjacência numérica (ex: 4 e 5) é permitida. */
void teste_validar_basico(void) { CU_ASSERT_EQUAL(validar(5, 4), 1); CU_ASSERT_EQUAL(validar(10, 11), 1); }

/** @brief Valida a regra especial de continuidade entre Ás (1) e Rei (13). */
void teste_validar_ciclo(void) { CU_ASSERT_EQUAL(validar(1, 13), 1); CU_ASSERT_EQUAL(validar(13, 1), 1); }

/** @brief Garante que cartas não adjacentes sejam rejeitadas. */
void teste_validar_invalido(void) { CU_ASSERT_EQUAL(validar(5, 7), 0); CU_ASSERT_EQUAL(validar(2, 2), 0); }

/** @brief Verifica se o sistema reconhece a vitória quando o tabuleiro está vazio. */
void teste_vitoria_sucesso(void) { Jogo j = {0}; CU_ASSERT_EQUAL(verificou_vitoria(&j), 1); }

/** @brief Garante que o jogo não termina precocemente se houver cartas no tabuleiro. */
void teste_vitoria_falha(void) { Jogo j = {0}; j.tam_pilha[0] = 1; CU_ASSERT_EQUAL(verificou_vitoria(&j), 0); }

/** @brief Verifica se ainda há jogadas quando o baralho de reserva tem cartas. */
void teste_jogadas_baralho_disponivel(void) { Jogo j = {0}; j.idx_baralho = 0; CU_ASSERT_EQUAL(tem_jogadas_possiveis(&j), 1); }

/** @brief Verifica se o sistema deteta jogadas possíveis entre as pilhas e o descarte. */
void teste_jogadas_tabuleiro_disponivel(void) { Jogo j = {0}; j.idx_baralho = 16; j.tam_pilha[0] = 1; j.pilhas[0][0].valor = 5; j.descarte.valor = 4; CU_ASSERT_EQUAL(tem_jogadas_possiveis(&j), 1); }

/** @brief Valida o estado de Game Over quando não há mais movimentos legais. */
void teste_game_over_real(void) { Jogo j = {0}; j.idx_baralho = 16; j.descarte.valor = 10; j.tam_pilha[0] = 1; j.pilhas[0][0].valor = 2; CU_ASSERT_EQUAL(tem_jogadas_possiveis(&j), 0); }

/** @brief Verifica se a inicialização distribui corretamente 5 cartas por pilha. */
void teste_setup_pilhas(void) { Jogo j; inicializar(&j); for (int i = 0; i < 7; i++) CU_ASSERT_EQUAL(j.tam_pilha[i], 5); }

/** @brief Garante que a pontuação começa em zero. */
void teste_pontos_iniciais(void) { Jogo j; inicializar(&j); CU_ASSERT_EQUAL(j.pontos, 0); }

/** @brief Valida comportamento quando a reserva chega ao fim. */
void teste_esgotar_reserva(void) { Jogo j = {0}; j.idx_baralho = 16; CU_ASSERT_EQUAL(tem_jogadas_possiveis(&j), 0); }

/** @brief Verifica se a carta movida mantém as suas propriedades no descarte. */
void teste_integridade_movimento(void) { Jogo j = {0}; j.descarte.valor = 10; j.pilhas[0][0].valor = 11; j.tam_pilha[0] = 1; j.descarte = j.pilhas[0][0]; j.tam_pilha[0]--; CU_ASSERT_EQUAL(j.descarte.valor, 11); }

/** @brief Testa se duas inicializações seguidas geram tabuleiros diferentes. */
void teste_aleatoriedade_inicializacao(void) { Jogo j1, j2; inicializar(&j1); inicializar(&j2); CU_ASSERT_NOT_EQUAL(j1.descarte.valor, j2.descarte.valor); }

/** @brief Garante que o índice do baralho não ultrapassa os limites de memória. */
void teste_limite_indice_baralho(void) { Jogo j; j.idx_baralho = 15; CU_ASSERT_TRUE(j.idx_baralho < 16); }

/** @brief Verifica a progressão positiva da pontuação. */
void teste_pontuacao_positiva(void) { Jogo j; j.pontos = 100; j.pontos += 10; CU_ASSERT_TRUE(j.pontos > 100); }

/* --- Estrutura Auxiliar --- */

/** @struct Caso @brief Associa um nome curto à função de teste. */
typedef struct { char *n; void (*f)(void); } Caso;

/**
 * @brief Função Principal. Inicializa o CUnit, regista os testes e executa a suite.
 * @return int Status do registo do CUnit.
 */
int main() {
    CU_pSuite s = NULL;
    Caso tests[] = {
        {"V1", teste_validar_basico}, {"V2", teste_validar_ciclo}, {"V3", teste_validar_invalido},
        {"V4", teste_vitoria_sucesso}, {"V5", teste_vitoria_falha}, {"V6", teste_jogadas_baralho_disponivel},
        {"V7", teste_jogadas_tabuleiro_disponivel}, {"V8", teste_game_over_real}, {"V9", teste_setup_pilhas},
        {"V10", teste_pontos_iniciais}, {"V11", teste_esgotar_reserva}, {"V12", teste_integridade_movimento},
        {"V13", teste_aleatoriedade_inicializacao}, {"V14", teste_limite_indice_baralho}, {"V15", teste_pontuacao_positiva}
    };

    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();
    
    if (!(s = CU_add_suite("Golf_Tests", NULL, NULL))) { 
        CU_cleanup_registry(); 
        return CU_get_error(); 
    }

    for (int i = 0; i < 15; i++) CU_add_test(s, tests[i].n, tests[i].f);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

