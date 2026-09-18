#ifndef SIMON_H
#define SIMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"

// Naipe (enum): Definir os quatro naipes do baralho: Copas, Ouros, Paus e Espadas.
typedef enum { COPAS, OUROS, PAUS, ESPADAS } Naipe;

// Carta (struct): Estrutura base que representa uma carta individual, contendo um 
// valor (inteiro) e um naipe.
typedef struct {
    int valor; 
    Naipe naipe;
} Carta;

// Jogo (struct): A estrutura central que armazena o estado completo da partida: pilhas[10][52]:
// matriz que representa as 10 colunas do tabuleiro, cada uma com capacidade para um baralho completo. 
// tam_pilha[10]: array que armazena a quantidade atual de cartas em cada uma das 10 colunas. 
// a_correr: Flag booleana (inteiro) que indica se o ciclo principal do jogo deve continuar ativo.
typedef struct {
    Carta pilhas[10][52]; 
    int tam_pilha[10];
    int a_correr; 
} Jogo;

// Lógica de jogo: 
// 1). inicializar(Jogo *j): Prepara o estado inicial, incluindo a criação, baralhação
// e distribuição das cartas. 
// 2). processar_jogada(Jogo *j, int o, int d, int q, char *msg): Valida e executa o movimento
// de q cartas da coluna origem o para o destino d. 
// 3). validar_sequencia(Jogo *j, int col, int qtd): Verifica se o bloco de cartas selecionado numa 
// coluna cumpre as regras de seqência (mesmo naipe e valores decrescentes). 
// 4). pilha_completa(Jogo *j, int col): Verifica se uma coluna específica contém uma sequêncoa completa
// de Rei e Às do mesmo naipe. 
// 5). transferir_cartas(Jogo *j, int o, int d, int q): Realiza a movimentacao fisica das cartas entre pilhas.
/* Lógica */
void inicializar(Jogo *j);
int processar_jogada(Jogo *j, int o, int d, int q, char *msg);
int validar_sequencia(Jogo *j, int col, int qtd);
int verificar_vitoria(Jogo *j);
int pilha_completa(Jogo *j, int col);
void transferir_cartas(Jogo *j, int o, int d, int q);

// Interface de utilizador: 
// 1). desenhar_carta(Carta c): Renderiza visualmente uma única carta com o seu símbolo 
// Unicode e cor respetiva. 
// 2). desenhar_linha(Jogo *j, int linha): Função que imprime uma linha horizontal do tabuleiro de jogo. 
// 3). desenhar(Jogo *j, char *msg): Limpa o terminal e redesenha todo o tabuleiro, incluindo o 
// cabeçalho e a mensagem de estado. 
/* Interface */
void desenhar_carta(Carta c);
void desenhar_linha(Jogo *j, int linha);
void desenhar(Jogo *j, char *msg);

#endif
