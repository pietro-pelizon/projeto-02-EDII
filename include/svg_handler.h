#ifndef PROJETO_02_EDII_SVG_HANDLER_H
#define PROJETO_02_EDII_SVG_HANDLER_H


#include <stdio.h>

#include "exhash.h"
#include "grafo.h"
#include "ponto.h"
#include "quadra.h"

/*
 * ------- MÓDULO DE SAÍDA SVG -------
 * Este módulo funciona como uma biblioteca de funções utilitárias
 * responsável por gerar e escrever o arquivo de saída no formato SVG.
 * Ele provê funcionalidades para:
 *
 * Gerenciamento de Arquivo: Controlar o ciclo de vida do arquivo SVG,
 * incluindo sua criação ('svg_init'), a e o seu fechamento ('fecha_svg');
 *
 * Abstração da Sintaxe SVG: Esconder a complexidade da sintaxe SVG,
 * permitindo que o resto do programa desenhe formas simplesmente
 * chamando uma função, sem precisar conhecer os detalhes das tags
 * '<rect>', '<circle>', etc.
 */

/// @brief Cria, abre e inicializa um arquivo SVG com o cabeçalho padrão.
/// @param caminho_arquivo O nome/caminho do arquivo a ser criado.
/// @return Retorna um ponteiro para o arquivo (FILE*) pronto para escrita, ou NULL se ocorrer um erro.
FILE *svg_init(const char *caminho_arquivo);

/// @brief Desenha um retângulo representando uma quadra no arquivo SVG.
/// @param svg Ponteiro para o arquivo SVG aberto para escrita.
/// @param q Ponteiro para a quadra com as dimensões e coordenadas a serem desenhadas.
void svg_quadra_insert(FILE *svg, const quadra_t *q);

/// @brief Insere uma linha vertical pontilhada
/// com a posição do endereço e seu id na outra ponta
/// @param svg Ponteiro para o arquivo (.svg)
/// @param x Coordenada x para inserir linha
/// @param y Coordenada y para inserir linha
/// @param id Identificador do registrador
void pos_endereco(FILE *svg, double x, double y, char *id);

/// @brief Insere a tag de fechamento `</svg>` e encerra o fluxo do arquivo com segurança.
/// @param svg Ponteiro para o arquivo SVG a ser fechado.
void fecha_svg(FILE *svg);

/// @brief Insere um retângulo com opacidade 0.5 no (.svg)
/// @param svg Ponteiro para o arquivo (.svg)
/// @param cor Cor do retângulo
/// @param min_x Menor X da bounding box
/// @param min_y Menor Y da bounding box
/// @param max_x Maior X da bounding box
/// @param max_y Maior Y da bounding box
void rect_componente_conexo(FILE *svg, char *cor, double min_x, double min_y, double max_x, double max_y);

/// @brief Desenha uma linha vermelha o (.svg), indicando a MST
/// @param svg Ponteiro para o arquivo (.svg)
/// @param id_origem Identificador do vértice de origem
/// @param id_destino Identificador do vértice de destino
/// @param g Ponteiro para o grafo
void linha_vermelha_exp(FILE *svg, const char *id_origem, const char *id_destino, graph_t *g);



#endif //PROJETO_02_EDII_SVG_HANDLER_H
