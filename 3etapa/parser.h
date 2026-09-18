/**
 * @file parser.h
 * @brief Protótipos e dependências do módulo de parsing da DSL.
 *
 * Este módulo é responsável por ler ficheiros .paciencia e
 * preencher a estrutura Jogo com os dados da DSL.
 */

#ifndef PARSER_H
#define PARSER_H

#include "paciencia.h"

/**
 * @brief Lê e interpreta um ficheiro DSL, preenchendo a estrutura Jogo.
 * @param j    Ponteiro para o jogo a inicializar.
 * @param path Caminho para o ficheiro .paciencia.
 * @return 1 em sucesso, 0 em erro.
 */
int parser_ler(Jogo *j, const char *path);

/**
 * @brief Lista os ficheiros .paciencia disponíveis numa pasta.
 * @param pasta Caminho para a pasta.
 * @param lista Array de strings para guardar os nomes.
 * @param max   Tamanho máximo do array.
 * @return Número de ficheiros encontrados.
 */
int parser_listar(const char *pasta, char lista[][MAX_NOME], int max);

#endif /* PARSER_H */
