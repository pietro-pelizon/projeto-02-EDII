#ifndef PROJETO_02_EDII_GRAFO_H
#define PROJETO_02_EDII_GRAFO_H

typedef struct stVertex vertex_t;
typedef struct stEdge edge_t;
typedef struct stGraph graph_t;

#include <stdbool.h>
#include "lista.h"

/// @brief Inicializa um grafo com as funções corretas para manipular
/// os tipos de dados do vértice e aresta
/// @param comparator Função para comparar dois elementos genéricos
/// @param destructor_edge_data Função para liberar o payload (data) da aresta
/// @param destructor_vertex_data Função para liberar o payload (data) do vértice
/// @return Retorna um ponteiro para o grafo criado ou NULL se falhar
graph_t *graph_init(void (*comparator)(void *a, void *b),
    void (*destructor_edge_data)(void *data),
    void (*destructor_vertex_data)(void *data));

/// @brief Adiciona um vértice ao grafo contendo "void *data"
/// @param g Ponteiro para o grafo
/// @param data Dado a ser contido pelo vértice
/// @param vertex_id Identificador do vértice
/// @return false caso não seja possível criar o vértice (vértice já existe), e true para criação bem sucedida
bool graph_add_vertex(graph_t *g, void *data, const char *vertex_id);

/// @brief Cria uma aresta direcionada conectando o vértice de origem ao de destino
/// @param g Ponteiro para o grafo
/// @param data Dado genérico associado à aresta
/// @param src_id Identificador do vértice de origem
/// @param target_id Identificador do vértice de destino
/// @param street_name Nome da rua correspondente a essa aresta
/// @return true se adicionada com sucesso, false se os vértices não existirem ou a aresta já existir
bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id, char *street_name);

/// @brief Verifica se existe uma aresta saindo do vértice V em direção ao vértice U
/// @param g Ponteiro para o grafo
/// @param id_v Identificador do vértice de origem
/// @param id_u Identificador do vértice de destino
/// @return true se forem adjacentes (V ⇨ U), false caso contrário
bool is_adjacente(graph_t *g, const char *id_v, const char *id_u);

/// @brief Busca um vértice no grafo pelo seu identificador
/// @param g Ponteiro para o grafo
/// @param vertex_id Identificador do vértice procurado
/// @return Ponteiro para o vértice (vertex_t) ou NULL se não for encontrado
vertex_t *graph_get_vertex(graph_t *g, const char *vertex_id);

/// @brief Busca uma aresta específica entre dois vértices
/// @param g Ponteiro para o grafo
/// @param src_id Identificador do vértice de origem
/// @param target_id Identificador do vértice de destino
/// @return Ponteiro para a aresta (edge_t) ou NULL se ela não existir
edge_t *graph_get_edge(graph_t *g, const char *src_id, const char *target_id);

/// @brief Remove e libera a memória de uma aresta específica entre dois vértices
/// @param g Ponteiro para o grafo
/// @param src_id Identificador do vértice de origem
/// @param target_id Identificador do vértice de destino
/// @return true se removida com sucesso, false se não for encontrada
bool graph_remove_edge(graph_t *g, const char *src_id, const char *target_id);

/// @brief Remove um vértice, limpando suas arestas de saída e as que apontavam para ele
/// @param g Ponteiro para o grafo
/// @param vertex_id Identificador do vértice a ser removido
/// @return true se removido com sucesso, false se o vértice não existir
bool graph_remove_vertex(graph_t *g, const char *vertex_id);

/// @brief Retorna a estrutura que armazena os vizinhos (arestas de saída) de um vértice
/// @param g Ponteiro para o grafo
/// @param vertex_id Identificador do vértice
/// @return Ponteiro para a lista_t contendo as arestas, ou NULL se o vértice não existir
lista_t *graph_get_neighbors(graph_t *g, const char *vertex_id);

/// @brief Destrói o grafo inteiro, liberando vértices, arestas e estruturas internas
/// @param g Ponteiro para o grafo a ser destruído
void graph_destroy(graph_t *g);

#endif //PROJETO_02_EDII_GRAFO_H