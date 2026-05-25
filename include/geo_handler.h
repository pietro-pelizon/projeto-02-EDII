#ifndef PROJETO_02_EDII_GEO_HANDLER_H
#define PROJETO_02_EDII_GEO_HANDLER_H
#include <stdio.h>

#include "grafo.h"

/// @brief Lê o arquivo (.geo), cria as quadras
/// e as adiciona a seu exhash respectivo
/// @param caminho_geo Path para o arquivo (.geo)
/// @param g Ponteiro para o grafo
/// @return exhash que contém as quadras
exhash_t *geo_handler(const char *caminho_geo, graph_t *g);


#endif //PROJETO_02_EDII_GEO_HANDLER_H
