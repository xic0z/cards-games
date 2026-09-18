#include "simon.h"

// Função que gera um baralho padrão de 52 cartas de forma ordenada.
// Utiliza um ciclo para preencher um array de 52 estruturas Carta.
// O valor definido por (i % 13) + 1 (garantindo valores de 1  13) e o naipe
// da divisão inteira i/13 (agrupando 13 cartas por cada um dos 4 naipes).
/** @brief Cria baralho ordenado por naipes e valores. */
void criar_deck(Carta *deck) {
    for (int i = 0; i < 52; i++) {
        deck[i].valor = (i % 13) + 1;
        deck[i].naipe = (Naipe)(i / 13);
    }
}

// Função que implementa o algoritmo de Fisher-Yates para garantir que o baralho seja
// misturado de forma aleatória e uniforme. 
// Funcionamento: Percorre o array de trás para a frente, trocando a carta atual por uma 
// carta numa posição aleatória anterior ou igual a ela. 
/** @brief Baralha o deck usando o algoritmo Fisher-Yates. */
void baralhar(Carta *deck) {
    for (int i = 51; i > 0; i--) {
        int r = rand() % (i + 1);
        Carta temp = deck[i];
        deck[i] = deck[r];
        deck[r] = temp;
    }
}

// Função que distribui as 52 cartas pela 10 colunas (pilhas) do jogo seguindo uma
// configuração específica. 
// Configuração: Utiliza um array de limites lim[] = {8,8,8,7,6,5,4,3,2,1} para definir
// quantas cartas cada coluna recebe. 
// Resultado: Preenche o campotam_pilha de cada coluna e transfere as cartas do deck para as 
// pilhas correspondentes.
/** @brief Distribui as cartas pelas 10 colunas com limites variaveis. */
void distribuir(Jogo *j, Carta *deck) {
    int k = 0;
    int lim[] = {8, 8, 8, 7, 6, 5, 4, 3, 2, 1};
    for (int p = 0; p < 10; p++) {
        j->tam_pilha[p] = lim[p];
        for (int c = 0; c < lim[p]; c++) {
            j->pilhas[p][c] = deck[k++];
        }
    }
}

// Função de alto nível que prepara o estado inicial do jogo. 
// Ações: Define o estado como ativo (a_correr = 1), zera a pontuação, cria o baralho,
// baralha-o e distribui as cartas pelas colunas. 
/** @brief Inicializa o estado estrutural e lógico do jogo. */
void inicializar(Jogo *j) {
    Carta deck[52];
    j->a_correr = 1;
    criar_deck(deck);
    baralhar(deck);
    distribuir(j, deck);
}

// Função que verifica se o grupo de cartas que o utilizador deseja mover respeita as regras
// do Simple Simon. 
// Regra Simple Simon: Para mover um bloco de cartas (qtd), todas devem ser do mesmo naipe e estar
// em ordem decrescente consecutiva (ex: 9 de Copas, 8 de Copas, 7 de Copas).
// Retorno: 1 se a sequência for válida, 0 caso contrário. 
/** @brief Valida se o bloco movido e do mesmo naipe e estritamente decrescente. */
int validar_sequencia(Jogo *j, int col, int qtd) {
    int res = 1;
    int base = j->tam_pilha[col] - qtd;
    // Se base < 0, a jogada ja e invalida (ja tratado no processar_jogada)
    for (int i = 0; i < qtd - 1; i++) {
        Carta atual = j->pilhas[col][base + i];
        Carta proxima = j->pilhas[col][base + i + 1];
        
        // Regra Simon: mesmo naipe E valor sucessivo (ex: 7 e 6)
        if (atual.naipe != proxima.naipe || atual.valor != proxima.valor + 1) {
            res = 0;
        }
    }
    return res;
}

// Função que executa a movimentação física das estruturas de dados entre as colunas.
// Funcionamento: Copia as q cartas da pilha de origem (o) para o topo da pilha de destino
// (d) e atualiza os respetivos contadores de tamanho (tam_pilha).
/** @brief Realiza a movimentacao fisica das cartas entre pilhas. */
void transferir_cartas(Jogo *j, int o, int d, int q) {
    int base_o = j->tam_pilha[o] - q;
    for (int i = 0; i < q; i++) {
        j->pilhas[d][j->tam_pilha[d]++] = j->pilhas[o][base_o + i];
    }
    j->tam_pilha[o] -= q;
}

// Função central de controlo que valida e executa uma jogada completa. 
// Validações efetuadas: 
// 1. Limites: Verifica se os índices das colunas são válidas (0-9) e se a 
// quantidade de cartas existe na origem. 
// 2. Integridade do bloco: Chama validar_sequencia para garantir que o bloco 
// movido é legal. 
// 3. Regra de destino: Se a coluna de destino não estiver vazia, a carta da base 
// do bloco movido deve ter um valor exatamente inferior à carta que já está no topo
// do destino (o naipe pode ser diferente para o encaixe, mas a sequência de valores
// deve ser mantida). 
// Saída: Retorna uma mensagem de sucesso ou o erro específico encontrado.
/** @brief Processa a jogada validando limites, sequencia e regras de destino. */
int processar_jogada(Jogo *j, int o, int d, int q, char *msg) {
  
    if (o < 0 || o > 9 || d < 0 || d > 9 || q <= 0 || q > j->tam_pilha[o])
        return sprintf(msg, "Indices invalidos!");
    
    if (!validar_sequencia(j, o, q))
        return sprintf(msg, "Sequencia invalida!");

    if (j->tam_pilha[d] > 0) {
        Carta topo = j->pilhas[d][j->tam_pilha[d] - 1];
        Carta base = j->pilhas[o][j->tam_pilha[o] - q];
        
        if (topo.valor != base.valor + 1)
            return sprintf(msg, "Destino deve ter valor superior!");
    } else {
    }

    transferir_cartas(j, o, d, q);
    return sprintf(msg, "Sucesso!");
}

// Função que verifica se uma coluna específica contém uma sequência finalizada. 
// Critério: Uma pilha está completa se tiver exatamente 13 cartas, todas do mesmo 
// naipe, ordenadas perfeitamente de Rei (13) a Ás (1). 
/** @brief Verifica se uma pilha especifica contem uma sequencia completa K a A. */
int pilha_completa(Jogo *j, int col) {
    if (j->tam_pilha[col] < 13) return 0;

    // Verifica a sequência nas últimas 13 cartas (topo da pilha)
    int base = j->tam_pilha[col] - 13;
    for (int i = 0; i < 12; i++) {
        if (j->pilhas[col][base + i].valor != 13 - i ||
            j->pilhas[col][base + i].naipe != j->pilhas[col][base + i + 1].naipe)
            return 0;
    }
    return 1;
}

// Função que analisa o tabuleiro para determinar se o jogador venceu. 
// Lógica: Vai iterar por todas as 10 colunas e conta quantas respondem 
// positivamente a pilha_completa.
// Condição: O jogo termina com vitória quando existem 4 sequências completas
// (representando os 4 naipes do baralho).
/** @brief Condicao de vitoria: 4 colunas com sequencias completas do mesmo naipe. */
int verificar_vitoria(Jogo *j) {
    int completas = 0;
    for (int i = 0; i < 10; i++) {
        if (pilha_completa(j, i)) completas++;
    }
    return (completas == 4);
}
