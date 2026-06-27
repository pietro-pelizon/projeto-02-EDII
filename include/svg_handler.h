#ifndef PROJETO_02_EDII_SVG_HANDLER_H
#define PROJETO_02_EDII_SVG_HANDLER_H


#include <stdio.h>

#include "grafo.h"
#include "quadra.h"

/*
 * ------- MÓDULO DE SAÍDA SVG -------
 * Este módulo funciona como uma biblioteca de funções utilitárias
 * responsável por gerar e escrever o arquivo de saída no formato SVG.
 * Ele provê funcionalidades para:
 *
 * Gerenciamento de Arquivo: Controlar o ciclo de vida do arquivo SVG,
 * incluindo sua criação ('svg_init'), a e o seu fechamento ('svg_close');
 *
 * Abstração da Sintaxe SVG: Esconder a complexidade da sintaxe SVG,
 * permitindo que o resto do programa desenhe formas simplesmente
 * chamando uma função, sem precisar conhecer os detalhes das tags
 * '<rect>', '<circle>', etc.
 */


/// @brief Cria, abre e inicializa um arquivo (.svg) com a tag raiz e o sistema de coordenadas definido.
/// @param caminho O nome ou caminho do arquivo .svg a ser criado no sistema.
/// @param min_x Limite esquerdo (x) da área de visualização (ViewBox).
/// @param min_y Limite superior (y) da área de visualização (ViewBox).
/// @param max_x Limite direito (x) da área de visualização (ViewBox).
/// @param max_y Limite inferior (y) da área de visualização (ViewBox).
/// @return Ponteiro para o arquivo aberto, ou NULL em caso de erro na abertura.
FILE *svg_init(const char *caminho, double min_x, double min_y, double max_x, double max_y);

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
void svg_posicao_endereco(FILE *svg, double x, double y, char *id);

/// @brief Insere a tag de fechamento `</svg>` e encerra o fluxo do arquivo com segurança.
/// @param svg Ponteiro para o arquivo SVG a ser fechado.
void svg_close(FILE *svg);

/// @brief Insere um retângulo com opacidade 0.5 no (.svg)
/// @param svg Ponteiro para o arquivo (.svg)
/// @param cor Cor do retângulo
/// @param min_x Menor X da bounding box
/// @param min_y Menor Y da bounding box
/// @param max_x Maior X da bounding box
/// @param max_y Maior Y da bounding box
void svg_rect_componente_conexo(FILE *svg, char *cor, double min_x, double min_y, double max_x, double max_y);

/// @brief Desenha uma linha vermelha o (.svg), indicando a MST
/// @param svg Ponteiro para o arquivo (.svg)
/// @param id_origem Identificador do vértice de origem
/// @param id_destino Identificador do vértice de destino
/// @param g Ponteiro para o grafo
/// @param cor Cor aplicada a linha que será criada
void svg_linha_caminho(FILE *svg, const char *id_origem, const char *id_destino, graph_t *g, const char *cor);

/// @brief Desenha placa de início de fim do caminho dado pelo comando "p?"
/// @param svg Ponteiro para o arquivo (.svg)
/// @param id_src Identificador do vértice de origem
/// @param id_dst Identificador do vértice de destino
/// @param g Ponteiro para o grafo
void svg_desenha_placas(FILE *svg, const char *id_src, const char *id_dst, graph_t *g);

/// @brief Cria a animação visual de um trajeto no arquivo (.svg).
/// @param svg Ponteiro para o arquivo (.svg) de saída.
/// @param caminho Lista encadeada com a sequência de IDs dos vértices que formam o trajeto.
/// @param g Ponteiro para o grafo (usado para buscar as coordenadas das ruas).
/// @param id_path Identificador único da animação (ID do elemento na tag XML).
/// @param velocidade Duração total ou velocidade da animação (geralmente em segundos).
/// @param cor Cor do caminho
void svg_anima_caminho(FILE *svg, list_t *caminho, graph_t *g, const char *id_path, double velocidade, const char *cor);

/// @brief Função de callback para desenhar as quadras no (.svg).
/// @param record_data Quadra a ser desenhada.
/// @param context Arquivo (.svg).
void svg_quadra_foreach_cb(void *record_data, void *context);

/// @brief Desenha a infraestrutura básica do mapa (vértices e arestas) no arquivo (.svg).
/// @param svg Ponteiro para o arquivo (.svg).
/// @param g Ponteiro para o grafo contendo os dados viários da cidade.
void svg_desenha_mapa_base(FILE *svg, graph_t *g);


#endif //PROJETO_02_EDII_SVG_HANDLER_H
