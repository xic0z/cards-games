#include "simon.h"

// Este ficheiro gere a ponte entre a entrada bruta do utilizador e a lógica de processamento
// do jogo, utilizando métodos de leitura mais robustos para evitar o bloqueio do terminal
// e garantir uma experiência de utilizador fluida.

// Função responsável pela captura e validação da linha de comando inserida pelo
// utilizador, tratando diferentes formatos de entrada e o encerramento do jogo.
// Parâmetros: 1). Jogo g: Ponteiro para a estrutura que armazena o estado global
// e controlo do jogo; 2). char msg: Ponteiro para a string de feedback onde são
// registadas as mensagens de sucesso ou erro.
// Funcionamento:
// 1. Leitura por Linha: Utiliza a função fgets para ler toda a linha de entrada de uma vez,
// o que evita problemas de sincronização no buffer do teclado (stdin) comuns no scanf.
// 2. Extração de Dados (sscanf): Tenta extrair três valores inteiros da string lida.
// Caso consiga os três, procede para a validação da jogada ou saída.
// 3. Saída do Jogo: Verifica especificamente se o primeiro valor inserido é 0. Se for,
// a flag a_correr é alterada para 0, terminando o loop principal do programa de forma limpa.
// 4. Tratamento de Erros: Caso a entrada não corresponda a três números (e não seja o
// comando de saída 0), define uma mensagem informativa no ponteiro msg ("Erro: Deves
// inserir 3 numeros!").
// 5. Conversão de Índices: Realiza a conversão da lógica do utilizador (Pilha 1 a 10)
// para a lógica de programação (índice 0 a 9) ao subtrair 1 aos valores de origem e destino.
/** @brief Gere a leitura do comando e executa a jogada. */
void gerir_input(Jogo *g, char *msg) {
    char linha[64];
    int o, d, q;

    printf("Origem Destino Qtd (0 p/ sair): ");
    if (!fgets(linha, sizeof(linha), stdin)) {
        g->a_correr = 0;
        return;
    }

    if (sscanf(linha, "%d %d %d", &o, &d, &q) == 3) {
        if (o == 0) {
            g->a_correr = 0;
        } else {
            processar_jogada(g, o - 1, d - 1, q, msg);
        }
    } else if (sscanf(linha, "%d", &o) == 1 && o == 0) {
        g->a_correr = 0;
    } else {
        sprintf(msg, "Erro: Deves inserir 3 numeros!");
    }
}

// O ponto de entrada do programa, responsável por configurar o ambiente e gerir o ciclo
// de vida da aplicação. 
// Configuração de sistema: 
// 1. Suporte Unicode: Configura o locale para en_US.UTF-8 para permitir a exibição correta
// dos símbolos dos naipes das cartas. 
// 2. Aleatoriedade: Inicializa a semente do gerador de números aleatórios (srand) com base no 
// tempo atual do sistema, garantindo baralhações distintas em cada execução. 
// Inicialização: Cria a cada instância do jogo g e define a mensagem de boas-vindas. Invoca a função
// inicializar(&g) para preparar o tabuleiro. 
// Ciclo Principal (while): O jogo permance em execução enquanto a variável g.a_correr for verdadeira.
// Em cada iteração, o ecrã é atualizado através da função desenhar. Verificação da vitória: após cada 
// jogada, a função verificar_vitoria é consultada. Se o resultado for positivo, exibe uma mensagem de parabéns
// e encerra o ciclo. Iteração: Caso o jogo ainda não tenha terminado, a função gerir_input é chamada para aguardar
// a próxima ação do jogador. 
/** @brief Ponto de entrada. Loop principal reduzido para 11 instrucoes. */
int main() {
    Jogo g;
    char msg[100] = "Bem-vindo!";
    setlocale(LC_ALL, "en_US.UTF-8");
    srand(time(NULL));
    inicializar(&g);
    while (g.a_correr) {
        desenhar(&g, msg);
        if (verificar_vitoria(&g)) {
            printf("PARABENS! GANHASTE O JOGO!\n");
            g.a_correr = 0;
        } else {
            gerir_input(&g, msg);
        }
    }
    return 0;
}
