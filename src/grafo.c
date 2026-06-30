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

typedef struct stTarjanCtx {
    int *discovery_time;
    int *low_link;
    bool *is_on_stack;
    list_t *stack;
    int timer;
    int num_sccs;

    exhash_t *id_to_index;
    graph_t *g;

    list_t *scc_list;
    edge_filter_fn filter;
    void *filter_ctx;
} tarjan_ctx_t;

typedef struct {
    tarjan_ctx_t *tarjan;
    int current_index;
} tarjan_setup_ctx_t;

// ======= STATIC DECLARATIONS ========

static void remove_incoming_edges_cb(void *record_data, void *context);
static void destroy_graph_internal_cb(void *record_data, void *context);
static void relax_edges(const char *u_id, double current_cost, dijkstra_ctx_t *ctx);
static list_t *reconstruct_path(const char *src_id, const char *dst_id, exhash_t *parents);
static int compare_target_id(void *target_edge, void *edge);

static tarjan_ctx_t *tarjan_ctx_create(graph_t *g, edge_filter_fn filter, void *filter_ctx);
static void tarjan_ctx_destroy(tarjan_ctx_t *ctx);
static void map_vertex_tarjan_cb(const char *id, void *v_data, list_t *adj, void *context);
static void tarjan_init_cb(const char *id, void *v_data, list_t *adj, void *context);
static void tarjan_dfs(const char *curr_id, tarjan_ctx_t *ctx);
static void destroy_parents_map(exhash_t *parents);

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

    new_graph -> vertices = exhash_init(sizeof(vertex_t *), EXHASH_BUCKET_BYTES(sizeof(vertex_t *), 8));
    new_graph -> total_vertices = 0;
    new_graph -> destructor_edge_data = destructor_edge_data;
    new_graph -> destructor_vertex_data = destructor_vertex_data;

    return new_graph;
}

bool graph_add_vertex(graph_t *g, void *data, const char *id) {
    if (exhash_search(g -> vertices, id, NULL)) {
        ERRO_LOG("Vértice já existente no grafo.");
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
        ERRO_LOG("Vértice de ID '%s' não encontrado.", vertex_id);
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

// ======= ALGORITHMS ========

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

list_t *tarjan(graph_t *g, edge_filter_fn filter, void *filter_ctx) {

    // Aloca as estruturas
    tarjan_ctx_t *ctx = tarjan_ctx_create(g, filter, filter_ctx);
    tarjan_setup_ctx_t setup = { ctx, 0 };

    // Mapeia os IDs (Strings) para Índices (Inteiros)
    graph_foreach_vertex(g, map_vertex_tarjan_cb, &setup);

    // Executa o DFS para cada vértice não visitado
    graph_foreach_vertex(g, tarjan_init_cb, ctx);

    // Salva o resultado antes de destruir o contexto
    list_t *result_sccs = ctx -> scc_list;

    // Libera a memória utilizada
    tarjan_ctx_destroy(ctx);

    return result_sccs;
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

static tarjan_ctx_t *tarjan_ctx_create(graph_t *g, edge_filter_fn filter, void *filter_ctx) {
    int num_vertices = graph_get_total_vertices(g);
    tarjan_ctx_t *ctx = malloc(sizeof(tarjan_ctx_t));
    assert(ctx != NULL);

    ctx -> discovery_time = malloc(sizeof(int) * num_vertices);
    ctx -> low_link = malloc(sizeof(int) * num_vertices);
    ctx -> is_on_stack = malloc(sizeof(bool) * num_vertices);
    ctx -> stack = list_init();
    ctx -> timer = 0;
    ctx -> num_sccs = 0;
    ctx -> id_to_index = exhash_init(sizeof(int), EXHASH_BUCKET_BYTES(sizeof(int), 8));
    ctx -> g = g;
    ctx -> scc_list = list_init();
    ctx -> filter = filter;
    ctx -> filter_ctx = filter_ctx;

    return ctx;
}

static void tarjan_ctx_destroy(tarjan_ctx_t *ctx) {
    free(ctx -> discovery_time);
    free(ctx -> low_link);
    free(ctx -> is_on_stack);
    list_free(ctx -> stack, free);
    exhash_destroy(ctx -> id_to_index, NULL);
    free(ctx);
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
                pq_enqueue(ctx -> min_heap, v_id, new_cost);

            }

            else {
                exhash_insert(ctx -> costs, &new_cost, v_id);
                pq_change_priority(ctx -> min_heap, v_id, new_cost);
            }

            void *old_parent = exhash_remove(ctx -> parents, v_id);
            if (old_parent != NULL) {
                free(*(char **)old_parent);
                free(old_parent);
            }


            char *u_id_copy = my_strdup(u_id);
            exhash_insert(ctx -> parents, &u_id_copy, v_id);

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

static void map_vertex_tarjan_cb(const char *id, void *v_data, list_t *adj, void *context) {
    (void)adj;
    (void)v_data;

    tarjan_setup_ctx_t *setup = context;
    tarjan_ctx_t *tarjan_ctx = setup -> tarjan;

    int idx = setup -> current_index;

    exhash_insert(tarjan_ctx -> id_to_index, &idx, id);

    tarjan_ctx -> discovery_time[idx] = -1;
    tarjan_ctx -> is_on_stack[idx] = false;

    setup -> current_index++;

}

static void tarjan_init_cb(const char *id, void *v_data, list_t *adj, void *context) {
    (void)v_data;
    (void)adj;
    tarjan_ctx_t *ctx = context;
    int idx = 0;
    exhash_search(ctx -> id_to_index, id, &idx);

    if (ctx -> discovery_time[idx] == -1) {
        tarjan_dfs(id, ctx);
    }
}



static void tarjan_dfs(const char *curr_id, tarjan_ctx_t *ctx) {
    int curr_idx = 0;
    exhash_search(ctx -> id_to_index, curr_id, &curr_idx);

    ctx -> discovery_time[curr_idx] = ctx -> timer;
    ctx -> low_link[curr_idx] = ctx -> timer;
    ctx -> timer++;

    list_push_front(ctx -> stack, my_strdup(curr_id));
    ctx -> is_on_stack[curr_idx] = true;

    list_t *neighbors = graph_get_neighbors(ctx -> g, curr_id);

    for (list_node_t *node = list_node_front(neighbors); node != NULL; node = list_node_next(node)) {
        edge_t *edge = list_node_data(node);
        const char *neighbor_id = edge_get_target_id(edge);
        void *street_data = edge_get_data(edge);



        if (ctx -> filter(street_data, ctx -> filter_ctx)) {
            int neighbor_idx = 0;

            exhash_search(ctx -> id_to_index, neighbor_id, &neighbor_idx);

            if (ctx -> discovery_time[neighbor_idx] == -1) {
                tarjan_dfs(neighbor_id, ctx);

                if (ctx -> low_link[neighbor_idx] < ctx -> low_link[curr_idx]) {
                    ctx -> low_link[curr_idx] = ctx -> low_link[neighbor_idx];
                }

            } else if (ctx -> is_on_stack[neighbor_idx]) {
                if (ctx -> discovery_time[neighbor_idx] < ctx -> low_link[curr_idx]) {
                    ctx -> low_link[curr_idx] = ctx -> discovery_time[neighbor_idx];

                }
            }
        }
    }



    if (ctx -> low_link[curr_idx] == ctx -> discovery_time[curr_idx]) {
        list_t *scc = list_init();
        char *popped_id;



        do {
            popped_id = list_pop_front(ctx -> stack);

            int popped_idx = 0;
            exhash_search(ctx -> id_to_index, popped_id, &popped_idx);

            ctx -> is_on_stack[popped_idx] = false;
            list_push_front(scc, popped_id);

        } while (strcmp(popped_id, curr_id) != 0);


        list_push_back(ctx -> scc_list, scc);
        ctx -> num_sccs++;
    }
}
