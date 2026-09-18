/**
 * @file main.c
 * @brief Interface de Utilizador e ciclo de controlo do Golf.
 * * Este ficheiro gere a entrada de dados (teclado) e a renderização 
 * visual no terminal, ligando a lógica do jogo ao utilizador.
 */

#include "golf.h"

/**
 * @brief Exibe a mensagem visual de vitória.
 */
void desenhar_vitoria() { 
    system("clear || cls"); 
    printf(MAG BOLD "--- PARABÉNS! ---\n" RESET); 
}

/**
 * @brief Exibe a mensagem visual de fim de jogo .
 */
void desenhar_gameover() { 
    printf(RED BOLD "\n--- GAME OVER! ---\n" RESET); 
}

/**
 * @brief Renderiza uma única carta com cor.
 * @param c A estrutura da carta a ser desenhada.
 */
void desenhar_carta(Carta c) {
    char *n[] = {"\u2665", "\u2666", "\u2660", "\u2663"}; // Copas, Ouros, Espadas, Paus
    char *v[] = {"","A","2","3","4","5","6","7","8","9","10","J","Q","K"};
    
    printf(BOLD "[%2s" RESET, v[c.valor]);
    // Define a cor do naipe 
    printf("%s%s" RESET BOLD "]" RESET, (c.naipe < 2) ? RED : "", n[c.naipe]);
}

/**
 * @brief  desenha uma linha horizontal de cartas das colunas.
 * @param j Ponteiro para o estado do jogo.
 * @param l Índice da linha a renderizar.
 */
static void desenhar_linha_coluna(Jogo *j, int l) {
    for (int p = 0; p < 7; p++) {
        (l < j->tam_pilha[p]) ? (desenhar_carta(j->pilhas[p][l]), printf("  ")) : printf("        ");
    }
    printf("\n");
}

/**
 * @brief cria o cabeçalho e todas as colunas de cartas no tabuleiro.
 * @param j Ponteiro para o estado do jogo.
 */
static void desenhar_colunas(Jogo *j) {
    for (int p = 0; p < 7; p++) printf(CYN "  P%d    " RESET, p + 1);
    printf("\n");
    // máximo de 5 cartas iniciais por coluna
    for (int l = 0; l < 5; l++) desenhar_linha_coluna(j, l);
}

/**
 * @brief Limpa o ecrã e desenha o estado atual do jogo.
 * @param j Ponteiro para o estado do jogo.
 * @param msg Mensagem de feedback. 
 */
void desenhar(Jogo *j, char *msg) {
    system("clear || cls");
    printf(GRN BOLD "=== GOLF SOLITAIRE ===\n" RESET);
    printf("Pontos: %d | Baralho: %d\n\n", j->pontos, 16 - j->idx_baralho);
    desenhar_colunas(j);
    printf("\n" YEL "DESCARTE: " RESET);
    desenhar_carta(j->descarte);
    printf("\n\n%s\n[1-7] Jogar | [8] Biscar | [0] Sair: ", msg);
}

/**
 * @brief Processa o numéro inserido pelo utilizador.
 * @param g Ponteiro para o estado do jogo.
 * @param cmd Inteiro que representa a ação escolhida.
 * @param msg Ponteiro para a string de feedback onde serão gravados erros.
 */
void processar_jogada(Jogo *g, int cmd, char *msg) {
    if (cmd >= 1 && cmd <= 7) {
        int p = cmd - 1;
        if (g->tam_pilha[p] > 0 && validar(g->pilhas[p][g->tam_pilha[p]-1].valor, g->descarte.valor)) {
            g->descarte = g->pilhas[p][--g->tam_pilha[p]]; 
            g->pontos += 10;
        } else {
            sprintf(msg, RED "Jogada Inválida!" RESET);
        }
    } else if (cmd == 8 && g->idx_baralho < 16) {
        g->descarte = g->baralho[g->idx_baralho++];
    }
}

/**
 * @brief Lê a entrada do utilizador.
 * @return int O valor numérico lido ou -1 em erro.
 */
int ler_comando() {
    int c; 
    char l[256];
    return (fgets(l, 256, stdin) && sscanf(l, "%d", &c) == 1) ? c : -1;
}

/**
 * @brief Implementa o ciclo principal de jogo .
 * * Gere a alternância entre desenho, leitura e processamento até
 * que o jogo termine por vitória, derrota ou desistência.
 * @param g Ponteiro para o estado do jogo.
 */
void executar_jogo(Jogo *g) {
    char msg[100] = "";
    int ativo = 1;
    while (ativo) {
        desenhar(g, msg); 
        msg[0] = '\0'; 
        int cmd = ler_comando();
        
        (cmd == 0) ? (ativo = 0) : processar_jogada(g, cmd, msg);
        
        if (verificou_vitoria(g)) { 
            desenhar_vitoria(); 
            ativo = 0; 
        }
        if (ativo && !tem_jogadas_possiveis(g)) { 
            desenhar(g, ""); 
            desenhar_gameover(); 
            ativo = 0; 
        }
    }
}

/**
 * @brief  entrada do programa.
 */
int main() {
    setlocale(LC_ALL, "en_US.UTF-8"); 
    srand(time(NULL));               //  baralho sempre aleatório
    
    Jogo g;
    inicializar(&g);
    executar_jogo(&g);
    
    return 0;
}
