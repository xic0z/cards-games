#include <stdio.h>
#include <CUnit/Basic.h>
#include "simon.h"

/* Protótipos de funções que estão no logica.c mas podem não estar no simon.h.
   Isto evita o erro de "implicit declaration".
*/
void criar_deck(Carta deck[]);
void distribuir(Jogo *j, Carta deck[]);

/* --- Suite 1: Inicialização e Baralho --- */

/**
 * @brief Verifica se o baralho é criado com o tamanho e ordenação base corretos.
 * Testa se a primeira carta é um Ás (1) e a última um Rei (13).
 */
void teste_deck_completo(void) {
    Carta deck[52];
    criar_deck(deck);
    CU_ASSERT_EQUAL(deck[0].valor, 1);
    CU_ASSERT_EQUAL(deck[51].valor, 13);
}

/**
 * @brief Valida se a distribuição inicial respeita os limites de cada coluna.
 * Verifica se a primeira coluna recebe 8 cartas e a última apenas 1.
 */
void teste_distribuicao_limites(void) {
    Jogo j;
    Carta deck[52];
    criar_deck(deck);
    distribuir(&j, deck);
    CU_ASSERT_EQUAL(j.tam_pilha[0], 8);
    CU_ASSERT_EQUAL(j.tam_pilha[9], 1);
}

/**
 * @brief Testa o estado inicial do objeto Jogo.
 * Garante que o jogo começa em execução.
 */
void teste_estado_inicial(void) {
    Jogo j;
    inicializar(&j);
    CU_ASSERT_EQUAL(j.a_correr, 1);
}

/* --- Suite 2: Validação de Sequências --- */

/**
 * @brief Valida uma sequência simples e correta.
 * Verifica se duas cartas do mesmo naipe em ordem decrescente (5 e 4) são aceites.
 */
void teste_sequencia_valida_simples(void) {
    Jogo j;
    j.tam_pilha[0] = 2;
    j.pilhas[0][0] = (Carta){5, COPAS};
    j.pilhas[0][1] = (Carta){4, COPAS};
    CU_ASSERT_TRUE(validar_sequencia(&j, 0, 2));
}

/**
 * @brief Testa a rejeição de sequências com naipes diferentes.
 * Uma sequência no Simple Simon deve obrigatoriamente partilhar o mesmo naipe para ser movida em bloco.
 */
void teste_sequencia_naipe_diferente(void) {
    Jogo j;
    j.tam_pilha[0] = 2;
    j.pilhas[0][0] = (Carta){5, COPAS};
    j.pilhas[0][1] = (Carta){4, ESPADAS};
    CU_ASSERT_FALSE(validar_sequencia(&j, 0, 2));
}

/**
 * @brief Testa a rejeição de sequências com valores não consecutivos.
 * Verifica se o sistema impede o movimento de cartas que não seguem a ordem n e n-1.
 */
void teste_sequencia_valor_errado(void) {
    Jogo j;
    j.tam_pilha[0] = 2;
    j.pilhas[0][0] = (Carta){5, COPAS};
    j.pilhas[0][1] = (Carta){3, COPAS};
    CU_ASSERT_FALSE(validar_sequencia(&j, 0, 2));
}

/**
 * @brief Valida uma sequência longa com 4 cartas do mesmo naipe.
 * Testa se a validação funciona para blocos maiores que um par.
 */
void teste_sequencia_longa_valida(void) {
    Jogo j;
    j.tam_pilha[0] = 4;
    j.pilhas[0][0] = (Carta){7, OUROS};
    j.pilhas[0][1] = (Carta){6, OUROS};
    j.pilhas[0][2] = (Carta){5, OUROS};
    j.pilhas[0][3] = (Carta){4, OUROS};
    CU_ASSERT_TRUE(validar_sequencia(&j, 0, 4));
}

/**
 * @brief Testa a rejeição de sequência longa com naipe quebrado a meio.
 * Garante que uma inconsistência a meio da cadeia invalida toda a sequência.
 */
void teste_sequencia_longa_naipe_quebrado(void) {
    Jogo j;
    j.tam_pilha[0] = 4;
    j.pilhas[0][0] = (Carta){7, OUROS};
    j.pilhas[0][1] = (Carta){6, OUROS};
    j.pilhas[0][2] = (Carta){5, PAUS};
    j.pilhas[0][3] = (Carta){4, PAUS};
    CU_ASSERT_FALSE(validar_sequencia(&j, 0, 4));
}

/* --- Suite 3: Processamento de Jogadas --- */

/**
 * @brief Valida a proteção contra índices de colunas inexistentes.
 * Garante que o jogo não processa movimentos para colunas fora do intervalo 0-9.
 */
void teste_jogada_indices_invalidos(void) {
    Jogo j;
    char msg[100];
    inicializar(&j);
    processar_jogada(&j, 10, 0, 1, msg);
    CU_ASSERT_STRING_EQUAL(msg, "Indices invalidos!");
}

/**
 * @brief Verifica se o sistema impede mover mais cartas do que as existentes na coluna.
 * A carta base do bloco movido deve ter valor imediatamente inferior à carta no topo do destino.
 */
void teste_jogada_quantidade_excessiva(void) {
    Jogo j;
    char msg[100];
    j.tam_pilha[0] = 1;
    processar_jogada(&j, 0, 1, 5, msg);
    CU_ASSERT_STRING_EQUAL(msg, "Indices invalidos!");
}

/**
 * @brief Testa a regra fundamental de destino.
 */
void teste_jogada_destino_invalido(void) {
    Jogo j;
    char msg[100];
    j.tam_pilha[0] = 1; j.pilhas[0][0] = (Carta){5, COPAS};
    j.tam_pilha[1] = 1; j.pilhas[1][0] = (Carta){10, COPAS};
    processar_jogada(&j, 0, 1, 1, msg);
    CU_ASSERT_STRING_EQUAL(msg, "Destino deve ter valor superior!");
}

/**
 * @brief Valida o movimento para uma coluna vazia.
 * Qualquer carta ou sequência válida pode ser movida para um espaço vazio.
 */
void teste_mover_para_coluna_vazia(void) {
    Jogo j;
    char msg[100];
    j.tam_pilha[0] = 1; j.pilhas[0][0] = (Carta){13, ESPADAS};
    j.tam_pilha[1] = 0;
    processar_jogada(&j, 0, 1, 1, msg);
    CU_ASSERT_STRING_EQUAL(msg, "Sucesso!");
}

/**
 * @brief Valida a execução com sucesso de uma jogada legítima.
 * Confirma se a mensagem de "Sucesso!" é retornada quando todas as regras são cumpridas.
 */
void teste_jogada_sucesso(void) {
    Jogo j;
    char msg[100];
    j.tam_pilha[0] = 1; j.pilhas[0][0] = (Carta){4, COPAS};
    j.tam_pilha[1] = 1; j.pilhas[1][0] = (Carta){5, ESPADAS};
    processar_jogada(&j, 0, 1, 1, msg);
    CU_ASSERT_STRING_EQUAL(msg, "Sucesso!");
}

/**
 * @brief Verifica que o estado das pilhas é atualizado após uma jogada bem-sucedida.
 * Após mover 1 carta da coluna 0 para a 1, os tamanhos devem refletir a transferência.
 */
void teste_jogada_atualiza_tamanhos(void) {
    Jogo j;
    char msg[100];
    j.tam_pilha[0] = 1; j.pilhas[0][0] = (Carta){4, COPAS};
    j.tam_pilha[1] = 1; j.pilhas[1][0] = (Carta){5, ESPADAS};
    processar_jogada(&j, 0, 1, 1, msg);
    CU_ASSERT_EQUAL(j.tam_pilha[0], 0);
    CU_ASSERT_EQUAL(j.tam_pilha[1], 2);
}

/* --- Suite 4: Condições de Vitória --- */

/**
 * @brief Verifica que uma pilha com menos de 13 cartas não é considerada completa.
 */
void teste_pilha_incompleta_por_tamanho(void) {
    Jogo j;
    j.tam_pilha[0] = 5;
    CU_ASSERT_FALSE(pilha_completa(&j, 0));
}

/**
 * @brief Valida o reconhecimento de uma sequência completa de Rei (K) a Ás (A).
 * Testa se a função deteta corretamente as 13 cartas ordenadas do mesmo naipe.
 */
void teste_pilha_completa_sucesso(void) {
    Jogo j;
    j.tam_pilha[0] = 13;
    for (int i = 0; i < 13; i++) j.pilhas[0][i] = (Carta){13 - i, OUROS};
    CU_ASSERT_TRUE(pilha_completa(&j, 0));
}

/**
 * @brief Testa que 13 cartas em ordem errada não são aceites como pilha completa.
 * Uma sequência K-A invertida (A no topo, K na base) deve ser rejeitada.
 */
void teste_pilha_completa_ordem_errada(void) {
    Jogo j;
    j.tam_pilha[0] = 13;
    for (int i = 0; i < 13; i++) j.pilhas[0][i] = (Carta){i + 1, OUROS};
    CU_ASSERT_FALSE(pilha_completa(&j, 0));
}

/**
 * @brief Garante que vitória não é declarada com apenas 3 pilhas completas.
 * O jogo só termina com os 4 naipes organizados.
 */
void teste_vitoria_falsa_tres_pilhas(void) {
    Jogo j;
    for (int p = 0; p < 10; p++) j.tam_pilha[p] = 0;
    for (int p = 0; p < 3; p++) {
        j.tam_pilha[p] = 13;
        for (int i = 0; i < 13; i++) j.pilhas[p][i] = (Carta){13 - i, (Naipe)p};
    }
    CU_ASSERT_FALSE(verificar_vitoria(&j));
}

/**
 * @brief Testa a condição global de vitória.
 * Confirma que o jogo termina quando existem 4 naipes completos organizados em colunas.
 */
void teste_vitoria_quatro_pilhas(void) {
    Jogo j;
    for (int p = 0; p < 10; p++) j.tam_pilha[p] = 0;
    for (int p = 0; p < 4; p++) {
        j.tam_pilha[p] = 13;
        for (int i = 0; i < 13; i++) j.pilhas[p][i] = (Carta){13 - i, (Naipe)p};
    }
    CU_ASSERT_TRUE(verificar_vitoria(&j));
}

/* --- Suite 5: Transferência Física --- */

/**
 * @brief Valida o movimento de múltiplas estruturas de dados entre arrays.
 * Garante que os contadores de tamanho das pilhas de origem e destino são atualizados corretamente.
 */
void teste_transferencia_multipla(void) {
    Jogo j;
    j.tam_pilha[0] = 3;
    j.tam_pilha[1] = 0;
    transferir_cartas(&j, 0, 1, 3);
    CU_ASSERT_EQUAL(j.tam_pilha[0], 0);
    CU_ASSERT_EQUAL(j.tam_pilha[1], 3);
}

/**
 * @brief Verifica se os valores das cartas são corretamente copiados na transferência.
 * Garante que a transferência move os dados e não apenas atualiza os contadores.
 */
void teste_transferencia_valores_corretos(void) {
    Jogo j;
    j.tam_pilha[0] = 2;
    j.pilhas[0][0] = (Carta){8, PAUS};
    j.pilhas[0][1] = (Carta){7, PAUS};
    j.tam_pilha[1] = 0;
    transferir_cartas(&j, 0, 1, 2);
    CU_ASSERT_EQUAL(j.pilhas[1][0].valor, 8);
    CU_ASSERT_EQUAL(j.pilhas[1][0].naipe, PAUS);
    CU_ASSERT_EQUAL(j.pilhas[1][1].valor, 7);
}

/* --- Helpers de registo --- */

static CU_pSuite adicionar_suite(const char *nome) {
    CU_pSuite s = CU_add_suite(nome, NULL, NULL);
    if (!s) { CU_cleanup_registry(); exit(CU_get_error()); }
    return s;
}

static void registar_testes(void) {
    CU_pSuite s1 = adicionar_suite("Setup");
    CU_add_test(s1, "Deck Ordenado",    teste_deck_completo);
    CU_add_test(s1, "Distribuicao",     teste_distribuicao_limites);
    CU_add_test(s1, "Init",             teste_estado_inicial);

    CU_pSuite s2 = adicionar_suite("Validacao");
    CU_add_test(s2, "Seq Valida",           teste_sequencia_valida_simples);
    CU_add_test(s2, "Naipe Diferente",      teste_sequencia_naipe_diferente);
    CU_add_test(s2, "Valor Errado",         teste_sequencia_valor_errado);
    CU_add_test(s2, "Seq Longa Valida",     teste_sequencia_longa_valida);
    CU_add_test(s2, "Seq Longa Quebrada",   teste_sequencia_longa_naipe_quebrado);

    CU_pSuite s3 = adicionar_suite("Movimentos");
    CU_add_test(s3, "Indices Fora",     teste_jogada_indices_invalidos);
    CU_add_test(s3, "Qtd Errada",       teste_jogada_quantidade_excessiva);
    CU_add_test(s3, "Regra Destino",    teste_jogada_destino_invalido);
    CU_add_test(s3, "Coluna Vazia",     teste_mover_para_coluna_vazia);
    CU_add_test(s3, "Sucesso Mov",      teste_jogada_sucesso);
    CU_add_test(s3, "Atualiza Sizes",   teste_jogada_atualiza_tamanhos);

    CU_pSuite s4 = adicionar_suite("Finalizacao");
    CU_add_test(s4, "Pilha Curta",      teste_pilha_incompleta_por_tamanho);
    CU_add_test(s4, "Pilha K-A",        teste_pilha_completa_sucesso);
    CU_add_test(s4, "Pilha Ordem Inv",  teste_pilha_completa_ordem_errada);
    CU_add_test(s4, "Vitoria Falsa",    teste_vitoria_falsa_tres_pilhas);
    CU_add_test(s4, "Vitoria",          teste_vitoria_quatro_pilhas);

    CU_pSuite s5 = adicionar_suite("Memoria");
    CU_add_test(s5, "Transf Multi",     teste_transferencia_multipla);
    CU_add_test(s5, "Transf Valores",   teste_transferencia_valores_corretos);
}

/* --- Função Principal de Testes --- */

/**
 * @brief Ponto de entrada para execução dos testes unitários.
 * Inicializa o registo do CUnit, organiza os testes em suites lógicas e apresenta os resultados.
 */
int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();
    registar_testes();
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
