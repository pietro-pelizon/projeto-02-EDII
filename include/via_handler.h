#ifndef PROJETO_02_EDII_VIA_HANDLER_H
#define PROJETO_02_EDII_VIA_HANDLER_H

/*
 * @file via_handler.h
 * @brief Processador do mapa viário (.via).
 *
 * @details
 * Lê e interpreta os dados de ruas e cruzamentos, instanciando os vértices
 * e arestas diretamente no TAD Grafo.
*/


#include "grafo.h"

/// @brief Lê o arquivo (.via) e processa os dados
/// conforme a necessidade
/// @param path_via Caminho para o arquivo (.via)
/// @param g Ponteiro para o grafo
void via_handler(char *path_via, graph_t *g);

#endif //PROJETO_02_EDII_VIA_HANDLER_H
