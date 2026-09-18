#include "simon.h"
// O código foca-se na representação visual de um jogo de cartas "Simple Simon" via terminal. 


// Esta função é responsável pela renderização individual de uma carta no terminal, utilizando 
// caracteres Unicode para os naipes e ANSI para distinção visual.
// Parâmeros: 1. Carta c - Uma estrutura que tem os campos valor (inteiro de 1 a 13) e naipe 
// (inteiro de 0 a 3). 
// Funcionamento: 
// 1. Mapeamento de naipes - Utiliza um array de strings com símbolos Unicode. 
// 2. Mapeamento de valores - Converte o valor numérico para a representação clássica (A, 2-10, J, Q, K).
// 3. Lógica de cores - Se o naipe for de Copas (0) ou Ouros (1), a função aplica a cor vermelha (RED. Caso
// contrário, usa a cor padrão. 
// Exemplo de saída: [A♥] (em vermelho) ou [10♠] (cor padrão).
/** @brief Desenha uma carta usando Unicodes e cores condicionais. */
void desenhar_carta(Carta c) {
    char *n[] = {"\u2665", "\u2666", "\u2663", "\u2660"}; // Ordem: Copas, Ouros, Paus, Espadas
    char *v[] = {"","A","2","3","4","5","6","7","8","9","10","J","Q","K"};
    
    printf(BOLD "[%2s%s%s" RESET BOLD "]" RESET, 
           v[c.valor], (c.naipe < 2) ? RED : "", n[c.naipe]);
}

// Função auxiliar de formatação para alinhar o tabuleiro. 
// Funcionamento: Itera de 0 a 9, para imprimir os rótulos das colunas P1 a P10. Utiliza o caractere de
// tabulação (/t) para garantir que os nomes fiquem alinhados com cartas que serão impressas abaixo. 
/** @brief Imprime os nomes das colunas P1 a P10. */
void desenhar_cabecalho() {
    for (int p = 0; p < 10; p++) {
        printf("P%d\t", p + 1);
    }
    printf("\n");
}

// Função que imprime uma linha horizontal do tabuleiro de jogo. 
// Parâmetros: 
// 1. Jogo *j - Ponteiro para a estrutura principal que contêm o estado atual do jogo. 
// 2. int linha - O índice da linha (profundidade da pilha) a ser impressa.
// Funcionamento: 
// 1. A função percorre 10 pilhas do jogo. 
// 2. Verificação de existência: Para cada pilha, verifica se a linha solicitada existe (se o tamanho da 
// pilha é maior do que o índice da linha). 
// 3. Se existir, chama a função desenhar_carta para mostrar a carta naquela posição. 
// 4. Se não existir, imprime apenas um espaço de tabulação para manter o alinhamento das colunas. 
/** @brief Imprime uma linha horizontal de cartas do tabuleiro. */
void desenhar_linha(Jogo *j, int linha) {
    for (int p = 0; p < 10; p++) {
        if (linha < j->tam_pilha[p]) {
            desenhar_carta(j->pilhas[p][linha]);
        }
        printf("\t");
    }
    printf("\n");
}

// Função principal de interface, responsável por limpar o ecrã e renderizar todo o estado do jogo. 
// Parâmetros: 
// 1. Jogo *j - Ponteiro para a estrutura do jogo. 
// 2. char *msg - Uma string de mensagem de status. 
// Fluxo de execução: 
// 1. Limpeza: Executa clear (Linux/macOS) ou cls (Windows) para atualizar a interface sem acumular texto.
// 2. Cabeçalho: Imprime o título estilizado e os nomes das colunas (P1-P10).
// 3. Cálculo de altura: Percorre todas as pilhas para encontrar qual é a mais alta (max), definindo 
// quantas linhas totais precisam de ser impressas.
// 4. Renderização: Executa um ciclo que chama desenhar_linha para cada nível até atingir a altura 
// máxima calculada.
// 5. Rodapé: Exibe a mensagem de status (SITUACAO) enviada por parâmetro.
/** @brief Desenha o tabuleiro completo. */
void desenhar(Jogo *j, char *msg) {
    int max = 0;
    system("clear || cls");
    printf(YEL "================== SIMPLE SIMON ==================\n\n" RESET);
    
    desenhar_cabecalho();

    for (int i = 0; i < 10; i++) {
        if (j->tam_pilha[i] > max) max = j->tam_pilha[i];
    }

    for (int l = 0; l < max; l++) {
        desenhar_linha(j, l);
    }
    
    printf("\n" YEL "SITUACAO: %s" RESET "\n", msg);
}
