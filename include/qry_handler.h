#ifndef PROJETO_02_EDII_QRY_HANDLER_H
#define PROJETO_02_EDII_QRY_HANDLER_H
#include <stdio.h>

#include "grafo.h"

/*
 * @file qry_handler.h
 * @brief Orquestrador de Comandos e Consultas (.qry).
 *
 * @details
 * Módulo de alto nível responsável por interpretar o arquivo interativo de entrada (.qry)
 * e aplicar regras de negócio complexas sobre a malha viária (Grafo). Faz a ponte
 * entre os TADs puros e as exigências do projeto, processando algoritmos clássicos.
 *
 * Funcionalidades principais:
 * - Modificação e filtragem espacial (comando "mvm").
 * - Identificação de componentes conexos via Busca em Largura (BFS) (comando "regs").
 * - Geração de Árvores Geradoras Mínimas (Kruskal) para ampliações viárias (comando "exp").
 * - Geração automática das saídas tabulares (.txt) e visuais (.svg).
 */


/// @brief Função que lê o arquivo (.qry) e faz o parsing do comandos
/// @param path_qry Nome ou localização do arquivo (.qry)
/// @param g Ponteiro para o grafo geral do programa
/// @param quadras Ponteiro para o hashmap que guarda as quadras
/// @param svg Ponteiro para o arquivo (.svg)
/// @param txt Ponteiro para o arquivo (.txt)
void qry_handler(char *path_qry, graph_t *g, exhash_t *quadras, FILE *svg, FILE *txt);

#endif //PROJETO_02_EDII_QRY_HANDLER_H
