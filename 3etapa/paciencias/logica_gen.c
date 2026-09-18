/**
 * @file logica_gen.c
 * @brief Motor de lógica genérico para qualquer paciência descrita pela DSL.
 *
 * Valida movimentos com base nas flags da RegraMove, executa
 * movimentos automáticos (AUTO) e verifica condições de vitória (WIN).
 * Segue todas as restrições: sem globais, sem break/continue/goto,
 * complexidade ciclomática <= 10, <= 15 instruções por função.
 */

#include "paciencia.h"

/* ------------------------------------------------------------------ */
/* Funções auxiliares de cartas                                         */
/* ------------------------------------------------------------------ */

/** @brief Devolve a cor de uma carta (0=vermelho, 1=preto). */
static int cor_carta(Carta c) {
    return (c.naipe >= 2) ? 1 : 0;
}

/** @brief Gera um baralho ordenado de 52 cartas. */
static void gerar_baralho(Carta *deck, int inicio) {
    int k = inicio;
    for (int s = 0; s < 4; s++) {
        for (int v = 1; v <= 13; v++) {
            deck[k].naipe = s;
            deck[k++].valor = v;
        }
    }
}

/** @brief Baralha um deck pelo algoritmo Fisher-Yates. */
static void baralhar(Carta *deck, int n) {
    for (int i = n - 1; i > 0; i--) {
        int r = rand() % (i + 1);
        Carta tmp = deck[i];
        deck[i] = deck[r];
        deck[r] = tmp;
    }
}

/* ------------------------------------------------------------------ */
/* Distribuição inicial                                                 */
/* ------------------------------------------------------------------ */

/**
 * @brief Calcula o número de baralhos válido (entre 1 e MAX_CARTAS/52).
 * @param n_baralhos Valor pedido.
 * @return Valor corrigido dentro dos limites.
 */
static int calcular_n_baralhos(int n_baralhos) {
    int min = (n_baralhos < 1) ? 1 : n_baralhos;
    return (min > MAX_CARTAS / 52) ? MAX_CARTAS / 52 : min;
}

/**
 * @brief Preenche as pilhas com as cartas do deck já baralhado.
 * @param j     Ponteiro para o jogo.
 * @param deck  Array de cartas baralhadas.
 * @param total Total de cartas disponíveis no deck.
 */
static void preencher_pilhas(Jogo *j, Carta *deck, int total) {
    int k = 0;
    for (int p = 0; p < j->n_pilhas; p++) {
        int n_init = j->pilhas[p].tam;
        j->pilhas[p].tam = 0;
        for (int i = 0; i < n_init && k < total; i++)
            j->pilhas[p].cartas[j->pilhas[p].tam++] = deck[k++];
    }
}

/** @brief Distribui as cartas iniciais pelas pilhas. */
void logica_distribuir(Jogo *j) {
    int nb    = calcular_n_baralhos(j->n_baralhos);
    int total = nb * 52;
    Carta deck[MAX_CARTAS];
    for (int b = 0; b < nb; b++)
        gerar_baralho(deck, b * 52);
    baralhar(deck, total);
    preencher_pilhas(j, deck, total);
}

/* ------------------------------------------------------------------ */
/* Validação de sequência dentro de uma pilha                          */
/* ------------------------------------------------------------------ */

/** @brief Verifica flags de ordem entre dois cartas consecutivas. */
static int valida_seq_ordem(Carta a, Carta b, RegraMove *r) {
    int ok = 1;
    if (r->f_dec_consec && (a.valor != b.valor + 1)) ok = 0;
    if (r->f_cre_consec && (a.valor != b.valor - 1)) ok = 0;
    return ok;
}

/** @brief Verifica flags de naipe/cor entre duas cartas consecutivas. */
static int valida_seq_naipe(Carta a, Carta b, RegraMove *r) {
    int ok = 1;
    if (r->f_msm_naipe_seq && (a.naipe != b.naipe))             ok = 0;
    if (r->f_alt_naipe_seq && (a.naipe == b.naipe))             ok = 0;
    if (r->f_msm_cor_seq && (cor_carta(a) != cor_carta(b)))     ok = 0;
    if (r->f_alt_cor_seq && (cor_carta(a) == cor_carta(b)))     ok = 0;
    return ok;
}

/** @brief Verifica flags de sequência de cartas (m, x, c, d, [, ]). */
static int valida_seq_interna(Carta *pilha, int base, int qtd, RegraMove *r) {
    int ok = 1;
    for (int i = 0; i < qtd - 1 && ok; i++) {
        Carta a = pilha[base + i];
        Carta b = pilha[base + i + 1];
        if (!valida_seq_ordem(a, b, r)) ok = 0;
        if (!valida_seq_naipe(a, b, r)) ok = 0;
    }
    return ok;
}

/** @brief Verifica flags de valor entre topo da sequência e topo do destino. */
static int valida_topo_valor(Carta topo_seq, Carta topo_dest, RegraMove *r) {
    int ok = 1;
    if (r->f_valor_inf && (topo_seq.valor != topo_dest.valor - 1)) ok = 0;
    if (r->f_valor_sup && (topo_seq.valor != topo_dest.valor + 1)) ok = 0;
    if (r->f_valor_adj) {
        int d = topo_seq.valor - topo_dest.valor;
        if (d != 1 && d != -1) ok = 0;
    }
    return ok;
}

/** @brief Verifica flags de naipe/cor entre topo da sequência e topo do destino. */
static int valida_topo_naipe(Carta topo_seq, Carta topo_dest, RegraMove *r) {
    int ok = 1;
    if (r->f_msm_naipe_topo && (topo_seq.naipe != topo_dest.naipe))         ok = 0;
    if (r->f_alt_naipe_topo && (topo_seq.naipe == topo_dest.naipe))         ok = 0;
    if (r->f_msm_cor_topo && (cor_carta(topo_seq) != cor_carta(topo_dest))) ok = 0;
    if (r->f_alt_cor_topo && (cor_carta(topo_seq) == cor_carta(topo_dest))) ok = 0;
    return ok;
}

/** @brief Verifica flags relativas ao topo da sequência vs. topo do destino. */
static int valida_topo_dest(Carta topo_seq, Carta topo_dest, RegraMove *r) {
    int ok = 1;
    if (!valida_topo_valor(topo_seq, topo_dest, r)) ok = 0;
    if (!valida_topo_naipe(topo_seq, topo_dest, r)) ok = 0;
    return ok;
}

/** @brief Verifica flags de topo/base da sequência (a, A, k, K). */
static int valida_flags_as_rei(Carta *pilha, int base, int qtd, RegraMove *r) {
    Carta topo_seq = pilha[base + qtd - 1];
    Carta base_seq = pilha[base];
    int ok = 1;
    if (r->f_topo_as  && topo_seq.valor != 1)  ok = 0;
    if (r->f_base_as  && base_seq.valor != 1)  ok = 0;
    if (r->f_topo_rei && topo_seq.valor != 13) ok = 0;
    if (r->f_base_rei && base_seq.valor != 13) ok = 0;
    return ok;
}

/* ------------------------------------------------------------------ */
/* Validação de uma regra MOV para um movimento concreto               */
/* ------------------------------------------------------------------ */

/** @brief Verifica pré-condições básicas da regra (tipos, tamanhos, flags simples). */
static int regra_pre(Jogo *j, int orig, int dest, int qtd, RegraMove *r) {
    Pilha *po = &j->pilhas[orig];
    Pilha *pd = &j->pilhas[dest];
    if (po->tipo_idx != r->origem_idx)                    return 0;
    if (pd->tipo_idx != r->dest_idx)                      return 0;
    if (qtd > po->tam)                                    return 0;
    if (!r->f_sequencia && qtd != 1)                      return 0;
    if (r->f_dest_vazio && pd->tam != 0)                  return 0;
    if (j->tipos[pd->tipo_idx].max_uma && pd->tam + qtd > 1) return 0;
    return 1;
}

/** @brief Verifica se há restrições de topo definidas na regra. */
static int tem_restricao_topo(RegraMove *r) {
    return r->f_valor_inf || r->f_valor_sup || r->f_valor_adj
        || r->f_msm_naipe_topo || r->f_alt_naipe_topo
        || r->f_msm_cor_topo   || r->f_alt_cor_topo;
}

/** @brief Verifica a relação entre a sequência e o topo do destino. */
static int regra_valida_dest(
    Jogo *j,
    int orig,
    int dest,
    int qtd,
    RegraMove *r)
{
    Pilha *po = &j->pilhas[orig];
    Pilha *pd = &j->pilhas[dest];

    if (r->f_sempre) return 1;

    if (pd->tam > 0) {
        Carta topo_seq  = po->cartas[po->tam - qtd];
        Carta topo_dest = pd->cartas[pd->tam - 1];
        return valida_topo_dest(topo_seq, topo_dest, r);
    }

    return !tem_restricao_topo(r);
}

/** @brief Verifica se uma regra específica permite um movimento. */
static int regra_permite(Jogo *j, int orig, int dest, int qtd, RegraMove *r) {
    if (!regra_pre(j, orig, dest, qtd, r)) return 0;
    int base = j->pilhas[orig].tam - qtd;
    if (!valida_seq_interna(j->pilhas[orig].cartas, base, qtd, r)) return 0;
    if (!valida_flags_as_rei(j->pilhas[orig].cartas, base, qtd, r)) return 0;
    return regra_valida_dest(j, orig, dest, qtd, r);
}

/** @brief Verifica se alguma regra MOV permite o movimento. */
int logica_valida_mov(Jogo *j, int orig, int dest, int qtd) {
    int ok = 0;
    for (int i = 0; i < j->n_movs && !ok; i++)
        ok = regra_permite(j, orig, dest, qtd, &j->movs[i]);
    return ok;
}

/* ------------------------------------------------------------------ */
/* Execução de movimentos                                               */
/* ------------------------------------------------------------------ */

/** @brief Executa a transferência física de cartas entre pilhas. */
void logica_executar_mov(Jogo *j, int orig, int dest, int qtd) {
    Pilha *po = &j->pilhas[orig];
    Pilha *pd = &j->pilhas[dest];
    int base = po->tam - qtd;
    for (int i = 0; i < qtd; i++)
        pd->cartas[pd->tam++] = po->cartas[base + i];
    po->tam -= qtd;
}

/** @brief Tenta aplicar uma regra AUTO a um par de pilhas; devolve 1 se moveu. */
static int autos_tentar(Jogo *j, int o, int d, RegraMove *auto_r) {
    int qtd = j->pilhas[o].tam;
    int moveu = 0;
    for (int q = 1; q <= qtd && !moveu; q++) {
        if (regra_permite(j, o, d, q, auto_r)) {
            logica_executar_mov(j, o, d, q);
            moveu = 1;
        }
    }
    return moveu;
}

/** @brief Tenta aplicar uma regra AUTO a todas as combinações de pilhas. */
static int autos_varrer(Jogo *j, RegraMove *auto_r) {
    int houve = 0;
    for (int o = 0; o < j->n_pilhas; o++) {
        for (int d = 0; d < j->n_pilhas; d++) {
            if (o != d && autos_tentar(j, o, d, auto_r)) houve = 1;
        }
    }
    return houve;
}

#define MAX_ITER_AUTOS 1024

/** @brief Executa os movimentos AUTO em cadeia até não haver mais. */
void logica_executar_autos(Jogo *j) {
    int houve = 1;
    int iter  = 0;
    while (houve && iter < MAX_ITER_AUTOS) {
        houve = 0;
        iter++;
        for (int a = 0; a < j->n_autos; a++) {
            if (autos_varrer(j, &j->autos[a])) houve = 1;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Vitória                                                              */
/* ------------------------------------------------------------------ */

/** @brief Conta sequências completas válidas (13 mesmo naipe, K->A) no topo das pilhas. */
static int contar_seq_completas(Jogo *j, int tipo) {
    int count = 0;
    for (int p = 0; p < j->n_pilhas; p++) {
        Pilha *pl = &j->pilhas[p];
        if (pl->tipo_idx != tipo || pl->tam < 13) continue;
        int base = pl->tam - 13;
        int valida = (pl->cartas[base].valor == 13);
        for (int i = 0; i < 12 && valida; i++) {
            Carta a = pl->cartas[base + i];
            Carta b = pl->cartas[base + i + 1];
            if (a.naipe != b.naipe || a.valor != b.valor + 1) valida = 0;
        }
        if (valida) count++;
    }
    return count;
}

/** @brief Verifica a condição de vitória (conjunção de todas as WIN). */
int logica_verificar_vitoria(Jogo *j) {
    if (j->n_wins == 0) return 0;
    int ok = 1;
    for (int w = 0; w < j->n_wins && ok; w++) {
        int tipo     = j->wins[w].tipo_idx;
        int esperado = j->wins[w].n_cartas;
        if (esperado == 0) {
            int soma = 0;
            for (int p = 0; p < j->n_pilhas; p++)
                if (j->pilhas[p].tipo_idx == tipo) soma += j->pilhas[p].tam;
            if (soma != 0) ok = 0;
        } else {
            if (contar_seq_completas(j, tipo) < esperado / 13) ok = 0;
        }
    }
    return ok;
}

/* ------------------------------------------------------------------ */
/* Histórico / Undo                                                     */
/* ------------------------------------------------------------------ */

/** @brief Guarda o estado atual das pilhas no histórico. */
void logica_guardar_estado(Jogo *j) {
    if (j->hist_tam >= MAX_HIST) return;
    EstadoPilhas *e = &j->hist[j->hist_tam++];
    e->n_pilhas = j->n_pilhas;
    for (int p = 0; p < j->n_pilhas; p++)
        e->pilhas[p] = j->pilhas[p];
}

/** @brief Repõe o estado anterior (undo). */
int logica_undo(Jogo *j) {
    if (j->hist_tam == 0) return 0;
    EstadoPilhas *e = &j->hist[--j->hist_tam];
    j->n_pilhas = e->n_pilhas;
    for (int p = 0; p < j->n_pilhas; p++)
        j->pilhas[p] = e->pilhas[p];
    return 1;
}
