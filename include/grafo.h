#ifndef PROJETO_02_EDII_GRAFO_H
#define PROJETO_02_EDII_GRAFO_H


/*
 * @file grafo.h
 * @brief Tipo Abstrato de Dados Grafo (Direcionado e Ponderado).
 *
 * @details
 * Estrutura o mapa da cidade gerenciando vértices (esquinas) e arestas (ruas)
 * de forma universal através de ponteiros genéricos (void *payload).
 * O módulo foi projetado utilizando a técnica de Ponteiros Opacos (Opaque Pointers)
 * com alocação dinâmica (Heap), garantindo encapsulamento total e impedindo que
 * regras de negócio vazem para dentro do TAD. Utiliza tabelas de espalhamento (Hashmap)
 * para indexação de vértices e listas de adjacência.
 *
 * Funcionalidades principais:
 * - Busca de vértices em tempo constante O(1) através do ID (String).
 * - Desacoplamento de dados: armazena qualquer tipo de struct via `void *`.
 * - Travessia segura: varredura do grafo protegida por funções de callback (`foreach`),
 * impedindo que arquivos externos manipulem os nós internos diretamente.
 */


typedef struct stVertex vertex_t;
typedef struct stEdge edge_t;
typedef struct stGraph graph_t;


#include <stdbool.h>
#include "exhash.h"
#include "lista.h"

typedef bool (*edge_filter_fn)(void *edge_data, void *context);

/// @brief Inicializa um grafo com as funções corretas para manipular
/// os tipos de dados do vértice e aresta
/// @param destructor_edge_data Função para liberar o payload (data) da aresta
/// @param destructor_vertex_data Função para liberar o payload (data) do vértice
/// @return Retorna um ponteiro para o grafo criado ou NULL se falhar
graph_t *graph_init(void (*destructor_edge_data)(void *data),
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
bool graph_is_adjacent(graph_t *g, const char *id_v, const char *id_u);

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
/// @return Ponteiro para a list_t contendo as arestas, ou NULL se o vértice não existir
list_t *graph_get_neighbors(graph_t *g, const char *vertex_id);

/// @brief Destrói o grafo inteiro, liberando vértices, arestas e estruturas internas
/// @param g Ponteiro para o grafo a ser destruído
void graph_destroy(graph_t *g);

/// @param g Ponteiro para o vértice
/// @return Retorna a quantidade total de vértices que o grafo irá ter
int graph_get_total_vertices(graph_t *g);

/// @brief Define a quantidade de vértices que o grafo terá
/// @param g Ponteiro para o grafo
/// @param new_total_vertices Quantidade de vértices
void graph_set_total_vertices(graph_t *g, int new_total_vertices);

/// @brief Aplica uma ação em cada vértice do grafo
/// seguindo determinada condição dada por contexto
/// @param g Ponteiro para o grafo
/// @param callback Função de callback da ação
/// @param context Contexto necessário para saber se deve ou não realizar a ação
void graph_foreach_vertex(graph_t *g, void (*callback)(const char *id, void *vertex_data, list_t *adjacent, void *context), void *context);

/// @param v Ponteiro para o vértice
/// @return Dado interno do vértice
void *vertex_get_data(vertex_t *v);

/// @param e Ponteiro para a aresta
/// @return Retorna o ID do vértice de destino
const char *edge_get_target_id(edge_t *e);

/// @param e Ponteiro para a aresta
/// @return Retorna o dado interno da aresta
void *edge_get_data(edge_t *e);

/* ___________________________________________________________________ */

/// @brief Usando o algoritmo criado por Dijkstra, descobre o caminho
/// mais eficiente para ir do ponto A até ponto B num grafo ponderado
/// @param g Ponteiro para o grafo
/// @param flag_tempo Decide se irá calcular considerando tempo mínimo
/// de travessia ou caminho mais curto
/// @param id_src Identificador do ponto de origem
/// @param id_dst Identificador do ponto de destino
/// @param custo_out Variável para guardar o custo total do caminho
/// @return Retorna uma lista ordenada com os identificadores
/// dos vértices que compoem o melhor caminho
list_t *dijkstra(graph_t *g, bool flag_tempo, char *id_src,  char *id_dst, double *custo_out);

/// @brief Executa o algoritmo de Tarjan para encontrar Componentes Fortemente Conexos (SCCs).
/// @param g Grafo contendo a estrutura viária da cidade.
/// @param filter Callback de filtragem que decide quais arestas são consideradas
/// para o cálculo dos componentes.
/// @param filter_ctx Contexto adicional passado para a função de filtragem.
/// @return Uma lista contendo sub-listas (cada sub-lista é um componente conexo).
list_t *tarjan(graph_t *g, edge_filter_fn filter, void *filter_ctx);


#endif //PROJETO_02_EDII_GRAFO_H