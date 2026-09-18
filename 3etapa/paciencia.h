/**
 * @file paciencia.h
 * @brief Estruturas e protótipos para o motor genérico de paciências.
 */

#ifndef PACIENCIA_H
#define PACIENCIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

/* --- Constantes ANSI para a Interface --- */
#ifndef RED
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"
#endif

/* --- Limites --- */
#define MAX_TIPOS      16
#define MAX_PILHAS     64
#define MAX_CARTAS    104
#define MAX_MOVS       32
#define MAX_WINS       16
#define MAX_NOME       64
#define MAX_HIST      256

/** @struct Carta */
typedef struct {
    int valor;  /**< 1=Ás, 2-10, 11=J, 12=Q, 13=K */
    int naipe;  /**< 0=Copas, 1=Ouros, 2=Paus, 3=Espadas */
} Carta;

/** @struct TipoPilha */
typedef struct {
    char nome[MAX_NOME];
    int visivel_todas;
    int visivel_nenhuma;
    int visivel_topo;
    int max_uma;
} TipoPilha;

/** @struct Pilha */
typedef struct {
    Carta cartas[MAX_CARTAS];
    int   tam;
    int   tipo_idx;
} Pilha;

/** @struct RegraMove */
typedef struct {
    int  origem_idx;
    int  dest_idx;
    int  e_auto;
    int  f_sempre;
    int  f_sequencia;
    int  f_dec_consec;
    int  f_cre_consec;
    int  f_valor_inf;
    int  f_valor_sup;
    int  f_valor_adj;
    int  f_msm_naipe_seq;
    int  f_msm_naipe_topo;
    int  f_alt_naipe_seq;
    int  f_alt_naipe_topo;
    int  f_msm_cor_seq;
    int  f_msm_cor_topo;
    int  f_alt_cor_seq;
    int  f_alt_cor_topo;
    int  f_dest_vazio;
    int  f_topo_as;
    int  f_base_as;
    int  f_topo_rei;
    int  f_base_rei;
} RegraMove;

/** @struct RegraWin */
typedef struct {
    int tipo_idx;
    int n_cartas;
} RegraWin;

/** @struct EstadoPilhas */
typedef struct {
    Pilha pilhas[MAX_PILHAS];
    int   n_pilhas;
} EstadoPilhas;

/** @struct Jogo */
typedef struct {
    char      nome[MAX_NOME];
    char      ficheiro[MAX_NOME];
    int       n_baralhos;
    TipoPilha tipos[MAX_TIPOS];
    int       n_tipos;
    Pilha     pilhas[MAX_PILHAS];
    int       n_pilhas;
    RegraMove movs[MAX_MOVS];
    int       n_movs;
    RegraMove autos[MAX_MOVS];
    int       n_autos;
    RegraWin  wins[MAX_WINS];
    int       n_wins;
    EstadoPilhas hist[MAX_HIST];
    int          hist_tam;
} Jogo;

/* --- Protótipos: parser.c --- */
int parser_ler(Jogo *j, const char *path);
int parser_listar(const char *pasta, char lista[][MAX_NOME], int max);

/* --- Protótipos: logica_gen.c --- */
void logica_distribuir(Jogo *j);
int  logica_valida_mov(Jogo *j, int orig, int dest, int qtd);
void logica_executar_mov(Jogo *j, int orig, int dest, int qtd);
void logica_executar_autos(Jogo *j);
int  logica_verificar_vitoria(Jogo *j);
void logica_guardar_estado(Jogo *j);
int  logica_undo(Jogo *j);

/* --- Protótipos: saveload.c --- */
int save_gravar(Jogo *j, const char *path);
int save_carregar(Jogo *j, const char *path);

/* --- Protótipos: interface_gen.c --- */
void ui_desenhar(Jogo *j, const char *msg);
void ui_ler_linha(char *buf, int tam, const char *prompt);
int  ui_menu_selecao(char lista[][MAX_NOME], int n);
void ui_executar_jogo(Jogo *j);

#endif /* PACIENCIA_H */
