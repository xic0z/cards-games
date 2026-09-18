/**
 * @file parser.c
 * @brief Leitura e interpretação de ficheiros DSL de paciências.
 *
 * Implementa o parser que converte ficheiros de texto no formato
 * da DSL numa estrutura Jogo pronta a usar. Suporta todos os
 * comandos: JOGO, BARALHOS, TIPO, INIT, MOV, AUTO e WIN.
 */

#include "paciencia.h"
#include <dirent.h>
#include <ctype.h>
#include <stddef.h>

/* ------------------------------------------------------------------ */
/* Funções auxiliares internas                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Remove comentários e whitespace de fim de linha.
 * @param linha Buffer da linha a limpar (modificado in-place).
 */
static void limpar_linha(char *linha) {
    char *p = strchr(linha, '#');
    if (p) *p = '\0';
    int n = strlen(linha);
    while (n > 0 && (linha[n-1] == '\n' || linha[n-1] == '\r'
                      || linha[n-1] == ' ' || linha[n-1] == '\t'))
        linha[--n] = '\0';
}

/**
 * @brief Procura um tipo de pilha pelo nome.
 * @param j    Ponteiro para o jogo.
 * @param nome Nome do tipo a procurar.
 * @return Índice do tipo ou -1 se não encontrado.
 */
static int achar_tipo(Jogo *j, const char *nome) {
    int res = -1;
    for (int i = 0; i < j->n_tipos; i++) {
        if (strcmp(j->tipos[i].nome, nome) == 0) res = i;
    }
    return res;
}

/**
 * @brief Interpreta as flags de um comando TIPO.
 * @param t     Ponteiro para o TipoPilha a preencher.
 * @param flags String de flags.
 */
static void parse_flags_tipo(TipoPilha *t, const char *flags) {
    for (int i = 0; flags[i]; i++) {
        if (flags[i] == '=') t->visivel_todas   = 1;
        if (flags[i] == '_') t->visivel_nenhuma = 1;
        if (flags[i] == '^') t->visivel_topo    = 1;
        if (flags[i] == '1') t->max_uma         = 1;
    }
}

/**
 * @brief Devolve o offset do campo de RegraMove correspondente a um char de flag.
 * @param c Carácter da flag a pesquisar.
 * @return Offset do campo, ou (size_t)-1 se não encontrado.
 */
static size_t offset_para_flag(char c) {
    static const struct { char c; size_t off; } mapa[] = {
        {'*', offsetof(RegraMove, f_sempre)},
        {'+', offsetof(RegraMove, f_sequencia)},
        {'[', offsetof(RegraMove, f_dec_consec)},
        {']', offsetof(RegraMove, f_cre_consec)},
        {'<', offsetof(RegraMove, f_valor_inf)},
        {'>', offsetof(RegraMove, f_valor_sup)},
        {'~', offsetof(RegraMove, f_valor_adj)},
        {'m', offsetof(RegraMove, f_msm_naipe_seq)},
        {'M', offsetof(RegraMove, f_msm_naipe_topo)},
        {'x', offsetof(RegraMove, f_alt_naipe_seq)},
        {'X', offsetof(RegraMove, f_alt_naipe_topo)},
        {'c', offsetof(RegraMove, f_msm_cor_seq)},
        {'C', offsetof(RegraMove, f_msm_cor_topo)},
        {'d', offsetof(RegraMove, f_alt_cor_seq)},
        {'D', offsetof(RegraMove, f_alt_cor_topo)},
        {'V', offsetof(RegraMove, f_dest_vazio)},
        {'a', offsetof(RegraMove, f_topo_as)},
        {'A', offsetof(RegraMove, f_base_as)},
        {'k', offsetof(RegraMove, f_topo_rei)},
        {'K', offsetof(RegraMove, f_base_rei)},
        {0, 0}
    };
    for (int k = 0; mapa[k].c; k++)
        if (mapa[k].c == c) return mapa[k].off;
    return (size_t)-1;
}

/**
 * @brief Aplica uma flag individual a uma RegraMove.
 * @param r Ponteiro para a RegraMove a preencher.
 * @param c Carácter da flag a aplicar.
 */
static void aplicar_flag_mov(RegraMove *r, char c) {
    size_t off = offset_para_flag(c);
    if (off != (size_t)-1)
        *((int *)((char *)r + off)) = 1;
}

/**
 * @brief Interpreta as flags de um comando MOV ou AUTO.
 * @param r     Ponteiro para a RegraMove a preencher.
 * @param flags String de flags.
 */
static void parse_flags_mov(RegraMove *r, const char *flags) {
    for (int i = 0; flags[i]; i++)
        aplicar_flag_mov(r, flags[i]);
}


/* ------------------------------------------------------------------ */
/* Processamento de cada comando                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Processa o comando JOGO.
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_jogo(Jogo *j, const char *resto) {
    sscanf(resto, "%63s", j->nome);
}

/**
 * @brief Processa o comando BARALHOS.
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_baralhos(Jogo *j, const char *resto) {
    sscanf(resto, "%d", &j->n_baralhos);
}

/**
 * @brief Processa o comando TIPO.
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_tipo(Jogo *j, const char *resto) {
    char nome[MAX_NOME], flags[MAX_NOME];
    if (sscanf(resto, "%63s %63s", nome, flags) != 2) return;
    int idx = achar_tipo(j, nome);
    if (idx < 0 && j->n_tipos < MAX_TIPOS) {
        idx = j->n_tipos++;
        memset(&j->tipos[idx], 0, sizeof(TipoPilha));
        strncpy(j->tipos[idx].nome, nome, MAX_NOME - 1);
    }
    if (idx >= 0) parse_flags_tipo(&j->tipos[idx], flags);
}

/**
 * @brief Processa o comando INIT.
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_init(Jogo *j, const char *resto) {
    char nome[MAX_NOME]; int n;
    if (sscanf(resto, "%63s %d", nome, &n) != 2) return;
    int t = achar_tipo(j, nome);
    if (t < 0 || j->n_pilhas >= MAX_PILHAS) return;
    int idx = j->n_pilhas++;
    memset(&j->pilhas[idx], 0, sizeof(Pilha));
    j->pilhas[idx].tipo_idx = t;
    j->pilhas[idx].tam = n; /* n_init guardado como tam; será preenchido */
}

/**
 * @brief Regista uma regra de movimento (MOV ou AUTO) no jogo.
 * @param j      Ponteiro para o jogo.
 * @param oi     Índice do tipo de pilha de origem.
 * @param di     Índice do tipo de pilha de destino.
 * @param e_auto 1 se AUTO, 0 se MOV.
 * @param flags  String de flags do movimento.
 */
static void registar_mov(Jogo *j, int oi, int di, int e_auto,
                          const char *flags) {
    RegraMove *arr = e_auto ? j->autos : j->movs;
    int *n         = e_auto ? &j->n_autos : &j->n_movs;
    if (*n >= MAX_MOVS) return;
    memset(&arr[*n], 0, sizeof(RegraMove));
    arr[*n].origem_idx = oi;
    arr[*n].dest_idx   = di;
    arr[*n].e_auto     = e_auto;
    parse_flags_mov(&arr[*n], flags);
    (*n)++;
}

/**
 * @brief Processa o comando MOV ou AUTO, interpretando tipos e flags.
 * @param j      Ponteiro para o jogo.
 * @param resto  Resto da linha após o comando.
 * @param e_auto 1 se AUTO, 0 se MOV.
 */
static void proc_mov(Jogo *j, const char *resto, int e_auto) {
    char orig[MAX_NOME], dest[MAX_NOME], flags[MAX_NOME];
    if (sscanf(resto, "%63s %63s %63s", orig, dest, flags) != 3) return;
    int oi = achar_tipo(j, orig);
    int di = achar_tipo(j, dest);
    if (oi < 0 || di < 0) return;
    registar_mov(j, oi, di, e_auto, flags);
}

/**
 * @brief Processa o comando WIN.
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_win(Jogo *j, const char *resto) {
    char nome[MAX_NOME]; int n;
    if (sscanf(resto, "%63s %d", nome, &n) != 2) return;
    int t = achar_tipo(j, nome);
    if (t < 0 || j->n_wins >= MAX_WINS) return;
    j->wins[j->n_wins].tipo_idx = t;
    j->wins[j->n_wins].n_cartas = n;
    j->n_wins++;
}

/* ------------------------------------------------------------------ */
/* Interface pública                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Lê e interpreta um ficheiro DSL, preenchendo a estrutura Jogo.
 * @param j    Ponteiro para o jogo.
 * @param path Caminho do ficheiro.
 * @return 1 em sucesso, 0 em erro.
 */

typedef void (*ProcFn)(Jogo *, const char *);

/**
 * @brief Wrapper para processar o comando AUTO (movimento automático).
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_auto(Jogo *j, const char *resto) { proc_mov(j, resto, 1); }

/**
 * @brief Wrapper para processar o comando MOV (movimento manual).
 * @param j     Ponteiro para o jogo.
 * @param resto Resto da linha após o comando.
 */
static void proc_mov0(Jogo *j, const char *resto) { proc_mov(j, resto, 0); }

static void processar_linha(Jogo *j, const char *linha) {
    static const struct {
        const char *cmd;
        ProcFn fn;
    } tabela[] = {
        {"JOGO",     proc_jogo},
        {"BARALHOS", proc_baralhos},
        {"TIPO",     proc_tipo},
        {"INIT",     proc_init},
        {"MOV",      proc_mov0},
        {"AUTO",     proc_auto},
        {"WIN",      proc_win},
        {NULL, NULL}
    };

    char cmd[32];

    if (sscanf(linha, "%31s", cmd) != 1)
        return;

    const char *resto = linha + strlen(cmd);

    while (*resto == ' ' || *resto == '\t')
        resto++;

    for (int i = 0; tabela[i].cmd; i++) {
        if (strcmp(cmd, tabela[i].cmd) == 0) {
            tabela[i].fn(j, resto);
            return;
        }
    }
}


int parser_ler(Jogo *j, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    memset(j, 0, sizeof(Jogo));
    j->n_baralhos = 1;
    const char *base = strrchr(path, '/');
    strncpy(j->ficheiro, base ? base + 1 : path, MAX_NOME - 1);
    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        limpar_linha(linha);
        if (linha[0] != '\0') processar_linha(j, linha);
    }
    fclose(f);
    return 1;
}


/**
 * @brief Lista ficheiros .paciencia numa pasta.
 * @param pasta Caminho da pasta.
 * @param lista Array de strings para os nomes.
 * @param max   Tamanho máximo do array.
 * @return Número de ficheiros encontrados.
 */

static void ordenar_lista(char lista[][MAX_NOME], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int k = 0; k < n - i - 1; k++)
            if (strcmp(lista[k], lista[k+1]) > 0) {
                char tmp[MAX_NOME];
                strncpy(tmp,        lista[k],   MAX_NOME - 1);
                strncpy(lista[k],   lista[k+1], MAX_NOME - 1);
                strncpy(lista[k+1], tmp,        MAX_NOME - 1);
            }
}

int parser_listar(const char *pasta, char lista[][MAX_NOME], int max) {
    DIR *d = opendir(pasta);
    if (!d) return 0;
    int n = 0;
    struct dirent *e;
    while ((e = readdir(d)) && n < max) {
        char *ext = strrchr(e->d_name, '.');
        if (ext && strcmp(ext, ".paciencia") == 0)
            strncpy(lista[n++], e->d_name, MAX_NOME - 1);
    }
    closedir(d);
    ordenar_lista(lista, n);
    return n;
}
