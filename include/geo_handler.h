#ifndef PROJETO_02_EDII_GEO_HANDLER_H
#define PROJETO_02_EDII_GEO_HANDLER_H

/*
 * @file geo_handler.h
 * @brief Processador do arquivo base (.geo).
 *
 * @details
 * Responsável pelo parsing e alocação das formas geométricas primárias,
 * textos e configurações iniciais da cidade.
*/

#include "grafo.h"

/// @brief Lê o arquivo (.geo), cria as quadras
/// e as adiciona a seu exhash respectivo
/// @param caminho_geo Path para o arquivo (.geo)
/// @return exhash que contém as quadras
exhash_t *geo_handler(const char *caminho_geo);


#endif //PROJETO_02_EDII_GEO_HANDLER_H
