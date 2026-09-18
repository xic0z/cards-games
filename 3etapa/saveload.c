/**
 * @file saveload.c
 * @brief Gravação e carregamento de estado de jogo em ficheiro de texto.
 *
 * save_gravar  → grava no formato interno (VERSAO 1 ...).
 * save_carregar → tenta primeiro o formato interno; se falhar, tenta
 *                 o formato externo (1ª linha = ficheiro .paciencia,
 *                 depois uma linha por pilha com as cartas).
 */

#include "paciencia.h"
#include <sys/stat.h>

/* ================================================================== */
/* Utilitários de carta                                                 */
/* ================================================================== */

/** Nomes dos valores: índice 1-13. */
static const char *VALORES[] = {
    "", "A","2","3","4","5","6","7","8","9","10","J","Q","K"
};
/** Letras dos naipes: 0=C(opas) 1=O(uros) 2=P(aus) 3=E(spadas). */
static const char NAIPES[] = "COPE";

/**
 * @brief Interpreta o valor de uma carta a partir de um token.
 * @param tok Token a interpretar.
 * @param val Ponteiro para o valor resultante.
 * @return Número de caracteres consumidos, ou 0 se inválido.
 */
/** @brief Interpreta valor de figura (A, J, Q, K) ou 10. */
/** @brief Interpreta figura simples (A, J, Q, K). */
static int parse_figura(const char *tok, int *val) {
    if (tok[0] == 'A') { *val = 1;  return 1; }
    if (tok[0] == 'J') { *val = 11; return 1; }
    if (tok[0] == 'Q') { *val = 12; return 1; }
    if (tok[0] == 'K') { *val = 13; return 1; }
    return 0;
}

/** @brief Interpreta valor de figura ou 10. */
static int parse_valor_figura(const char *tok, int *val) {
    int i = parse_figura(tok, val);
    if (i > 0) return i;
    if (tok[0] == '1' && tok[1] == '0') { *val = 10; return 2; }
    return 0;
}

/** @brief Interpreta valor numérico (2-9). */
static int parse_valor_numero(const char *tok, int *val) {
    if (tok[0] >= '2' && tok[0] <= '9') { *val = tok[0] - '0'; return 1; }
    return 0;
}

static int parse_valor(const char *tok, int *val) {
    int i = parse_valor_figura(tok, val);
    if (i > 0) return i;
    return parse_valor_numero(tok, val);
}

/**
 * @brief Interpreta o naipe de uma carta a partir de um caractere.
 * @param n Caractere do naipe.
 * @return Índice do naipe (0-3) ou -1 se inválido.
 */
static int parse_naipe(char n) {
    if (n == 'C' || n == 'H') return 0;
    if (n == 'O' || n == 'D') return 1;
    if (n == 'P')              return 2;
    if (n == 'E' || n == 'S') return 3;
    return -1;
}

/**
 * @brief Converte uma string "AC", "10H", "KS" … numa Carta.
 * @return 1 em sucesso, 0 se o token for inválido.
 */
static int token_para_carta(const char *tok, Carta *c) {
    int val = 0;
    int i = parse_valor(tok, &val);
    if (i == 0) return 0;
    int naipe = parse_naipe(tok[i]);
    if (naipe < 0) return 0;
    c->valor = val;
    c->naipe = naipe;
    return 1;
}

/* ================================================================== */
/* Auxiliares para RegraMove                                            */
/* ================================================================== */

static void escrever_regra(FILE *f, int i, const RegraMove *r) {
    fprintf(f,
        "MOV %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        i,
        r->origem_idx, r->dest_idx,
        r->e_auto, r->f_sempre,
        r->f_sequencia, r->f_dec_consec, r->f_cre_consec,
        r->f_valor_inf, r->f_valor_sup, r->f_valor_adj,
        r->f_msm_naipe_seq, r->f_msm_naipe_topo,
        r->f_alt_naipe_seq, r->f_alt_naipe_topo,
        r->f_msm_cor_seq,   r->f_msm_cor_topo,
        r->f_alt_cor_seq,   r->f_alt_cor_topo,
        r->f_dest_vazio,
        r->f_topo_as, r->f_base_as,
        r->f_topo_rei);
}

static int ler_regra(FILE *f, int esperado, RegraMove *r) {
    int idx;
    int lido = fscanf(f,
        " MOV %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        &idx,
        &r->origem_idx, &r->dest_idx,
        &r->e_auto, &r->f_sempre,
        &r->f_sequencia, &r->f_dec_consec, &r->f_cre_consec,
        &r->f_valor_inf, &r->f_valor_sup, &r->f_valor_adj,
        &r->f_msm_naipe_seq, &r->f_msm_naipe_topo,
        &r->f_alt_naipe_seq, &r->f_alt_naipe_topo,
        &r->f_msm_cor_seq,   &r->f_msm_cor_topo,
        &r->f_alt_cor_seq,   &r->f_alt_cor_topo,
        &r->f_dest_vazio,
        &r->f_topo_as, &r->f_base_as,
        &r->f_topo_rei);
    return lido == 23 && idx == esperado;
}

/* ================================================================== */
/* Formato interno (VERSAO 1) — gravação                               */
/* ================================================================== */

/** @brief Grava os tipos de pilha no ficheiro. */
static void gravar_tipos(FILE *f, Jogo *j) {
    fprintf(f, "N_TIPOS %d\n", j->n_tipos);
    for (int i = 0; i < j->n_tipos; i++) {
        TipoPilha *t = &j->tipos[i];
        fprintf(f, "TIPO %d %d %d %d %d %s\n",
                i, t->visivel_todas, t->visivel_nenhuma,
                t->visivel_topo, t->max_uma, t->nome);
    }
}

/** @brief Grava as regras MOV e AUTO no ficheiro. */
static void gravar_regras(FILE *f, Jogo *j) {
    fprintf(f, "N_MOVS %d\n", j->n_movs);
    for (int i = 0; i < j->n_movs; i++)
        escrever_regra(f, i, &j->movs[i]);
    fprintf(f, "N_AUTOS %d\n", j->n_autos);
    for (int i = 0; i < j->n_autos; i++)
        escrever_regra(f, i, &j->autos[i]);
}

/** @brief Grava as condições de vitória no ficheiro. */
static void gravar_wins(FILE *f, Jogo *j) {
    fprintf(f, "N_WINS %d\n", j->n_wins);
    for (int i = 0; i < j->n_wins; i++)
        fprintf(f, "WIN %d %d %d\n",
                i, j->wins[i].tipo_idx, j->wins[i].n_cartas);
}

/** @brief Grava as pilhas e cartas no ficheiro. */
static void gravar_pilhas(FILE *f, Jogo *j) {
    fprintf(f, "N_PILHAS %d\n", j->n_pilhas);
    for (int i = 0; i < j->n_pilhas; i++) {
        Pilha *p = &j->pilhas[i];
        fprintf(f, "PILHA %d %d %d", i, p->tipo_idx, p->tam);
        for (int k = 0; k < p->tam; k++)
            fprintf(f, " %d %d", p->cartas[k].valor, p->cartas[k].naipe);
        fprintf(f, "\n");
    }
}

/**
 * @brief Grava o estado corrente do jogo num ficheiro de texto interno.
 * @return 1 em sucesso, 0 em erro.
 */
/** @brief Abre o ficheiro de save para escrita. */
static FILE *abrir_save(const char *path) {
    mkdir("saves", 0755);
    return fopen(path, "w");
}

int save_gravar(Jogo *j, const char *path) {
    FILE *f = abrir_save(path);
    if (!f) return 0;
    fprintf(f, "VERSAO 1\n");
    fprintf(f, "NOME %s\n",       j->nome);
    fprintf(f, "FICHEIRO %s\n",   j->ficheiro);
    fprintf(f, "N_BARALHOS %d\n", j->n_baralhos);
    gravar_tipos(f, j);
    gravar_regras(f, j);
    gravar_wins(f, j);
    gravar_pilhas(f, j);
    int erro = ferror(f);
    fclose(f);
    return !erro;
}

/* ================================================================== */
/* Formato interno (VERSAO 1) — carregamento                           */
/* ================================================================== */

/** @brief Lê o cabeçalho do ficheiro interno. */
static int ler_cabecalho(FILE *f, Jogo *j) {
    int versao;
    if (fscanf(f, " VERSAO %d", &versao) != 1 || versao != 1) return 0;
    if (fscanf(f, " NOME %63s",     j->nome)       != 1) return 0;
    if (fscanf(f, " FICHEIRO %63s", j->ficheiro)    != 1) return 0;
    if (fscanf(f, " N_BARALHOS %d", &j->n_baralhos) != 1) return 0;
    return 1;
}

/** @brief Lê os tipos de pilha do ficheiro interno. */
static int ler_tipos(FILE *f, Jogo *j) {
    if (fscanf(f, " N_TIPOS %d", &j->n_tipos) != 1
        || j->n_tipos < 0 || j->n_tipos > MAX_TIPOS) return 0;
    for (int i = 0; i < j->n_tipos; i++) {
        TipoPilha *t = &j->tipos[i];
        int idx;
        if (fscanf(f, " TIPO %d %d %d %d %d %63s",
                   &idx, &t->visivel_todas, &t->visivel_nenhuma,
                   &t->visivel_topo, &t->max_uma, t->nome) != 6
            || idx != i) return 0;
    }
    return 1;
}

/** @brief Lê as regras MOV do ficheiro interno. */
static int ler_movs(FILE *f, Jogo *j) {
    if (fscanf(f, " N_MOVS %d", &j->n_movs) != 1
        || j->n_movs < 0 || j->n_movs > MAX_MOVS) return 0;
    for (int i = 0; i < j->n_movs; i++)
        if (!ler_regra(f, i, &j->movs[i])) return 0;
    return 1;
}

/** @brief Lê as regras AUTO do ficheiro interno. */
static int ler_autos(FILE *f, Jogo *j) {
    if (fscanf(f, " N_AUTOS %d", &j->n_autos) != 1
        || j->n_autos < 0 || j->n_autos > MAX_MOVS) return 0;
    for (int i = 0; i < j->n_autos; i++)
        if (!ler_regra(f, i, &j->autos[i])) return 0;
    return 1;
}

/** @brief Lê as regras MOV e AUTO do ficheiro interno. */
static int ler_regras(FILE *f, Jogo *j) {
    return ler_movs(f, j) && ler_autos(f, j);
}

/** @brief Lê as condições de vitória do ficheiro interno. */
static int ler_wins(FILE *f, Jogo *j) {
    if (fscanf(f, " N_WINS %d", &j->n_wins) != 1
        || j->n_wins < 0 || j->n_wins > MAX_WINS) return 0;
    for (int i = 0; i < j->n_wins; i++) {
        int idx;
        if (fscanf(f, " WIN %d %d %d",
                   &idx, &j->wins[i].tipo_idx, &j->wins[i].n_cartas) != 3
            || idx != i) return 0;
    }
    return 1;
}

/** @brief Lê as pilhas e cartas do ficheiro interno. */
/** @brief Lê as cartas de uma pilha do ficheiro interno. */
static int ler_cartas_pilha(FILE *f, Pilha *p) {
    for (int k = 0; k < p->tam; k++)
        if (fscanf(f, " %d %d",
                   &p->cartas[k].valor, &p->cartas[k].naipe) != 2) return 0;
    return 1;
}

/** @brief Lê as pilhas e cartas do ficheiro interno. */
static int ler_pilhas(FILE *f, Jogo *j) {
    if (fscanf(f, " N_PILHAS %d", &j->n_pilhas) != 1
        || j->n_pilhas < 0 || j->n_pilhas > MAX_PILHAS) return 0;
    for (int i = 0; i < j->n_pilhas; i++) {
        Pilha *p = &j->pilhas[i];
        int idx;
        if (fscanf(f, " PILHA %d %d %d", &idx, &p->tipo_idx, &p->tam) != 3
            || idx != i || p->tam < 0 || p->tam > MAX_CARTAS) return 0;
        if (!ler_cartas_pilha(f, p)) return 0;
    }
    return 1;
}

/**
 * @brief Tenta carregar o formato interno (VERSAO 1).
 * @return 1 em sucesso, 0 se o ficheiro não for formato interno.
 */
static int carregar_interno(Jogo *j, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    int ok = ler_cabecalho(f, j)
          && ler_tipos(f, j)
          && ler_regras(f, j)
          && ler_wins(f, j)
          && ler_pilhas(f, j);
    if (ok) j->hist_tam = 0;
    fclose(f);
    return ok;
}

/* ================================================================== */
/* Formato externo                                                      */
/* ================================================================== */

/**
 * @brief Lê as cartas de uma linha de texto para uma Pilha.
 * @return 1 em sucesso, 0 se algum token for inválido.
 */
static int linha_para_pilha(char *linha, Pilha *p) {
    p->tam = 0;
    char *tok = strtok(linha, " \t\r\n");
    while (tok) {
        if (p->tam >= MAX_CARTAS) return 0;
        if (!token_para_carta(tok, &p->cartas[p->tam])) return 0;
        p->tam++;
        tok = strtok(NULL, " \t\r\n");
    }
    return 1;
}

/** @brief Lê o nome do ficheiro .paciencia da primeira linha do save externo. */
static int ler_nome_paciencia(FILE *f, char *pac_ficheiro) {
    if (!fgets(pac_ficheiro, MAX_NOME, f)) return 0;
    int n = strlen(pac_ficheiro);
    while (n > 0 && (pac_ficheiro[n-1] == '\n' || pac_ficheiro[n-1] == '\r'
                     || pac_ficheiro[n-1] == ' ')) pac_ficheiro[--n] = '\0';
    return 1;
}

/** @brief Tenta fazer parser do jogo a partir do ficheiro .paciencia. */
static int ler_jogo_paciencia(Jogo *tmp, const char *pac_ficheiro) {
    char tentativa[MAX_NOME * 2];
    snprintf(tentativa, sizeof(tentativa), "paciencias/%s", pac_ficheiro);
    if (parser_ler(tmp, tentativa)) return 1;
    return parser_ler(tmp, pac_ficheiro);
}

/** @brief Lê as linhas de cartas para cada pilha do save externo. */
static int ler_pilhas_externo(FILE *f, Jogo *tmp) {
    char linha[MAX_CARTAS * 6];
    for (int i = 0; i < tmp->n_pilhas; i++) {
        if (tmp->pilhas[i].tam == 0) continue;
        if (!fgets(linha, sizeof(linha), f)) {
            tmp->pilhas[i].tam = 0;
        } else {
            int l = strlen(linha);
            while (l > 0 && (linha[l-1] == '\n' || linha[l-1] == '\r')) linha[--l] = '\0';
            if (!linha_para_pilha(linha, &tmp->pilhas[i])) return 0;
        }
    }
    return 1;
}

/**
 * @brief Tenta carregar o formato externo.
 * @return 1 em sucesso, 0 em erro.
 */
/** @brief Carrega o jogo do ficheiro .paciencia e lê as pilhas externas. */
static int carregar_tmp(Jogo *tmp, FILE *f, const char *pac_ficheiro) {
    if (!ler_jogo_paciencia(tmp, pac_ficheiro)) return 0;
    return ler_pilhas_externo(f, tmp);
}

/** @brief Finaliza o carregamento externo, copiando tmp para j se ok. */
static int finalizar_externo(Jogo *j, Jogo *tmp, int ok) {
    if (ok) { tmp->hist_tam = 0; *j = *tmp; }
    free(tmp);
    return ok;
}

static int carregar_externo(Jogo *j, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char pac_ficheiro[MAX_NOME];
    if (!ler_nome_paciencia(f, pac_ficheiro)) { fclose(f); return 0; }
    Jogo *tmp = calloc(1, sizeof(Jogo));
    if (!tmp) { fclose(f); return 0; }
    int ok = carregar_tmp(tmp, f, pac_ficheiro);
    fclose(f);
    return finalizar_externo(j, tmp, ok);
}

/* ================================================================== */
/* API pública                                                          */
/* ================================================================== */

/**
 * @brief Carrega um estado de jogo — tenta formato interno, depois externo.
 * @return 1 em sucesso, 0 em erro.
 */
int save_carregar(Jogo *j, const char *path) {
    if (carregar_interno(j, path)) return 1;
    return carregar_externo(j, path);
}
