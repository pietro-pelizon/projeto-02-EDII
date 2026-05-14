#include "../include/grafo.h"
#include "../include/lista.h"

// Vértice genérico para guardar qualquer tipo de dado
typedef struct stVertex {
    char *id;
    void *data;
    lista *adjacent;

} t_vertex;

// Aresta genérica para guardar qualquer tipo de dado
typedef struct stEdge {
    char *id;
    char *id_destino;
    void *data;
} t_edge;

// Grafo genérico que será inicializado com as funções adequadas para manipular
// qualquer tipo dos dados contidos na aresta e vértice
typedef struct stGraph {
    lista *vertices;

    void (*comparator)(void *data);
    void (*destructor_edge_data)(void *data);
    void (*destructor_vertex_data)(void *data);
}t_graph;