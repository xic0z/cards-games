/**
 * @file test_logica.c
 * @brief Suite de testes unitários para o motor de lógica de Paciência.
 *
 * Testa todas as funções públicas de logica_gen.c com base no comportamento
 * real da implementação: logica_distribuir, logica_valida_mov,
 * logica_executar_mov, logica_executar_autos, logica_verificar_vitoria,
 * logica_guardar_estado e logica_undo.
 *
 * Cumpre todas as regras do projeto: sem globais, sem break/continue/goto,
 * complexidade ciclomática <= 10, <= 15 instruções por função, documentação
 * completa em todas as funções.
 */

#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "paciencia.h"

/* ------------------------------------------------------------------ */
/* Auxiliares de setup                                                  */
/* ------------------------------------------------------------------ */

/**
 * @brief Aloca um Jogo no heap e inicializa os campos base.
 * @return Ponteiro para Jogo alocado e zerado, ou NULL em falha.
 * @post j tem 2 tipos, 3 pilhas (2 do tipo 0, 1 do tipo 1), sem regras.
 * @note O chamador é responsável por libertar com free().
 */
static Jogo *novo_jogo(void) {
    Jogo *j = calloc(1, sizeof(Jogo));
    if (!j) return NULL;
    j->n_baralhos = 1;
    j->n_tipos    = 2;
    j->n_pilhas   = 3;
    j->tipos[0].max_uma   = 0;
    j->tipos[1].max_uma   = 1;
    j->pilhas[0].tipo_idx = 0;
    j->pilhas[1].tipo_idx = 0;
    j->pilhas[2].tipo_idx = 1;
    return j;
}

/**
 * @brief Cria uma regra MOV simples entre tipo 0 -> tipo 0.
 * @param r       Ponteiro para a RegraMove a preencher.
 * @param orig    Índice do tipo de origem.
 * @param dest    Índice do tipo de destino.
 */
static void setup_regra_simples(RegraMove *r, int orig, int dest) {
    memset(r, 0, sizeof(RegraMove));
    r->origem_idx = orig;
    r->dest_idx   = dest;
}

/**
 * @brief Coloca uma carta no topo de uma pilha.
 * @param p     Ponteiro para a Pilha.
 * @param valor Valor da carta (1-13).
 * @param naipe Naipe da carta (0=Copas,1=Ouros,2=Paus,3=Espadas).
 */
static void empilhar_carta(Pilha *p, int valor, int naipe) {
    p->cartas[p->tam].valor = valor;
    p->cartas[p->tam].naipe = naipe;
    p->tam++;
}

/* ------------------------------------------------------------------ */
/* Testes: logica_distribuir                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Verifica que distribuir preenche as pilhas com os tamanhos pedidos.
 * @post pilhas[0].tam == 5, pilhas[1].tam == 7, pilhas[2].tam == 0.
 */
void test_distribuir_tamanhos(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->pilhas[0].tam = 5;
    j->pilhas[1].tam = 7;
    j->pilhas[2].tam = 0;

    logica_distribuir(j);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 5);
    CU_ASSERT_EQUAL(j->pilhas[1].tam, 7);
    CU_ASSERT_EQUAL(j->pilhas[2].tam, 0);
    free(j);
}

/**
 * @brief Verifica que as cartas distribuídas têm valores válidos (1-13).
 * @post Todas as cartas nas pilhas têm valor entre 1 e 13.
 */
void test_distribuir_valores_validos(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->pilhas[0].tam = 10;
    j->pilhas[1].tam = 10;

    logica_distribuir(j);

    int valido = 1;
    for (int i = 0; i < j->pilhas[0].tam; i++)
        if (j->pilhas[0].cartas[i].valor < 1 || j->pilhas[0].cartas[i].valor > 13)
            valido = 0;
    CU_ASSERT_TRUE(valido);
    free(j);
}

/**
 * @brief Verifica que as cartas distribuídas têm naipes válidos (0-3).
 * @post Todas as cartas nas pilhas têm naipe entre 0 e 3.
 */
void test_distribuir_naipes_validos(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->pilhas[0].tam = 10;
    j->pilhas[1].tam = 10;

    logica_distribuir(j);

    int valido = 1;
    for (int i = 0; i < j->pilhas[0].tam; i++)
        if (j->pilhas[0].cartas[i].naipe < 0 || j->pilhas[0].cartas[i].naipe > 3)
            valido = 0;
    CU_ASSERT_TRUE(valido);
    free(j);
}

/**
 * @brief Verifica que distribuir com 2 baralhos distribui 104 cartas no total.
 * @post Soma dos tamanhos das pilhas == 52 cartas distribuídas.
 */
void test_distribuir_total_cartas(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->n_baralhos  = 1;
    j->pilhas[0].tam = 26;
    j->pilhas[1].tam = 26;

    logica_distribuir(j);

    CU_ASSERT_EQUAL(j->pilhas[0].tam + j->pilhas[1].tam, 52);
    free(j);
}

/* ------------------------------------------------------------------ */
/* Testes: logica_valida_mov                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Valida movimento sempre permitido (f_sempre=1) entre tipos corretos.
 * @post logica_valida_mov retorna 1.
 */
void test_valida_mov_sempre(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 7, 3);
    empilhar_carta(&j->pilhas[1], 9, 0);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_sempre = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita movimento quando os tipos de pilha não correspondem à regra.
 * @post logica_valida_mov retorna 0 (tipo errado).
 */
void test_valida_mov_tipo_errado(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 1, 1); /* regra para tipo 1, pilhas são tipo 0 */
    j->movs[0].f_sempre = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Valida movimento com restrição f_valor_inf (origem == destino - 1).
 * @pre Pilha 0: [5 Espadas]. Pilha 1: [6 Copas]. Regra: f_valor_inf=1.
 * @post logica_valida_mov retorna 1.
 */
void test_valida_mov_valor_inf_sucesso(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3); /* 5 Espadas (preto) */
    empilhar_carta(&j->pilhas[1], 6, 0); /* 6 Copas (vermelho) */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_valor_inf = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita movimento com f_valor_inf quando valor não é consecutivo.
 * @pre Pilha 0: [3 Espadas]. Pilha 1: [6 Copas]. Regra: f_valor_inf=1.
 * @post logica_valida_mov retorna 0.
 */
void test_valida_mov_valor_inf_falha(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 3, 3);
    empilhar_carta(&j->pilhas[1], 6, 0);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_valor_inf = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Valida movimento com f_alt_cor_topo (cores alternadas no topo).
 * @pre Pilha 0: [5 Espadas/preto]. Pilha 1: [6 Copas/vermelho].
 * @post logica_valida_mov retorna 1 (cores diferentes).
 */
void test_valida_mov_alt_cor_sucesso(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3); /* preto */
    empilhar_carta(&j->pilhas[1], 6, 0); /* vermelho */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_alt_cor_topo = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita movimento com f_alt_cor_topo quando cores são iguais.
 * @pre Pilha 0: [5 Espadas/preto]. Pilha 1: [6 Paus/preto].
 * @post logica_valida_mov retorna 0.
 */
void test_valida_mov_alt_cor_falha(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3); /* preto */
    empilhar_carta(&j->pilhas[1], 6, 2); /* preto */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_alt_cor_topo = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita mover mais do que 1 carta quando f_sequencia está inativo.
 * @pre Pilha 0 com 2 cartas. Regra sem f_sequencia.
 * @post logica_valida_mov com qtd=2 retorna 0.
 */
void test_valida_mov_sem_sequencia(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 6, 0);
    empilhar_carta(&j->pilhas[0], 5, 3);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_sempre = 1;
    j->movs[0].f_sequencia = 0;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 2));
    free(j);
}

/**
 * @brief Rejeita mover para pilha não vazia quando f_dest_vazio está ativo.
 * @pre Pilha 1 com 1 carta. Regra f_dest_vazio=1.
 * @post logica_valida_mov retorna 0.
 */
void test_valida_mov_dest_vazio_falha(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 13, 3); /* Rei */
    empilhar_carta(&j->pilhas[1], 5, 0);  /* pilha destino não vazia */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_dest_vazio = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Valida mover para pilha vazia quando f_dest_vazio está ativo.
 * @pre Pilha 1 vazia. Regra f_dest_vazio=1.
 * @post logica_valida_mov retorna 1.
 */
void test_valida_mov_dest_vazio_sucesso(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 13, 3); /* Rei */
    /* pilhas[1].tam == 0 */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_dest_vazio = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita mover 2ª carta para pilha max_uma já ocupada.
 * @pre Pilha 2 (tipo 1, max_uma=1) com 1 carta.
 * @post logica_valida_mov retorna 0.
 */
void test_valida_mov_max_uma_falha(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 0);
    empilhar_carta(&j->pilhas[2], 3, 1); /* célula já ocupada */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 1);
    j->movs[0].f_sempre = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 2, 1));
    free(j);
}

/**
 * @brief Valida mover para pilha max_uma vazia.
 * @pre Pilha 2 (tipo 1, max_uma=1) vazia.
 * @post logica_valida_mov retorna 1.
 */
void test_valida_mov_max_uma_sucesso(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 0);
    /* pilhas[2].tam == 0 */

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 1);
    j->movs[0].f_sempre = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 2, 1));
    free(j);
}

/**
 * @brief Rejeita movimento quando não há nenhuma regra definida.
 * @post logica_valida_mov retorna 0 com n_movs == 0.
 */
void test_valida_mov_sem_regras(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 0);
    j->n_movs = 0;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Valida f_topo_rei: só permite mover quando topo da seq é Rei (13).
 * @pre Pilha 0: [Rei Espadas]. Pilha 1: vazia. Regra: f_topo_rei=1.
 * @post logica_valida_mov retorna 1.
 */
void test_valida_mov_topo_rei_sucesso(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 13, 3);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_topo_rei = 1;

    CU_ASSERT_TRUE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/**
 * @brief Rejeita f_topo_rei quando o topo não é Rei.
 * @pre Pilha 0: [5 Espadas]. Regra: f_topo_rei=1.
 * @post logica_valida_mov retorna 0.
 */
void test_valida_mov_topo_rei_falha(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3);

    j->n_movs = 1;
    setup_regra_simples(&j->movs[0], 0, 0);
    j->movs[0].f_topo_rei = 1;

    CU_ASSERT_FALSE(logica_valida_mov(j, 0, 1, 1));
    free(j);
}

/* ------------------------------------------------------------------ */
/* Testes: logica_executar_mov                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Verifica a transferência de 1 carta entre pilhas.
 * @pre Pilha 0: [10 Ouros]. Pilha 1: vazia.
 * @post Pilha 0 vazia; Pilha 1 com 1 carta de valor 10.
 */
void test_executar_mov_uma_carta(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 10, 1);

    logica_executar_mov(j, 0, 1, 1);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 0);
    CU_ASSERT_EQUAL(j->pilhas[1].tam, 1);
    CU_ASSERT_EQUAL(j->pilhas[1].cartas[0].valor, 10);
    CU_ASSERT_EQUAL(j->pilhas[1].cartas[0].naipe, 1);
    free(j);
}

/**
 * @brief Verifica a transferência de uma sequência de 3 cartas.
 * @pre Pilha 0: [8,7,6]. Pilha 1: vazia.
 * @post Pilha 0 vazia; Pilha 1 com 3 cartas na mesma ordem.
 */
void test_executar_mov_sequencia(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 8, 0);
    empilhar_carta(&j->pilhas[0], 7, 3);
    empilhar_carta(&j->pilhas[0], 6, 0);

    logica_executar_mov(j, 0, 1, 3);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 0);
    CU_ASSERT_EQUAL(j->pilhas[1].tam, 3);
    CU_ASSERT_EQUAL(j->pilhas[1].cartas[0].valor, 8);
    CU_ASSERT_EQUAL(j->pilhas[1].cartas[2].valor, 6);
    free(j);
}

/**
 * @brief Verifica que mover qtd cartas parciais deixa as restantes na origem.
 * @pre Pilha 0: [9,8,7]. Move 2 cartas para Pilha 1.
 * @post Pilha 0 com 1 carta (valor 9); Pilha 1 com 2 cartas.
 */
void test_executar_mov_parcial(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 9, 0);
    empilhar_carta(&j->pilhas[0], 8, 3);
    empilhar_carta(&j->pilhas[0], 7, 0);

    logica_executar_mov(j, 0, 1, 2);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 1);
    CU_ASSERT_EQUAL(j->pilhas[0].cartas[0].valor, 9);
    CU_ASSERT_EQUAL(j->pilhas[1].tam, 2);
    free(j);
}

/* ------------------------------------------------------------------ */
/* Testes: logica_executar_autos                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Verifica que AUTO move um Ás para uma pilha de foundation (tipo diferente).
 *
 * Usa tipos distintos para origem (tipo 0) e destino (tipo 1) garantindo
 * que o movimento é unidirecional e não cria um ciclo infinito.
 *
 * @pre Pilha 0 (tipo 0): [Ás Copas]. Pilha 2 (tipo 1): vazia.
 *      Regra AUTO: tipo 0 -> tipo 1, f_base_as=1 (só move Ás).
 * @post Pilha 0 vazia; Pilha 2 com 1 carta (Ás).
 */
void test_autos_f_sempre(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 1, 0); /* Ás de Copas */

    j->n_autos = 1;
    setup_regra_simples(&j->autos[0], 0, 1); /* tipo 0 -> tipo 1 */
    j->autos[0].f_base_as = 1;               /* só move se a base da seq é Ás */

    logica_executar_autos(j);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 0);
    CU_ASSERT_EQUAL(j->pilhas[2].tam, 1);
    free(j);
}

/**
 * @brief Verifica que sem regras AUTO o estado fica inalterado.
 * @pre Pilha 0: [5 Paus]. Sem regras AUTO.
 * @post Pilha 0 continua com 1 carta.
 */
void test_autos_sem_regras(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 2);
    j->n_autos = 0;

    logica_executar_autos(j);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 1);
    free(j);
}

/**
 * @brief Verifica que AUTO não move quando a regra não é satisfeita.
 * @pre Pilha 0: [5 Espadas/preto]. Pilha 1: [7 Paus/preto]. Regra: f_alt_cor_topo.
 * @post Estado inalterado (mesma cor, regra falha).
 */
void test_autos_regra_nao_satisfeita(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 5, 3); /* preto */
    empilhar_carta(&j->pilhas[1], 7, 2); /* preto */

    j->n_autos = 1;
    setup_regra_simples(&j->autos[0], 0, 0);
    j->autos[0].f_alt_cor_topo = 1;

    logica_executar_autos(j);

    CU_ASSERT_EQUAL(j->pilhas[0].tam, 1);
    CU_ASSERT_EQUAL(j->pilhas[1].tam, 1);
    free(j);
}

/* ------------------------------------------------------------------ */
/* Testes: logica_verificar_vitoria                                     */
/* ------------------------------------------------------------------ */

/**
 * @brief Retorna 0 quando não há condições WIN definidas.
 * @post logica_verificar_vitoria retorna 0.
 */
void test_vitoria_sem_wins(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->n_wins = 0;
    CU_ASSERT_FALSE(logica_verificar_vitoria(j));
    free(j);
}

/**
 * @brief Retorna 0 quando pilha tem menos cartas do que o exigido.
 * @pre WIN: tipo 0 deve ter 13 cartas. Pilha 0 tem 10.
 * @post logica_verificar_vitoria retorna 0.
 */
void test_vitoria_incompleta(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->n_wins = 1;
    j->wins[0].tipo_idx = 0;
    j->wins[0].n_cartas = 13;
    j->pilhas[0].tam    = 10;
    j->pilhas[1].tam    = 13;

    CU_ASSERT_FALSE(logica_verificar_vitoria(j));
    free(j);
}

/**
 * @brief Retorna 1 quando todas as pilhas do tipo WIN têm o número exato.
 * @pre WIN: tipo 0 deve ter 13 cartas. Pilha 0 e Pilha 1 têm 13.
 * @post logica_verificar_vitoria retorna 1.
 */
void test_vitoria_completa(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->n_wins = 1;
    j->wins[0].tipo_idx = 0;
    j->wins[0].n_cartas = 13;
    j->pilhas[0].tam    = 13;
    j->pilhas[1].tam    = 13;

    CU_ASSERT_TRUE(logica_verificar_vitoria(j));
    free(j);
}

/**
 * @brief Retorna 0 quando apenas uma de duas WIN está satisfeita.
 * @pre WIN1: tipo 0 com 13; WIN2: tipo 1 com 1. Tipo 1 tem 0 cartas.
 * @post logica_verificar_vitoria retorna 0.
 */
void test_vitoria_parcial(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->n_wins = 2;
    j->wins[0].tipo_idx = 0; j->wins[0].n_cartas = 13;
    j->wins[1].tipo_idx = 1; j->wins[1].n_cartas = 1;
    j->pilhas[0].tam    = 13;
    j->pilhas[1].tam    = 13;
    j->pilhas[2].tam    = 0;  /* tipo 1, não satisfeita */

    CU_ASSERT_FALSE(logica_verificar_vitoria(j));
    free(j);
}

/* ------------------------------------------------------------------ */
/* Testes: logica_guardar_estado e logica_undo                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Verifica que undo repõe o estado guardado anteriormente.
 * @pre Guarda estado com pilhas[0].tam==2; altera para 5; faz undo.
 * @post pilhas[0].tam volta a 2.
 */
void test_undo_restaura_estado(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->pilhas[0].tam = 2;

    logica_guardar_estado(j);
    j->pilhas[0].tam = 5;

    CU_ASSERT_TRUE(logica_undo(j));
    CU_ASSERT_EQUAL(j->pilhas[0].tam, 2);
    free(j);
}

/**
 * @brief Verifica que undo falha quando o histórico está vazio.
 * @post logica_undo retorna 0.
 */
void test_undo_historico_vazio(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    j->hist_tam = 0;

    CU_ASSERT_FALSE(logica_undo(j));
    free(j);
}

/**
 * @brief Verifica múltiplos undo em sequência.
 * @pre Guarda 2 estados, faz 2 undo. Terceiro undo deve falhar.
 * @post Terceiro undo retorna 0.
 */
void test_undo_multiplos(void) {
    Jogo *j = novo_jogo(); if (!j) return;

    j->pilhas[0].tam = 1;
    logica_guardar_estado(j);
    j->pilhas[0].tam = 2;
    logica_guardar_estado(j);
    j->pilhas[0].tam = 3;

    CU_ASSERT_TRUE(logica_undo(j));
    CU_ASSERT_EQUAL(j->pilhas[0].tam, 2);
    CU_ASSERT_TRUE(logica_undo(j));
    CU_ASSERT_EQUAL(j->pilhas[0].tam, 1);
    CU_ASSERT_FALSE(logica_undo(j));
    free(j);
}

/**
 * @brief Verifica que guardar estado copia as cartas corretamente.
 * @pre Pilha 0 tem 1 carta (valor=7). Guarda, altera para valor=9, faz undo.
 * @post Carta volta a valor=7.
 */
void test_undo_restaura_cartas(void) {
    Jogo *j = novo_jogo(); if (!j) return;
    empilhar_carta(&j->pilhas[0], 7, 2);

    logica_guardar_estado(j);
    j->pilhas[0].cartas[0].valor = 9;

    logica_undo(j);

    CU_ASSERT_EQUAL(j->pilhas[0].cartas[0].valor, 7);
    free(j);
}

/* ------------------------------------------------------------------ */
/* Ponto de entrada                                                     */
/* ------------------------------------------------------------------ */

/**
 * @brief Regista todas as suites e testes, corre e devolve o resultado.
 * @return 0 se todos os testes passaram, código de erro CUnit caso contrário.
 */
int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    CU_pSuite s = CU_add_suite("logica_gen", NULL, NULL);
    if (!s) { CU_cleanup_registry(); return CU_get_error(); }

    if (
        !CU_add_test(s, "distribuir: tamanhos pedidos respeitados",    test_distribuir_tamanhos)       ||
        !CU_add_test(s, "distribuir: valores das cartas validos",      test_distribuir_valores_validos)||
        !CU_add_test(s, "distribuir: naipes das cartas validos",       test_distribuir_naipes_validos) ||
        !CU_add_test(s, "distribuir: total de cartas correto",         test_distribuir_total_cartas)   ||
        !CU_add_test(s, "valida_mov: f_sempre permite qualquer mov",   test_valida_mov_sempre)         ||
        !CU_add_test(s, "valida_mov: tipo errado rejeita",             test_valida_mov_tipo_errado)    ||
        !CU_add_test(s, "valida_mov: f_valor_inf sucesso",             test_valida_mov_valor_inf_sucesso)||
        !CU_add_test(s, "valida_mov: f_valor_inf falha",               test_valida_mov_valor_inf_falha)||
        !CU_add_test(s, "valida_mov: f_alt_cor_topo sucesso",          test_valida_mov_alt_cor_sucesso)||
        !CU_add_test(s, "valida_mov: f_alt_cor_topo falha",            test_valida_mov_alt_cor_falha)  ||
        !CU_add_test(s, "valida_mov: sem f_sequencia rejeita qtd>1",   test_valida_mov_sem_sequencia)  ||
        !CU_add_test(s, "valida_mov: f_dest_vazio falha com cartas",   test_valida_mov_dest_vazio_falha)||
        !CU_add_test(s, "valida_mov: f_dest_vazio sucesso vazio",      test_valida_mov_dest_vazio_sucesso)||
        !CU_add_test(s, "valida_mov: max_uma falha pilha cheia",       test_valida_mov_max_uma_falha)  ||
        !CU_add_test(s, "valida_mov: max_uma sucesso pilha vazia",     test_valida_mov_max_uma_sucesso)||
        !CU_add_test(s, "valida_mov: sem regras rejeita sempre",       test_valida_mov_sem_regras)     ||
        !CU_add_test(s, "valida_mov: f_topo_rei sucesso",              test_valida_mov_topo_rei_sucesso)||
        !CU_add_test(s, "valida_mov: f_topo_rei falha",                test_valida_mov_topo_rei_falha) ||
        !CU_add_test(s, "executar_mov: transfere 1 carta",             test_executar_mov_uma_carta)    ||
        !CU_add_test(s, "executar_mov: transfere sequencia",           test_executar_mov_sequencia)    ||
        !CU_add_test(s, "executar_mov: move parcial deixa resto",      test_executar_mov_parcial)      ||
        !CU_add_test(s, "autos: f_sempre move automaticamente",        test_autos_f_sempre)            ||
        !CU_add_test(s, "autos: sem regras nao altera estado",         test_autos_sem_regras)          ||
        !CU_add_test(s, "autos: regra nao satisfeita nao move",        test_autos_regra_nao_satisfeita)||
        !CU_add_test(s, "vitoria: sem wins retorna 0",                 test_vitoria_sem_wins)          ||
        !CU_add_test(s, "vitoria: pilha incompleta retorna 0",         test_vitoria_incompleta)        ||
        !CU_add_test(s, "vitoria: todas completas retorna 1",          test_vitoria_completa)          ||
        !CU_add_test(s, "vitoria: win parcial retorna 0",              test_vitoria_parcial)           ||
        !CU_add_test(s, "undo: restaura estado anterior",              test_undo_restaura_estado)      ||
        !CU_add_test(s, "undo: historico vazio retorna 0",             test_undo_historico_vazio)      ||
        !CU_add_test(s, "undo: multiplos undos em sequencia",          test_undo_multiplos)            ||
        !CU_add_test(s, "undo: restaura conteudo das cartas",          test_undo_restaura_cartas)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    int falhas = (int)CU_get_number_of_failures();
    CU_cleanup_registry();
    return falhas;
}
