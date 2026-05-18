#include "../include/grafo.h"

#include <assert.h>

#include "exhash.h"
#include "../include/lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Vértice genérico para guardar qualquer tipo de dado
typedef struct stVertex {
    char *id;
    void *data;
    lista_t *adjacent;

} vertex_t;

// Aresta genérica para guardar qualquer tipo de dado
typedef struct stEdge {
    char *id;
    char *target_id;
    void *data;
} edge_t;

// Grafo genérico que será inicializado com as funções adequadas para manipular
// qualquer tipo dos dados contidos na aresta e vértice
typedef struct stGraph {
    exhash_t *vertices;

    void (*comparator)(void *a, void *b);
    void (*destructor_edge_data)(void *data);
    void (*destructor_vertex_data)(void *data);
}graph_t;

graph_t *graph_init(void (*comparator)(void *a, void *b),
    void (*destructor_edge_data)(void *data),
    void (*destructor_vertex_data)(void *data)) {

    graph_t *graph = malloc(sizeof(graph_t));
    assert(graph != NULL);

    graph -> vertices = exhash_init(sizeof(vertex_t *), 128);

    graph -> comparator = comparator;
    graph -> destructor_edge_data = destructor_edge_data;
    graph -> destructor_vertex_data = destructor_vertex_data;

    return graph;

}

bool graph_add_vertex(graph_t *g, void *data, const char *id) {

    // Checa se vértice com determinado ID já existe.
    // Caso exista, a função retorna false
    if (exhash_search(g -> vertices, id, NULL)) {
        fprintf(stderr, "Vértice já existente no grafo.");
        return false;
    }

    // Aloca memória para o novo vértice
    vertex_t *new_vertex = malloc(sizeof(vertex_t));
    assert(new_vertex != NULL);

    // Inicializando campos do vértice
    new_vertex -> id = malloc(strlen(id) + 1);
    strcpy(new_vertex -> id, id);

    new_vertex -> data = data;
    new_vertex -> adjacent = init_lista();

    // Adiciona um ponteiro do novo vértice ao hashmap do grafo
    exhash_insert(g -> vertices, &new_vertex, id);

    return true;
}

// Função de comparação do destino da aresta, para checar duplicações
// ao adicionar novas arestas ao grafo
static int cmp_target_edge(void *edge, void *target_edge) {
    edge_t *c = (edge_t*) edge;
    char *wanted_id = (char *)target_edge;

    return strcmp(c -> id, wanted_id);
}

bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id) {
}