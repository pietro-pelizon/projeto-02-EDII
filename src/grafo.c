#include "../include/grafo.h"

#include <assert.h>
#include <math.h>
#include "../include/exhash.h"
#include "../include/lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "rua.h"
#include "utils.h"

#define EXHASH_BUCKET_BYTES(record_size, n_entries) \
((sizeof(uint64_t) + (record_size)) * (n_entries) + sizeof(uint16_t) * 2)

// ======= CONTEXT STRUCTS ========

typedef struct {
    graph_t *graph;
    exhash_t *costs;
    exhash_t *parents;
    pqueue_t *min_heap;
    bool use_time;
} dijkstra_ctx_t;

typedef struct {
    const char *target_id;
    void (*destructor_edge_data)(void *);
} remove_edge_ctx_t;

// ======= STATIC DECLARATIONS ========

static void remove_incoming_edges_cb(void *record_data, void *context);
static void destroy_graph_internal_cb(void *record_data, void *context);
static void relax_edges(const char *u_id, double current_cost, dijkstra_ctx_t *ctx);
static list_t *reconstruct_path(const char *src_id, const char *dst_id, exhash_t *parents);
static int compare_target_id(void *target_edge, void *edge);

// ======= GRAPH STRUCTS ========

typedef struct stVertex {
    char *id;
    void *data;
    list_t *adjacent;
} vertex_t;

typedef struct stEdge {
    char *id;
    char *target_id;
    void *data;
} edge_t;

typedef struct stGraph {
    exhash_t *vertices;
    int total_vertices;
    void (*destructor_edge_data)(void *data);
    void (*destructor_vertex_data)(void *data);
} graph_t;

// ======= PUBLIC FUNCTIONS ========

graph_t *graph_init(
    void (*destructor_edge_data)(void *data),
    void (*destructor_vertex_data)(void *data)) {

    graph_t *new_graph = malloc(sizeof(graph_t));
    assert(new_graph != NULL);

    new_graph -> vertices = exhash_init(sizeof(vertex_t *), 512);
    new_graph -> total_vertices = 0;
    new_graph -> destructor_edge_data = destructor_edge_data;
    new_graph -> destructor_vertex_data = destructor_vertex_data;

    return new_graph;
}

bool graph_add_vertex(graph_t *g, void *data, const char *id) {
    if (exhash_search(g -> vertices, id, NULL)) {
        fprintf(stderr, "Vértice já existente no grafo. (grafo.c:%d)", __LINE__);
        return false;
    }

    vertex_t *new_vertex = malloc(sizeof(vertex_t));
    assert(new_vertex != NULL);

    new_vertex -> id = malloc(strlen(id) + 1);
    strcpy(new_vertex -> id, id);

    new_vertex -> data = data;
    new_vertex -> adjacent = list_init();

    exhash_insert(g -> vertices, &new_vertex, id);

    return true;
}

static int compare_target_id(void *target_edge, void *edge) {
    edge_t *e = (edge_t *)edge;
    char *wanted_id = (char *)target_edge;
    return strcmp(e -> target_id, wanted_id);
}

bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id, char *edge_id) {
    assert(g != NULL && data != NULL && src_id != NULL && target_id != NULL);

    vertex_t *src_v = NULL;
    exhash_search(g -> vertices, src_id, &src_v);

    vertex_t *target_v = NULL;
    exhash_search(g -> vertices, target_id, &target_v);

    if (!src_v || !target_v) return false;

    if (list_search(src_v -> adjacent, ((void *)target_id), compare_target_id)) {
        return false;
    }

    edge_t *new_edge = malloc(sizeof(edge_t));
    assert(new_edge != NULL);

    new_edge -> target_id = malloc(strlen(target_id) + 1);
    strncpy(new_edge -> target_id, target_id, strlen(target_id) + 1);

    new_edge -> id = my_strdup(edge_id);
    new_edge -> data = data;

    list_push_back(src_v -> adjacent, new_edge);

    return true;
}

bool graph_is_adjacent(graph_t *g, const char *u_id, const char *v_id) {
    assert(g != NULL && u_id != NULL && v_id != NULL);

    vertex_t *u = NULL;
    exhash_search(g -> vertices, u_id, &u);

    vertex_t *v = NULL;
    exhash_search(g -> vertices, v_id, &v);

    if (!u || !v) return false;

    if (list_search(u -> adjacent, ((void *)v_id), compare_target_id)) {
        return true;
    }

    return false;
}

vertex_t *graph_get_vertex(graph_t *g, const char *vertex_id) {
    assert(g != NULL && vertex_id != NULL);

    vertex_t *wanted_vertex = NULL;

    if (!exhash_search(g -> vertices, vertex_id, &wanted_vertex)) {
        fprintf(stderr, "Vértice de ID %s não encontrado. (grafo.c:%d)\n", vertex_id, __LINE__);
        return NULL;
    }

    return wanted_vertex;
}

edge_t *graph_get_edge(graph_t *g, const char *src_id, const char *target_id) {
    assert(g != NULL);

    vertex_t *src_v = NULL;
    exhash_search(g -> vertices, src_id, &src_v);

    if (!src_v) return NULL;

    return list_search(src_v -> adjacent, (void *)target_id, compare_target_id);
}

bool graph_remove_edge(graph_t *g, const char *src_id, const char *target_id) {
    assert(g != NULL);

    vertex_t *src_v = NULL;
    exhash_search(g -> vertices, src_id, &src_v);

    if (!src_v) return false;

    edge_t *e = list_remove_first(src_v -> adjacent, (void *)target_id, compare_target_id);
    if (!e) return false;

    free(e -> target_id);
    free(e -> id);

    if (g -> destructor_edge_data && e -> data) {
        g -> destructor_edge_data(e -> data);
    }

    free(e);
    return true;
}

bool graph_remove_vertex(graph_t *g, const char *vertex_id) {
    assert(g != NULL);

    vertex_t *v = graph_get_vertex(g, vertex_id);
    if (!v) return false;

    remove_edge_ctx_t context = { vertex_id, g -> destructor_edge_data };
    exhash_foreach(g -> vertices, remove_incoming_edges_cb, &context);

    list_free(v -> adjacent, g -> destructor_edge_data);

    free(v -> id);
    if (g -> destructor_vertex_data && v -> data) {
        g -> destructor_vertex_data(v -> data);
    }

    free(v);

    void *removed = exhash_remove(g -> vertices, vertex_id);
    free(removed);

    return true;
}

list_t *graph_get_neighbors(graph_t *g, const char *vertex_id) {
    assert(g != NULL);

    vertex_t *v = graph_get_vertex(g, vertex_id);
    if (!v) return NULL;

    return v -> adjacent;
}

void graph_destroy(graph_t *g) {
    assert(g != NULL);

    exhash_foreach(g -> vertices, destroy_graph_internal_cb, g);
    exhash_destroy(g -> vertices, NULL);
    free(g);
}

int graph_get_total_vertices(graph_t *g) {
    return g -> total_vertices;
}

void graph_set_total_vertices(graph_t *g, int new_total_vertices) {
    g -> total_vertices = new_total_vertices;
}

exhash_t *graph_get_vertices_map(graph_t *g) {
    return g -> vertices;
}

typedef struct {
    void (*user_callback)(const char *, void *, list_t *, void *);
    void *user_ctx;
} internal_ctx_t;

static void vertex_foreach_wrapper_cb(void *record_data, void *context) {
    vertex_t *v = *(vertex_t **)record_data;
    internal_ctx_t *ctx = context;

    if (ctx -> user_callback) {
        ctx -> user_callback(v -> id, v -> data, v -> adjacent, ctx -> user_ctx);
    }
}

void graph_foreach_vertex(graph_t *g, void (*callback)(const char *id, void *vertex_data, list_t *adjacent, void *context), void *context) {
    assert(g && g -> vertices && callback);

    internal_ctx_t bridge = {callback, context};
    exhash_foreach(g -> vertices, vertex_foreach_wrapper_cb, &bridge);
}

void *vertex_get_data(vertex_t *v) {
    assert(v);
    return v -> data;
}

const char *edge_get_target_id(edge_t *e) {
    assert(e);
    return e -> target_id;
}

void *edge_get_data(edge_t *e) {
    assert(e);
    return e -> data;
}

// ======= STATIC IMPLEMENTATIONS ========

static void destroy_graph_internal_cb(void *record_data, void *context) {
    vertex_t *v = *(vertex_t **)record_data;
    graph_t *g = (graph_t *)context;

    for (list_node_t *node = list_node_front(v -> adjacent); node != NULL; node = list_node_next(node)) {
        edge_t *e = list_node_data(node);
        free(e -> target_id);
        free(e -> id);

        if (g -> destructor_edge_data && e -> data) {
            g -> destructor_edge_data(e -> data);
        }
        free(e);
    }
    list_free(v -> adjacent, NULL);

    if (g -> destructor_vertex_data && v -> data) {
        g -> destructor_vertex_data(v -> data);
    }
    free(v -> id);
    free(v);
}

static void remove_incoming_edges_cb(void *record_data, void *context) {
    vertex_t *v = *(vertex_t **)record_data;
    remove_edge_ctx_t *ctx = (remove_edge_ctx_t *)context;

    edge_t *removed_edge = list_remove_first(v -> adjacent, (void *)ctx -> target_id, compare_target_id);

    if (removed_edge) {
        free(removed_edge -> target_id);
        free(removed_edge -> id);

        if (ctx -> destructor_edge_data && removed_edge -> data) {
            ctx -> destructor_edge_data(removed_edge -> data);
        }
        free(removed_edge);
    }
}

static void free_parent_string_cb(void *record_data, void *context) {
    (void)context;
    char *str = *(char **)record_data;
    free(str);
}

static void destroy_parents_map(exhash_t *parents) {
    exhash_foreach(parents, free_parent_string_cb, NULL);
    exhash_destroy(parents, NULL);
}

list_t *dijkstra(graph_t *graph, bool use_time, char *src_id, char *dst_id, double *out_cost) {
    assert(graph != NULL);

    dijkstra_ctx_t ctx = {
        .graph = graph,
        .costs = exhash_init(sizeof(double), EXHASH_BUCKET_BYTES(sizeof(double), 8)),
        .parents = exhash_init(sizeof(char *), EXHASH_BUCKET_BYTES(sizeof(char *), 8)),
        .min_heap = pq_init(16),
        .use_time = use_time
    };

    double initial_cost = 0.0;
    exhash_insert(ctx.costs, &initial_cost, src_id);
    pq_enqueue(ctx.min_heap, src_id, 0);

    while (!pq_is_empty(ctx.min_heap)) {
        char *curr_id = pq_dequeue(ctx.min_heap);

        if (strcmp(curr_id, dst_id) == 0) {
            free(curr_id);
            break;
        }

        double current_cost = 0.0;
        exhash_search(ctx.costs, curr_id, &current_cost);

        relax_edges(curr_id, current_cost, &ctx);

        free(curr_id);
    }

    list_t *path = list_init();

    if (exhash_search(ctx.costs, dst_id, NULL)) {
        list_free(path, NULL);
        path = reconstruct_path(src_id, dst_id, ctx.parents);

        if (out_cost != NULL) {
            exhash_search(ctx.costs, dst_id, out_cost);
        }
    }

    pq_destroy(ctx.min_heap);
    exhash_destroy(ctx.costs, NULL);
    destroy_parents_map(ctx.parents);

    return path;
}

static void relax_edges(const char *u_id, double current_cost, dijkstra_ctx_t *ctx) {
    list_t *neighbors = graph_get_neighbors(ctx -> graph, u_id);

    for (list_node_t *node = list_node_front(neighbors); node != NULL; node = list_node_next(node)) {
        edge_t *edge = list_node_data(node);
        rua_t *street = edge_get_data(edge);
        const char *v_id = edge_get_target_id(edge);

        double weight = ctx -> use_time ? rua_get_comprimento(street) / rua_get_velocidade_media(street) : rua_get_comprimento(street);
        double new_cost = current_cost + weight;

        double v_cost = INFINITY;
        bool found_v = exhash_search(ctx -> costs, v_id, &v_cost);

        if (new_cost < v_cost) {
            if (found_v) {
                exhash_update(ctx -> costs, v_id, &new_cost);
            } else {
                exhash_insert(ctx -> costs, &new_cost, v_id);
            }

            void *old_parent = exhash_remove(ctx -> parents, v_id);
            if (old_parent != NULL) {
                free(*(char **)old_parent);
                free(old_parent);
            }

            char *u_id_copy = my_strdup(u_id);
            exhash_insert(ctx -> parents, &u_id_copy, v_id);
            pq_enqueue(ctx -> min_heap, v_id, new_cost);
        }
    }
}

static list_t *reconstruct_path(const char *src_id, const char *dst_id, exhash_t *parents) {
    list_t *path = list_init();
    const char *current = dst_id;

    while (current != NULL) {
        list_push_front(path, my_strdup(current));

        if (strcmp(current, src_id) == 0) {
            break;
        }

        const char *next = NULL;
        if (!exhash_search(parents, current, &next)) break;
        current = next;
    }

    return path;
}