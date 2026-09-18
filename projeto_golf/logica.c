/**
 * @file logica.c
 * @brief Motor de processamento central do Solitário Golf.
 * * Implementa as regras, baralhamento e gestão do estado do jogo.
 * Segue as restrições de ausência de break/continue/globais.
 */

#include "golf.h"

/**
 * @brief Gera um baralho ordenado de 52 cartas.
 * @param deck Array de cartas a preencher.
 */
void criar_deck(Carta *deck) {
    int k = 0;
    for (int s = 0; s < 4; s++) {
        for (int v = 1; v <= 13; v++) {
            deck[k].naipe = s;
            deck[k++].valor = v;
        }
    }
}

/**
 * @brief Baralha o deck usando o algoritmo Fisher-Yates.
 * @param deck Array de cartas a baralhar.
 */
void baralhar(Carta *deck) {
    for (int i = 51; i > 0; i--) {
        int r = rand() % (i + 1);
        Carta temp = deck[i];
        deck[i] = deck[r];
        deck[r] = temp;
    }
}

/**
 * @brief Preenche o baralho de reserva do jogo.
 * @param j Estrutura do jogo.
 * @param deck Deck baralhado.
 * @param k Índice atual no deck.
 */
void preencher_reserva(Jogo *j, Carta *deck, int k) {
    int i = 0;
    while (k < 52) {
        j->baralho[i++] = deck[k++];
    }
}

/**
 * @brief Distribui as cartas iniciais pelo tabuleiro.
 * @param j Estrutura do jogo.
 * @param deck Deck baralhado.
 */
void distribuir(Jogo *j, Carta *deck) {
    int k = 0;
    for (int p = 0; p < 7; p++) {
        for (int c = 0; c < 5; c++) {
            j->pilhas[p][c] = deck[k++];
        }
        j->tam_pilha[p] = 5;
    }
    j->descarte = deck[k++];
    preencher_reserva(j, deck, k);
}

/**
 * @brief Inicializa o estado completo do jogo.
 * @param j Estrutura do jogo.
 */
void inicializar(Jogo *j) {
    Carta deck[52];
    criar_deck(deck);
    baralhar(deck);
    distribuir(j, deck);
    j->idx_baralho = 0;
    j->pontos = 0;
}

/**
 * @brief Valida se a diferença entre cartas permite a jogada.
 * @param v1 Valor da carta da pilha.
 * @param v2 Valor da carta no descarte.
 * @return 1 se válido, 0 caso contrário.
 */
int validar(int v1, int v2) {
    int d = v1 - v2;
    int res = (d == 1 || d == -1 || (v1 == 1 && v2 == 13) || (v1 == 13 && v2 == 1));
    return res;
}

/**
 * @brief Verifica se ainda existem movimentos possíveis.
 * @param j Estrutura do jogo.
 * @return 1 se houver jogadas, 0 caso contrário.
 */
int tem_jogadas_possiveis(Jogo *j) {
    int res = 0;
    if (j->idx_baralho < 16) res = 1;
    for (int i = 0; i < 7; i++) {
        if (j->tam_pilha[i] > 0 && validar(j->pilhas[i][j->tam_pilha[i]-1].valor, j->descarte.valor)) {
            res = 1;
        }
    }
    return res;
}

/**
 * @brief Verifica a condição de vitória.
 * @param j Estrutura do jogo.
 * @return 1 se venceu, 0 caso contrário.
 */
int verificou_vitoria(Jogo *j) {
    int vitoria = 1;
    for (int i = 0; i < 7; i++) {
        if (j->tam_pilha[i] > 0) vitoria = 0;
    }
    return vitoria;
}
