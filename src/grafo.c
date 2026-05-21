#include "../include/grafo.h"

#include <assert.h>

#include "exhash.h"
#include "../include/lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ======= Declarações estáticas ========

static void remove_neighbors_edges(void *record_data, void *context);

// Struct para transportar o ID alvo e a função de free do grafo
// Como a exhash_foreach precisa de um callback fixo, precisamos de uma nova
// struct que representa os dados que a função recebe
typedef struct stContext{
    const char *target_id;
    void (*destructor_edge_data)(void *);
} ctx_remove_t;

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

    graph -> vertices = exhash_init(sizeof(vertex_t *), 512);

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


bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id, char *street_name) {
    assert(g != NULL && data != NULL && src_id != NULL && target_id != NULL);

    // Aloca memória e procura o vértice de origem no hashmap do grafo
    vertex_t *src_v = malloc(sizeof(vertex_t));
    exhash_search(g -> vertices, src_id, &src_v);

    // Aloca memória e procura o vértice de destino no hashmap do grafo
    vertex_t *target_v = malloc(sizeof(vertex_t));
    exhash_search(g -> vertices, target_id, &target_v);


    // Se um dos vértices não existir, não é possível
    // criar a aresta, pois ela já existe
    if (!src_v || !target_v) return false;

    // Checa se já existe uma aresta idêntica, caso exista, não cria nova aresta
    if (search_lista(src_v -> adjacent, ((void *)target_id), cmp_target_edge)) {
        return false;
    }

    // Cria nova aresta DIRECIONADA e preenche suas informações
    edge_t *new_edge = malloc (sizeof(edge_t));
    assert(new_edge != NULL);

    new_edge -> target_id = malloc (strlen(target_id) + 1);
    strncpy(new_edge -> target_id, target_id, strlen(target_id) + 1);

    new_edge -> id = street_name;
    new_edge -> data = data;

    // Insere a aresta na lista de adjacência do vértice de origem
    // Nova aresta parte de src_v para target_v
    insert_tail(src_v -> adjacent, new_edge);

    return true;
}

bool is_adjacente(graph_t *g, const char *id_v, const char *id_u) {
    assert(g != NULL && id_v != NULL && id_u != NULL);

    vertex_t *v = malloc (sizeof(vertex_t));
    exhash_search(g -> vertices, id_v, &v);

    vertex_t *u = malloc(sizeof(vertex_t));
    exhash_search(g -> vertices, id_u, &u);

    // Caso um dos dois vértices, impossível checar adjacência
    // então assumimos que é false
    if (!v || !u) return false;

    // Procura na lista de adjacência de V se existe o vértice U
    if (search_lista(v -> adjacent, ((void *)id_u), cmp_target_edge)) {
        return true;
    }

    return false;
}

vertex_t *graph_get_vertex(graph_t *g, const char *vertex_id) {
    assert(g != NULL && vertex_id != NULL);

    vertex_t *v = NULL;

    if (!exhash_search(g -> vertices, vertex_id, &v)) {
        fprintf(stderr, "Vértice de ID %s não encontrado.\n", vertex_id);
        return NULL;
    }

    return v;
}

edge_t *graph_get_edge(graph_t *g, const char *src_id, const char *target_id) {
    assert(g != NULL);

    vertex_t *src_v = NULL;

    // Acha o vértice de origem em O(1)
    exhash_search(g -> vertices, src_id, &src_v);
    if (!src_v) return NULL;

    // Busca a aresta na lista dele usando a função de procurar na lista
    return search_lista(src_v -> adjacent, (void *)target_id, cmp_target_edge);
}

bool graph_remove_edge(graph_t *g, const char *src_id, const char *target_id) {
    assert(g != NULL);

    edge_t *e = graph_get_edge(g, src_id, target_id);
    if (!e) return false;

    g -> destructor_edge_data(e);

    return true;
}

bool graph_remove_vertex(graph_t *g, const char *vertex_id) {
    assert(g != NULL);

    // Acha o vértice desejado
    vertex_t *v = graph_get_vertex(g, vertex_id);
    if (!v) return false;

    // Remove cada aresta associada ao vértice a ser destruído
    ctx_remove_t context = { vertex_id, g -> destructor_edge_data };
    exhash_foreach(g -> vertices, remove_neighbors_edges, &context);

    // Itera pela sua lista de adjacência destruindo as arestas
    free_lista(v -> adjacent, g -> destructor_edge_data);

    // Remove id e dados
    free(v -> id);
    if (g -> destructor_vertex_data && v -> data) {
        g -> destructor_vertex_data(v);
    }

    // Remove o vértice em si
    free(v);

    // Remove o vértice do hashmap geral do grafo
    void *removed = exhash_remove(g -> vertices, vertex_id);
    free(removed);

    return true;
}

lista_t *graph_get_neighbors(graph_t *g, const char *vertex_id) {
    assert(g != NULL);

    vertex_t *v = graph_get_vertex(g, vertex_id);
    if (!v) return NULL;

    return v -> adjacent;
}

void graph_destroy(graph_t *g) {
    assert (g != NULL);

static edge_destroy_internal(void *edge_data) {
    edge_t *e = edge_data;

    free(e -> id);
}


}

// O callback que será chamado para CADA vértice do grafo
static void remove_neighbors_edges(void *record_data, void *context) {
    //Como o exhash guarda ponteiros, record_data é um (vertex_t **)
    vertex_t *v = *(vertex_t **)record_data;
    ctx_remove_t *ctx = (ctx_remove_t *)context;

    // Tenta remover a aresta que aponta para o vértice que está morrendo
    edge_t *aresta_removida = remove_first_data(v -> adjacent, (void *)ctx -> target_id, cmp_target_edge);

    if (aresta_removida) {
        free(aresta_removida -> target_id);
        if (ctx -> destructor_edge_data && aresta_removida -> data) {
            ctx -> destructor_edge_data(aresta_removida -> data);
        }
        free(aresta_removida);
    }
}




