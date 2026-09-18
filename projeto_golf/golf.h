/**
 * @file golf.h
 * @brief Definições de estruturas e protótipos para o jogo Golf Solitaire.
 * * Este ficheiro define a arquitetura de dados e as constantes de formatação
 * visual, garantindo a separação entre lógica e interface.
 */

#ifndef GOLF_H
#define GOLF_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>

/* --- Constantes de Formatação ANSI --- */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"

/**
 * @struct Carta
 * @brief Representa uma carta individual do baralho.
 */
typedef struct {
    int valor; /**< Valor da carta (1 a 13) */
    int naipe; /**< Naipe da carta (0 a 3) */
} Carta;

/**
 * @struct Jogo
 * @brief Contém o estado completo de uma partida.
 */
typedef struct {
    Carta pilhas[7][20]; 
    int tam_pilha[7];    
    Carta baralho[52];   
    int idx_baralho;     
    Carta descarte;      
    int pontos;          
} Jogo;

/* --- Protótipos da Lógica (logica.c) --- */

/**
 * @brief Inicializa o jogo, baralha e distribui as cartas.
 * @param j Ponteiro para a estrutura do jogo.
 */
void inicializar(Jogo *j);

/**
 * @brief Valida se uma jogada é permitida segundo as regras do Golf.
 * @param v1 Valor da carta a jogar.
 * @param v2 Valor da carta no descarte.
 * @return 1 se válido, 0 caso contrário.
 */
int validar(int v1, int v2);

/**
 * @brief Verifica se ainda existem jogadas possíveis no tabuleiro.
 * @param j Ponteiro para o estado do jogo.
 * @return 1 se houver jogadas, 0 se não houver.
 */
int tem_jogadas_possiveis(Jogo *j);

/**
 * @brief Verifica se o jogador venceu o jogo.
 * @param j Ponteiro para o estado do jogo.
 * @return 1 se venceu, 0 caso contrário.
 */
int verificou_vitoria(Jogo *j);

/* --- Protótipos da Interface (main.c) --- */

/**
 * @brief Controla o fluxo principal de execução do jogo.
 * @param g Ponteiro para o estado do jogo.
 */
void executar_jogo(Jogo *g);

/**
 * @brief Renderiza o estado do jogo no terminal.
 * @param j Ponteiro para o estado do jogo.
 * @param msg Mensagem de feedback ao utilizador.
 */
void desenhar(Jogo *j, char *msg);

#endif
