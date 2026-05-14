#ifndef PROJETO_02_EDII_GRAFO_H
#define PROJETO_02_EDII_GRAFO_H

typedef struct stVertex t_vertex;
typedef struct stEdge t_edge;
typedef struct stGraph t_graph;

#include <stdbool.h>
#include "lista.h"

/// @brief Inicializa um grafo com n vertices
/// @return Retorna um ponteiro para o grafo criado
t_graph *graph_init(int n);

bool graph_add_vertex(t_graph *g, t_vertex *v);

bool graph_add_edge(t_graph *g, t_edge *a);

bool is_adjacente(t_graph *g, const char *id_v, const char *id_u);

t_vertex *graph_get_vertex(t_graph *g, const char *id_vertice);

t_edge *graph_remove_edge(t_graph *g, const char *id_origem, const char *id_destino);

t_vertex *graph_remove_vertex(t_graph *g, const char *id_vertice);

lista *graph_get_neighbors(t_graph *g, const char *id_vertice);

void graph_destroy(t_graph *g);

#endif //PROJETO_02_EDII_GRAFO_H