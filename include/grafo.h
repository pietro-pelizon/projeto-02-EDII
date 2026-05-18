#ifndef PROJETO_02_EDII_GRAFO_H
#define PROJETO_02_EDII_GRAFO_H

typedef struct stVertex vertex_t;
typedef struct stEdge edge_t;
typedef struct stGraph graph_t;

#include <stdbool.h>
#include "lista.h"

/// @brief Inicializa um grafo com n vertices
/// @param comparator
/// @param destructor_edge_data
/// @param destructor_vertex_data
/// @return Retorna um ponteiro para o grafo criado
graph_t *graph_init(void (*comparator)(void *a, void *b),
    void (*destructor_edge_data)(void *data),
    void (*destructor_vertex_data)(void *data));


bool graph_add_vertex(graph_t *g, void *data, const char *vertex_id);

bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id);

bool is_adjacente(graph_t *g, const char *id_v, const char *id_u);

vertex_t *graph_get_vertex(graph_t *g, const char *id_vertice);

edge_t *graph_remove_edge(graph_t *g, const char *id_origem, const char *id_destino);

vertex_t *graph_remove_vertex(graph_t *g, const char *id_vertice);

lista_t *graph_get_neighbors(graph_t *g, const char *id_vertice);

void graph_destroy(graph_t *g);

#endif //PROJETO_02_EDII_GRAFO_H