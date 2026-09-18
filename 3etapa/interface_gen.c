/**
 * @file interface_gen.c
 * @brief Interface CLI adaptativa para qualquer paciência descrita pela DSL.
 *
 * Trata da renderização do tabuleiro no terminal e da leitura
 * de comandos do utilizador. Não contém lógica de regras.
 * Cumpre: sem variáveis globais, sem break/continue/goto,
 * complexidade ciclomática <= 10, <= 15 instruções por função.
 */
#include <sys/stat.h>
#include "paciencia.h"
#include <ctype.h>

/* ------------------------------------------------------------------ */
/* Renderização de cartas                                               */
/* ------------------------------------------------------------------ */

/**
 * @brief Desenha uma carta com cor ANSI e símbolo Unicode do naipe.
 * @param c Carta a desenhar.
 */
static void desenhar_carta(Carta c) {
    const char *naipes[] = {"\u2665", "\u2666", "\u2663", "\u2660"};
    const char *valores[] = {"","A","2","3","4","5","6","7","8","9","10","J","Q","K"};
    const char *cor = (c.naipe < 2) ? RED : "";
    printf(BOLD "[%2s%s%s" RESET BOLD "]" RESET,
           valores[c.valor], cor, naipes[c.naipe]);
}
// O static garante que esta função é chamada apenas por este ficheiro, em todo o projeto.
// const char *cor = (c.naipe < 2) ? RED : ""; atribui a cor vermelha a copas(0) e a ouros (1), caso c.naipe<2, caso contrário, vai ter a cor predefinida do terminal.
// "[%2s%s%s" o termo %2s garante o alinhamento perfeito das colunas do jogo, garantindo q valores que tenham apenas 1 digitio (como 7 ou A) ocupem exatamente o mesmo espaço que o 10.
// Se a carta tiver c.valor = 12, ele acede a valores[12], que devolve "Q". O %2s garante que ela é impressa com um espaço atras ( Q) para manter o alinhamento.

/**
 * @brief Imprime espaço com a largura de uma carta (pilha vazia).
 * Carta ocupa 5 colunas visuais + 1 espaço separador = 6 colunas.
 */
static void desenhar_vazio(void) {
    printf("      ");
}

/* ------------------------------------------------------------------ */
/* Funções de deteção de pilhas para o Golf                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Encontra o índice do tipo de pilha com mais instâncias (tableau).
 * @param j Ponteiro para o jogo.
 * @return Índice do tipo tableau.
 */
static int golf_tipo_tableau(Jogo *j) {
    int contagens[MAX_TIPOS] = {0};
    for (int p = 0; p < j->n_pilhas; p++)
        contagens[j->pilhas[p].tipo_idx]++;
    int melhor = 0;
    for (int t = 1; t < j->n_tipos; t++)
        if (contagens[t] > contagens[melhor]) melhor = t;
    return melhor;
}   
// Basicamente no ficheiro paciencias, apenas tepos 3 tipos de pilhas: MESA STOCK WASTE
// O problema é que no golf temos 9 pilhas diferentes (7 de jogo, 1 do descarte, 1 do baralho)
// Supondo que o parser ao ler o ficheiro deu estes id's aos tipos:
// ID 0 = stock (baralho); ID 1 = mesa (coluna); ID 2 = waste (descarte).
// O motor genérico nao faz a mínima ideia que o ID 1 são as colunas. Quando o jogador faz uma jogada, o motor precisa de saber qual destes é a coluna.
// 1 ciclo corre 9 vezes (9 pilhas).
// Quando p = 0, o código vê j->pilhas[0].tipo_idx. O ID desse tipo é 1 (MESA). Então o código faz contagens[1]++. A urna 1 passa a ter 1 voto.
// Isto repete-se para todas as 7 colunas. No final da 7 coluna (p=6), a urna 1 tem 7 votos.
// Quando p=7 o tipo_idx do baralho é 0(Stock). O código faz: contagens[0]++. A urna 0 passa a ter 1 voto.
// Quando p=8 o tipo_idx do baralho é 2 (waste). O código faz: contagens[2]++. A urna 2 passa a ter 1 voto.
// Agr entra o ciclo para comparar quem é o tipo q tem mais votos.
// Começamos por assumir que é o 0 (baralho), entrando no ciclo, que vai de 1 a 2, se contagens[1] > contagens[0], daqui concluimos que 7 > 1, ou seja o melhor vai ser a urna 1.
// Retorna o número 1, que diz respeito às colunas do jogo.


static int golf_idx_por_tipo(Jogo *j, const char *nome_tipo) {
    for (int t = 0; t < j->n_tipos; t++) {
        if (strcmp(j->tipos[t].nome, nome_tipo) == 0) {
            for (int p = 0; p < j->n_pilhas; p++)
                if (j->pilhas[p].tipo_idx == t) return p;
        }
    }
    return -1;
}


/**
 * @brief Encontra a pilha não-tableau com mais cartas (baralho de reserva).
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @return Índice da pilha baralho, ou -1 se não existir.
 */
static int golf_idx_baralho(Jogo *j, int tipo_tab) {
    (void)tipo_tab;
    return golf_idx_por_tipo(j, "STOCK");
}

// O objetivo desta função é devolver o índice de 0-8 da pilha que corresponde ao baralho.
// int idx vai guardar a posição da pilha que achamos ser o baralho. int maior vai guardar o numero de cartas que encontramos numa pilha.
// Entramos no ciclo que vai percorrer todas as pilhas do golf (0 a 8).
// A condição if diz que j->pilhas[p].tipo_idx != tipo_tab, ou seja o tipo das pilhas tem de ser diferente do tipo das colunas, ou seja eliminamos logo as 7 colunas do jogo.
// De seguida, o tamanho da pilha tem de ser maior do que o maior tamanho q vi ate agr.
// OU seja, como as pilhas de 0-6 sao ignoradas, ficamos com a pilha 7 (baralho) e pilha 8 (descarte).
// Entra no if, verifica a primeira condicao (não ser coluna) e o tamanho dela (16) é maior que o maior (-1), assim o maior passa a ser 16 e o idx passa a ser 7.
// Dps vem a pilha 8 e ela n passa nestas condicoes.
// A função acaba por retorna o indice da pilhas do baralho, que neste caso é 7.
// Neste caso n precisamos do ID, visto que, por definicao, a pilha do baralho é aquela que contém o maior número de cartas.

/**
 * @brief Encontra a pilha não-tableau com menos cartas (descarte).
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @param idx_bar  Índice da pilha baralho (a excluir).
 * @return Índice da pilha descarte, ou -1 se não existir.
 */
static int golf_idx_descarte(Jogo *j, int tipo_tab, int idx_bar) {
    (void)tipo_tab; (void)idx_bar;
    return golf_idx_por_tipo(j, "DESCARTE");
}


// Aqui vamos procurar a pilha que, no inicio, tem o menor numero de cartas.
// Começamos com menor = MAX_CARTAS + 1 = 105, forçando o programa a analisar todas as pilhas do jogo.
// Entrando no ciclo if, temos as seguintes condições: Nao pode ser colunas, não pode ser o indice do baralho que descobrimos na funcao anterior, o meu tamanho atual, tem de ser menor do que o menor tamanho encontrado até agora.
// Avançamos ent para a pilha 8 (1 carta), faz a pergunta 1<105, sim, logo o menor passa a ser 1 (tamanho da pilha) e o idx passa a ser 8.
// A função retorna ent 8.

/**
 * @brief Conta quantas pilhas são do tipo tableau.
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @return Número de pilhas tableau.
 */
static int golf_n_tableau(Jogo *j, int tipo_tab) {
    int n = 0;
    for (int p = 0; p < j->n_pilhas; p++)
        if (j->pilhas[p].tipo_idx == tipo_tab) n++;
    return n;
} // Devolve a quantidade de pilhas que vão ser do tipo 1(colunas), vai devolver 7.

/**
 * @brief Devolve a altura máxima das pilhas tableau.
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @return Altura máxima.
 */
static int golf_max_altura(Jogo *j, int tipo_tab) {
    int max = 1;
    for (int p = 0; p < j->n_pilhas; p++)
        if (j->pilhas[p].tipo_idx == tipo_tab && j->pilhas[p].tam > max)
            max = j->pilhas[p].tam;
    return max;
} // Esta funcao vai devolver 5, quantidade de cartas de cada pilha do golf.

/**
 * @brief Encontra o índice real da cmd-ésima pilha tableau (1-based).
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @param cmd      Número da pilha desejada (1-based).
 * @return Índice da pilha em j->pilhas, ou -1 se não encontrada.
 */
static int golf_encontrar_tab(Jogo *j, int tipo_tab, int cmd) {
    int conta = 0, idx = -1;
    for (int p = 0; p < j->n_pilhas && idx < 0; p++) {
        if (j->pilhas[p].tipo_idx == tipo_tab) {
            conta++;
            if (conta == cmd) idx = p;
        }
    }
    return idx;
}
// Esta função permite ao utilizador mover a pilha que desejar.
// Para o utilizador a contagem das pilhas começa em 1, mas para o computador começa em 0.
// Entrando no if, ignoramos o descarte e o baralho visto que nao sao iguais a tipo_tab e sempre que encontra uma coluna faz conta++.
// Segundo if: apenas quando a conta for igual ao comando digitado pelo utilizador, é q o idx passa a ser o p.
// Esta função vai retornar ent o indice da coluna q o jogador quer efetivamente mover.

/* ------------------------------------------------------------------ */
/* Renderização do Golf                                                 */
/* ------------------------------------------------------------------ */

/**
 * @brief Imprime o cabeçalho do tabuleiro Golf.
 * @param j       Ponteiro para o jogo.
 * @param idx_bar Índice da pilha baralho.
 * @param n_tab   Número de colunas tableau.
 */
static void golf_desenhar_cabecalho(Jogo *j, int idx_bar, int n_tab) {
    printf(GRN BOLD "=== %s ===\n" RESET, j->nome);
    printf("Baralho (P%d): %-2d | Descarte\n\n",
           idx_bar + 1, idx_bar >= 0 ? j->pilhas[idx_bar].tam : 0);
    for (int i = 0; i < n_tab; i++) printf(CYN "P%-2d   " RESET, i + 1);
    printf("\n");
}

/**
 * @brief Desenha uma linha horizontal das colunas tableau do Golf.
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 * @param l        Índice da linha a desenhar.
 */
static void golf_desenhar_linha(Jogo *j, int tipo_tab, int l) {
    for (int p = 0; p < j->n_pilhas; p++) {
        if (j->pilhas[p].tipo_idx != tipo_tab) continue;
        if (l < j->pilhas[p].tam) {
            desenhar_carta(j->pilhas[p].cartas[l]);
            printf(" ");
        } else {
            desenhar_vazio();
        }
    }
    printf("\n");
}

/**
 * @brief Desenha todas as linhas das colunas tableau do Golf.
 * @param j        Ponteiro para o jogo.
 * @param tipo_tab Índice do tipo tableau.
 */
static void golf_desenhar_colunas(Jogo *j, int tipo_tab) {
    int max = golf_max_altura(j, tipo_tab);
    for (int l = 0; l < max; l++)
        golf_desenhar_linha(j, tipo_tab, l);
}

/**
 * @brief Desenha a pilha de descarte do Golf.
 * @param j        Ponteiro para o jogo.
 * @param idx_desc Índice da pilha descarte.
 */
static void golf_desenhar_descarte(Jogo *j, int idx_desc) {
    printf("\n" YEL "DESCARTE: " RESET);
    if (idx_desc >= 0 && j->pilhas[idx_desc].tam > 0)
        desenhar_carta(j->pilhas[idx_desc].cartas[j->pilhas[idx_desc].tam - 1]);
    else
        printf("[     ]");
}

/**
 * @brief Desenha o tabuleiro completo do Golf.
 * @param j   Ponteiro para o jogo.
 * @param msg Mensagem de feedback.
 */
static void ui_desenhar_golf(Jogo *j, const char *msg) {
    int tipo_tab = golf_tipo_tableau(j);
    int idx_bar  = golf_idx_baralho(j, tipo_tab);
    int idx_desc = golf_idx_descarte(j, tipo_tab, idx_bar);
    int n_tab    = golf_n_tableau(j, tipo_tab);
    system("clear || cls");
    golf_desenhar_cabecalho(j, idx_bar, n_tab);
    golf_desenhar_colunas(j, tipo_tab);
    golf_desenhar_descarte(j, idx_desc);
    if (msg && msg[0]) printf("\n\n" YEL ">> %s" RESET, msg);
    printf("\n\n" BOLD "[1-%d] Mover | [%d] Biscar | [u] Undo | [s] Save | [l] Load | [r] Read | [0] Sair\n"
           RESET, n_tab, n_tab + 1);
}

/* ------------------------------------------------------------------ */
/* Renderização genérica Simple Simon                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief Devolve a altura máxima entre todas as pilhas.
 * @param j Ponteiro para o jogo.
 * @return Altura máxima.
 */
static int gen_max_altura(Jogo *j) {
    int max = 1;
    for (int p = 0; p < j->n_pilhas; p++)
        if (j->pilhas[p].tam > max) max = j->pilhas[p].tam;
    return max;
}
// Esta função serve para ver qual das pilhas tem mais cartas.
// O programa começa por assumir, que a maior pilha do jogo tem 1 carta (max=1)
// O ciclo percorre todas as pilhas do jogo.
// A cada iteração é perguntado: o tamanho desta pilha p é maior do que o máximo que ja registei até agr? Se sim a variável max 
// é atualizada, se não o programma ignora e avança para a pilha seguinte.

/**
 * @brief Imprime o cabeçalho com os rótulos de todas as pilhas.
 * @param j Ponteiro para o jogo.
 */
static void gen_desenhar_cabecalho(Jogo *j) {
    printf(MAG BOLD "=== %s ===\n\n" RESET, j->nome);
    for (int p = 0; p < j->n_pilhas; p++)
        printf(CYN "P%-2d   " RESET, p + 1);
    printf("\n");
}

/**
 * @brief Desenha uma linha horizontal de todas as pilhas.
 * @param j Ponteiro para o jogo.
 * @param l Índice da linha a desenhar.
 */
static void gen_desenhar_linha(Jogo *j, int l) {
    for (int p = 0; p < j->n_pilhas; p++) {
        if (l < j->pilhas[p].tam) {
            desenhar_carta(j->pilhas[p].cartas[l]);
            printf(" ");
        } else {
            desenhar_vazio();
        }
    }
    printf("\n");
}

/**
 * @brief Desenha o tabuleiro completo para jogos genéricos.
 * @param j   Ponteiro para o jogo.
 * @param msg Mensagem de feedback.
 */

static int gen_tipo_principal(Jogo *j) {
    int contagens[MAX_TIPOS] = {0};
    for (int p = 0; p < j->n_pilhas; p++)
        contagens[j->pilhas[p].tipo_idx]++;
    int melhor = 0;
    for (int t = 1; t < j->n_tipos; t++)
        if (contagens[t] > contagens[melhor]) melhor = t;
    return melhor;
}

/** @brief Imprime os rótulos das pilhas TAB. */
static void gen_cabecalho_tab(Jogo *j, int tipo_tab) {
    int n = 0;
    for (int p = 0; p < j->n_pilhas; p++)
        if (j->pilhas[p].tipo_idx == tipo_tab)
            printf(CYN "P%-2d   " RESET, ++n);
    printf("\n");
}

/** @brief Calcula a altura máxima das pilhas TAB. */
static int gen_max_tab(Jogo *j, int tipo_tab) {
    int max = 1;
    for (int p = 0; p < j->n_pilhas; p++)
        if (j->pilhas[p].tipo_idx == tipo_tab && j->pilhas[p].tam > max)
            max = j->pilhas[p].tam;
    return max;
}

/** @brief Desenha uma linha das pilhas TAB. */
static void gen_desenhar_linha_tab(Jogo *j, int tipo_tab, int l) {
    for (int p = 0; p < j->n_pilhas; p++) {
        if (j->pilhas[p].tipo_idx != tipo_tab) continue;
        if (l < j->pilhas[p].tam) { desenhar_carta(j->pilhas[p].cartas[l]); printf(" "); }
        else desenhar_vazio();
    }
    printf("\n");
}

static void ui_desenhar_gen(Jogo *j, const char *msg) {
    int tipo_tab = golf_tipo_tableau(j);
    system("clear || cls");
    printf(MAG BOLD "=== %s ===\n\n" RESET, j->nome);
    gen_cabecalho_tab(j, tipo_tab);
    int max = gen_max_tab(j, tipo_tab);
    for (int l = 0; l < max; l++)
        gen_desenhar_linha_tab(j, tipo_tab, l);
    if (msg && msg[0]) printf("\n" YEL ">> %s" RESET, msg);
    printf("\n\n" BOLD "Ações: " RESET "[m]over  [u]ndo  [s]ave  [l]oad [r]ead  [0]sair\n");
}

/* ------------------------------------------------------------------ */
/* Interface pública de desenho e input                                 */
/* ------------------------------------------------------------------ */

/**
 * @brief Desenha o tabuleiro adequado ao tipo de jogo.
 * @param j   Ponteiro para o jogo.
 * @param msg Mensagem de feedback.
 */
void ui_desenhar(Jogo *j, const char *msg) {
    if (strstr(j->nome, "Golf")) // Caso seja encontrada a palavra golf, é desenhado o tabuleiro do golf.
        ui_desenhar_golf(j, msg);
    else
        ui_desenhar_gen(j, msg); // Caso contrário, é desenhado o tabuleiro do simple simon.
}

/**
 * @brief Lê uma linha de input do utilizador sem crashar em input inválido.
 * @param buf    Buffer de destino.
 * @param tam    Tamanho do buffer.
 * @param prompt Texto a mostrar antes da leitura.
 */
void ui_ler_linha(char *buf, int tam, const char *prompt) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, tam, stdin)) buf[0] = '\0';
    int n = strlen(buf);
    if (n > 0 && buf[n-1] == '\n') buf[n-1] = '\0';
}

/* ------------------------------------------------------------------ */
/* Menu de seleção                                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Imprime o menu de seleção de paciências.
 * @param lista Array com os nomes dos ficheiros.
 * @param n     Número de entradas.
 */
static void menu_imprimir(char lista[][MAX_NOME], int n) {
    system("clear || cls");
    printf(MAG BOLD
        "╔══════════════════════════════╗\n"
        "║     ESCOLHE UMA PACIÊNCIA    ║\n"
        "╚══════════════════════════════╝\n" RESET);
    for (int i = 0; i < n; i++)
        printf(CYN "  [%d]" RESET " %s\n", i + 1, lista[i]);
    printf(YEL "  [0]" RESET " Sair\n\n");
}

/**
 * @brief Lê e valida a opção do menu.
 * @param n Número de opções disponíveis.
 * @return Índice (0-based), -1 para sair, -2 para input inválido.
 */
static int menu_ler_opcao(int n) {
    char buf[64]; char extra;
    int escolha;
    ui_ler_linha(buf, sizeof(buf), "Opção: ");
    if (sscanf(buf, "%d %c", &escolha, &extra) != 1) return -2;
    if (escolha == 0)                                 return -1;
    if (escolha >= 1 && escolha <= n)                 return escolha - 1;
    return -2;
}

/**
 * @brief Mostra o menu de seleção e devolve o índice escolhido.
 * @param lista Array com os nomes dos ficheiros.
 * @param n     Número de entradas.
 * @return Índice (0-based) ou -1 para sair.
 */
int ui_menu_selecao(char lista[][MAX_NOME], int n) {
    int resultado = -2;
    while (resultado == -2) {
        menu_imprimir(lista, n);
        resultado = menu_ler_opcao(n);
    }
    return resultado;
}

/* ------------------------------------------------------------------ */
/* Comandos do ciclo de jogo                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Executa o undo e atualiza a mensagem de feedback.
 * @param j   Ponteiro para o jogo.
 * @param msg Buffer para a mensagem.
 */
static void jogo_cmd_undo(Jogo *j, char *msg) {
    if (!logica_undo(j))
        strcpy(msg, RED "Nada para desfazer!" RESET);
}
static void save_path(Jogo *j, char *buf, int tam) {
    const char *base = strrchr(j->ficheiro, '/');
    base = base ? base + 1 : j->ficheiro;

    char nome_base[MAX_NOME];
    strncpy(nome_base, base, sizeof(nome_base) - 1);
    nome_base[sizeof(nome_base) - 1] = '\0';

    char *ponto = strrchr(nome_base, '.');
    if (ponto) *ponto = '\0';

    snprintf(buf, tam, "saves/%s.save", nome_base);  /* ← só muda esta linha */
}
// O snprintf, funciona como um printf, mas inves de mandar o texto para o ecrã imprime-o para dentro da string buf.
// Ele pega no nome do ficheiro original e junta-lhe ".save" no fim e guarda tudo no buf.

/**
 * @brief Grava o estado e atualiza a mensagem de feedback.
 * @param j   Ponteiro para o jogo.
 * @param msg Buffer para a mensagem.
 */
static void jogo_cmd_save(Jogo *j, char *msg) {
    char path[MAX_NOME];
    save_path(j, path, sizeof(path));
    if (save_gravar(j, path))
        strcpy(msg, GRN "Jogo guardado!" RESET);
    else
        strcpy(msg, RED "Erro ao guardar!" RESET);
}

/**
 * @brief Carrega o estado e atualiza a mensagem de feedback.
 * @param j   Ponteiro para o jogo.
 * @param msg Buffer para a mensagem.
 */
static void jogo_cmd_load(Jogo *j, char *msg) {
    char path[MAX_NOME];
    save_path(j, path, sizeof(path));
    if (save_carregar(j, path))
        strcpy(msg, GRN "Jogo carregado!" RESET);
    else
        strcpy(msg, RED "Sem save para este jogo!" RESET);
}

static void jogo_cmd_read(Jogo *j, char *msg) {
    char path[MAX_NOME];
    ui_ler_linha(path, sizeof(path), "Ficheiro: ");
    if (save_carregar(j, path))
        strcpy(msg, GRN "Ficheiro carregado!" RESET);
    else
        strcpy(msg, RED "Erro ao carregar ficheiro!" RESET);
}


/**
 * @brief Valida e executa um movimento, atualizando o histórico e os autos.
 * @param j   Ponteiro para o jogo.
 * @param o   Índice da pilha de origem (0-based).
 * @param d   Índice da pilha de destino (0-based).
 * @param q   Número de cartas a mover.
 * @param msg Buffer para a mensagem de feedback.
 */
static void jogo_executar_mov(Jogo *j, int o, int d, int q, char *msg) {
    if (logica_valida_mov(j, o, d, q)) { 
        logica_guardar_estado(j);
        logica_executar_mov(j, o, d, q);
        logica_executar_autos(j);
    } else {
        strcpy(msg, RED "Movimento inválido!" RESET);
    }
}
// Quando o jogador quer mover uma carta, esta função avalia se esse movimento é válido.
// o = origem ; d = destino ; q = quantidade (quantas cartas sao movidas de uma vez, no golf é apenas uma, mas no simple simon podem ser várias).
// Vai chamar a funcao logica_valida_mov que vai ler as flags daquela regra DSL (como f_valor_adj ou f_dec_consec) para verificar se o movimento respeita as regras do jogo atual.
// Antes de executar oque quer q seja é invocada a logica_guardar_estado, para caso o utilizador deseje fazer um undo, o jogo nao dê erro.
// De seguida, avançamos para o movimento q pretendemos fazer (logica_executar_mov)
// Por fim, chamamos a funcao logica_executar_autos q corre os movimentos automaticos definidos no ficheiro DSL.

/**
 * @brief Processa um dígito do Golf: move da coluna ou bisca do baralho.
 * @param j   Ponteiro para o jogo.
 * @param cmd Dígito introduzido (1-based).
 * @param msg Buffer para a mensagem de feedback.
 */
static void jogo_cmd_golf_digit(Jogo *j, int cmd, char *msg) {
    int tipo_tab = golf_tipo_tableau(j); // ID1 das colunas.
    int idx_bar  = golf_idx_baralho(j, tipo_tab); // Descobre onde está o baralho (indice 7)
    int idx_desc = golf_idx_descarte(j, tipo_tab, idx_bar); // Descobre onde está o descarte (indice 8)
    int n_tab    = golf_n_tableau(j, tipo_tab); // Contagem de quantas colunas existem, deverá devolver 7.
    if (idx_desc < 0) {
        strcpy(msg, RED "Sem pilha de descarte!" RESET); // COnsidera erros de escrita do ficheiro DSL.
        return;
    }
    if (cmd >= 1 && cmd <= n_tab) { // Caso o utilizador deseje escolher uma das 7 pilhas do jogo.
        int p = golf_encontrar_tab(j, tipo_tab, cmd); // Chama esta funcao, visto que no pc esta ordenado de 0-6, mas visualmente está de 1-7.
        if (p >= 0) jogo_executar_mov(j, p, idx_desc, 1, msg); // Se a coluna for encontrada, executa a jogada pretendida
    } else if (idx_bar >= 0 && cmd == n_tab + 1) { // Caso decida biscar.
        jogo_executar_mov(j, idx_bar, idx_desc, 1, msg); 
    } else {
        strcpy(msg, RED "Dígito inválido!" RESET);
    }
}

/**
 * @brief Lê e processa o comando de mover para jogos genéricos.
 * @param j   Ponteiro para o jogo.
 * @param msg Buffer para a mensagem de feedback.
 */
static void jogo_cmd_gen_mover(Jogo *j, char *msg) {
    char buf[64]; char ext2;
    int o, d, q;
    ui_ler_linha(buf, sizeof(buf), "Origem Destino Qtd: ");
    if (sscanf(buf, "%d %d %d %c", &o, &d, &q, &ext2) != 3) {
        strcpy(msg, RED "Formato: <orig> <dest> <qtd>" RESET);
    } else if (o < 1 || o > j->n_pilhas || d < 1 || d > j->n_pilhas) {
        strcpy(msg, RED "Pilhas inexistentes!" RESET);
    } else {
        jogo_executar_mov(j, o - 1, d - 1, q, msg);
    }
}

/**
 * @brief Despacha a operação para o handler correto conforme o jogo.
 * @param j   Ponteiro para o jogo.
 * @param op  Carácter do comando (já em minúsculas).
 * @param msg Buffer para a mensagem de feedback.
 */
static void jogo_processar_op(Jogo *j, char op, char *msg) {
    int eh_golf = strstr(j->nome, "Golf") != NULL;
    if      (op == 'u')                              jogo_cmd_undo(j, msg);
    else if (op == 's')                              jogo_cmd_save(j, msg);
    else if (op == 'l')                              jogo_cmd_load(j, msg);
    else if (op == 'r') 			     jogo_cmd_read(j, msg);
    else if (eh_golf && isdigit((unsigned char)op))  jogo_cmd_golf_digit(j, op - '0', msg);
    else if (!eh_golf && op == 'm')                  jogo_cmd_gen_mover(j, msg);
    else strcpy(msg, RED "Comando inválido! Use 'u','s','l','m' ou dígito." RESET);
}

/**
 * @brief Lê e valida o carácter de comando do utilizador.
 * @param buf Buffer com a linha lida.
 * @param op  Ponteiro onde o carácter será guardado.
 * @param msg Buffer para a mensagem de feedback em caso de erro.
 * @return 1 se leitura válida, 0 em caso de erro.
 */
static int jogo_ler_op(char *buf, char *op, char *msg) {
    char extra;
    int lido = sscanf(buf, " %c %c", op, &extra); //Colaca o primeiro numero em op e o segundo em extra.
    *op = (char)tolower((unsigned char)*op); // Caso o utilizador digite em letras maiusculas.
    if (lido != 1) {
        strcpy(msg, RED "Escreve apenas um comando." RESET);
        return 0;
    }
    return 1;
}

/**
 * @brief Interpreta o buffer de input e executa o comando correspondente.
 * @param j     Ponteiro para o jogo.
 * @param buf   Buffer com a linha lida.
 * @param msg   Buffer para a mensagem de feedback.
 * @param ativo Ponteiro para a flag de controlo do loop.
 */
static void jogo_processar_buf(Jogo *j, char *buf, char *msg, int *ativo) {
    char op;
    if (!jogo_ler_op(buf, &op, msg)) return;
    if (op == '0') { *ativo = 0; return; }
    jogo_processar_op(j, op, msg);
}

/* ------------------------------------------------------------------ */
/* Loop principal da partida                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Executa uma iteração do ciclo de jogo.
 * @param j   Ponteiro para o jogo.
 * @param msg Buffer para a mensagem de feedback.
 * @return 1 para continuar, 0 para terminar.
 */
static int jogo_tick(Jogo *j, char *msg) {
    char buf[64];
    ui_desenhar(j, msg);
    msg[0] = '\0';
    if (logica_verificar_vitoria(j)) {
        ui_ler_linha(buf, sizeof(buf), GRN BOLD "\n*** VITÓRIA! Parabéns! Carrega Enter para sair. ***\n" RESET);
        return 0;
    }
    int ativo = 1;
    ui_ler_linha(buf, sizeof(buf), "> ");
    if (buf[0] != '\0') jogo_processar_buf(j, buf, msg, &ativo);
    return ativo;
}

// Caso o utilizador tenha digitado algo (buf[0]!='\0') avança para a processar buf.
// Passamos &ativo, caso o utilizador digite o comando de saída isto saia imediatamente.

/**
 * @brief Controla o loop principal de uma partida completa.
 * @param j Ponteiro para o jogo já inicializado e distribuído.
 */
void ui_executar_jogo(Jogo *j) {
    char msg[256] = "Boa sorte!"; // Cria o buffer de comunicação inicializado c as mensagem Boa sorte, vai ser aq q vai aparecer todas as outras mensagens p.ex erros.
    int  ativo    = 1;
    while (ativo) ativo = jogo_tick(j, msg); // Caso o jogo_tick q depende daquilo q o utilizador digitou, devolver 1, o jogo continua
                                            // caso devolva 0 o jogo para.
} 
