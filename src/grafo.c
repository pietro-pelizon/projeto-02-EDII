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

// ======= DECLARAÇÕES STATIC ========

static void remove_neighbors_edges(void *record_data, void *context);
static void graph_destroy_internal(void *record_data, void *context);


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
    list_t *adjacent;

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
    int total_vertices;
    void (*destructor_edge_data)(void *data);
    void (*destructor_vertex_data)(void *data);
}graph_t;

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

    // Checa se vértice com determinado ID já existe.
    // Caso exista, a função retorna false
    if (exhash_search(g -> vertices, id, NULL)) {
        fprintf(stderr, "Vértice já existente no grafo. (grafo.c:%d)", __LINE__);
        return false;
    }

    // Aloca memória para o novo vértice
    vertex_t *new_vertex = malloc(sizeof(vertex_t));
    assert(new_vertex != NULL);

    // Inicializando campos do vértice
    new_vertex -> id = malloc(strlen(id) + 1);
    strcpy(new_vertex -> id, id);

    new_vertex -> data = data;
    new_vertex -> adjacent = list_init();

    // Adiciona um ponteiro do novo vértice ao hashmap do grafo
    exhash_insert(g -> vertices, &new_vertex, id);

    return true;
}

// Função de comparação do destino da aresta, para checar duplicações
// ao adicionar novas arestas ao grafo
static int cmp_target_edge(void *target_edge, void *edge) {
    edge_t *e = (edge_t*) edge;
    char *wanted_id = (char *)target_edge;

    return strcmp(e -> target_id, wanted_id);
}


bool graph_add_edge(graph_t *g, void *data, const char *src_id, const char *target_id, char *street_name) {
    assert(g != NULL && data != NULL && src_id != NULL && target_id != NULL);

    vertex_t *src_v = NULL;
    exhash_search(g -> vertices, src_id, &src_v);

    vertex_t *target_v = NULL;
    exhash_search(g -> vertices, target_id, &target_v);


    // Se um dos vértices não existir, não é possível
    // criar a aresta, pois ela já existe
    if (!src_v || !target_v) return false;

    // Checa se já existe uma aresta idêntica, caso exista, não cria nova aresta
    if (list_search(src_v -> adjacent, ((void *)target_id), cmp_target_edge)) {
        return false;
    }

    // Cria nova aresta DIRECIONADA e preenche suas informações
    edge_t *new_edge = malloc (sizeof(edge_t));
    assert(new_edge != NULL);

    new_edge -> target_id = malloc (strlen(target_id) + 1);
    strncpy(new_edge -> target_id, target_id, strlen(target_id) + 1);

    new_edge -> id = my_strdup(street_name);
    new_edge -> data = data;

    // Nova aresta parte de src_v para target_v
    list_push_back(src_v -> adjacent, new_edge);

    return true;
}

bool is_adjacente(graph_t *g, const char *id_v, const char *id_u) {
    assert(g != NULL && id_v != NULL && id_u != NULL);

    vertex_t *v = NULL;
    exhash_search(g -> vertices, id_v, &v);

    vertex_t *u = NULL;
    exhash_search(g -> vertices, id_u, &u);

    // Caso um dos dois vértices, impossível checar adjacência
    // então assumimos que é false
    if (!v || !u) return false;

    // Procura na lista de adjacência de V se existe o vértice U
    if (list_search(v -> adjacent, ((void *)id_u), cmp_target_edge)) {
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

    // Busca a aresta na lista dele usando a função de procurar na lista
    return list_search(src_v -> adjacent, (void *)target_id, cmp_target_edge);
}


bool graph_remove_edge(graph_t *g, const char *src_id, const char *target_id) {
    assert(g != NULL);

    vertex_t *src_v = NULL;
    exhash_search(g -> vertices, src_id, &src_v);
    if (!src_v) return false;

    edge_t *e = list_remove_first(src_v -> adjacent, (void *)target_id, cmp_target_edge);
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

    // Acha o vértice desejado
    vertex_t *v = graph_get_vertex(g, vertex_id);
    if (!v) return false;

    // Remove cada aresta associada ao vértice a ser destruído
    ctx_remove_t context = { vertex_id, g -> destructor_edge_data };
    exhash_foreach(g -> vertices, remove_neighbors_edges, &context);

    // Itera pela sua lista de adjacência destruindo as arestas
    list_free(v -> adjacent, g -> destructor_edge_data);

    // Remove id e dados
    free(v -> id);
    if (g -> destructor_vertex_data && v -> data) {
        g -> destructor_vertex_data(v -> data);
    }

    // Remove o vértice em si
    free(v);

    // Remove o vértice do hashmap geral do grafo
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
    assert (g != NULL);

    // Itera e destrói todos os vértices e suas respectivas listas de adjacência
    exhash_foreach(g -> vertices, graph_destroy_internal, g);

    // Destrói a estrutura raiz do Hash Map (Diretório e Baldes)
    exhash_destroy(g -> vertices, NULL);

    // Libera a raiz do grafo
    free(g);

}

int graph_get_total_vertices(graph_t *g) {
    return g -> total_vertices;
}

void graph_set_total_vertices(graph_t *g, int new_total_vertices) {
    g -> total_vertices = new_total_vertices;
}

exhash_t *graph_get_exhash(graph_t *g) {
    return g -> vertices;
}

// Struct para poder passar as informações para o graph_foreach_vertex
typedef struct {
    void (*user_callback)(const char *, void *, list_t *, void *);
    void *user_ctx;
} internal_ctx_t;

// Wrapper que será declarado só aqui nesse módulo
// para encapsular a lógica da função pública
static void internal_foreach_wrapper(void *record_data, void *context) {
    vertex_t *v = *(vertex_t **)record_data;
    internal_ctx_t *ctx = context;

    if (ctx -> user_callback) {
        ctx -> user_callback(v -> id, v -> data, v -> adjacent, ctx -> user_ctx);
    }
}


// Função pública para iterar sobre os vértices sem quebrar a opacidade
void graph_foreach_vertex(graph_t *g, void (*callback)(const char *id, void *vertex_data, list_t *adjacent, void *context), void *context) {
    assert(g && g -> vertices && callback);

    // Ponte para os outros módulos
    internal_ctx_t bridge = {callback, context};

    exhash_foreach(g -> vertices, internal_foreach_wrapper, &bridge);
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

// ======= IMPLEMENTAÇÕES STATIC ========

static void graph_destroy_internal(void *record_data, void *context) {
    vertex_t *v = *(vertex_t **)record_data;
    graph_t *g = (graph_t *)context;

    for (list_node_t *no = list_node_front(v -> adjacent); no != NULL; no = list_node_next(no)) {
        edge_t *e = list_node_data(no);
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

// O callback que será chamado para CADA vértice do grafo
static void remove_neighbors_edges(void *record_data, void *context) {
    // Como o exhash guarda ponteiros, record_data é um (vertex_t **)
    vertex_t *v = *(vertex_t **)record_data;
    ctx_remove_t *ctx = (ctx_remove_t *)context;

    // Tenta remover a aresta que aponta para o vértice que está morrendo
    edge_t *aresta_removida = list_remove_first(v -> adjacent, (void *)ctx -> target_id, cmp_target_edge);

    if (aresta_removida) {
        free(aresta_removida -> target_id);
        free(aresta_removida -> id);
        if (ctx -> destructor_edge_data && aresta_removida -> data) {
            ctx -> destructor_edge_data(aresta_removida -> data);
        }
        free(aresta_removida);
    }
}

static void libera_string_do_pais(void *record_data, void *context) {
    (void)context;
    // record_data aponta para dentro do bucket onde está guardado o char*
    char *str = *(char **)record_data;
    free(str);
}

static void libera_pais(exhash_t *pais) {
    exhash_foreach(pais, libera_string_do_pais, NULL);
    exhash_destroy(pais, NULL);
}

list_t *dijkstra(graph_t *g, bool flag_tempo, char *id_src, char *id_dst, double *custo_out) {
    assert(g != NULL);

    // Inicializando as estruturas necessárias para o algoritmo
    exhash_t *custos = exhash_init(sizeof(double), EXHASH_BUCKET_BYTES(sizeof(double), 8));
    assert(custos != NULL);
    exhash_t *pais = exhash_init(sizeof(char *), EXHASH_BUCKET_BYTES(sizeof(char *), 8));
    assert(pais != NULL);
    pqueue_t *min_heap = pq_init(16);
    assert(min_heap != NULL);


    double custo_zero = 0.0;
    exhash_insert(custos, &custo_zero, id_src);
    pq_enqueue(min_heap, id_src, 0);


    // Loop principal
    while (!pq_is_empty(min_heap)) {
        char *id_esquina = pq_dequeue(min_heap);

        // Se o id retirado é o id do destino
        // temos o melhor caminho
        if (strcmp(id_esquina, id_dst) == 0) {
            free(id_esquina);
            break;
        }

        // Temos que olhar para cada vértice adjacente a esse
        // e checar se o caminho é melhor
        list_t *vizinhos = graph_get_neighbors(g, id_esquina);

        double custo_atual = 0.0;
        double custo_lido = 0.0;

        // Buscando o custo acumulado para chegar no vértice atual
        bool found_esquina = exhash_search(custos, id_esquina, &custo_lido);

        // Se achou, esse será o custo atual
        custo_atual = found_esquina ? custo_lido : custo_atual;

        // Loop para olhar todos os vértices adjacentes
        for (list_node_t *no = list_node_front(vizinhos); no != NULL; no = list_node_next(no)) {

            // Obtendo o payload (dado interno) da aresta
            edge_t *aresta = list_node_data(no);
            rua_t *rua = edge_get_data(aresta);

            // Temos que olhar para seu vizinho e descobrir
            // qual o custo para chegar nele
            const char *id_vizinho = edge_get_target_id(aresta);

            // Se a flag tempo for true, o peso será o tempo mínimo do trajeto
            // se não, o peso será o comprimento
            double peso = flag_tempo ? rua_get_comprimento(rua)/rua_get_velocidade_media(rua) : rua_get_comprimento(rua);

            // Cálculo do novo custo
            double novo_custo = custo_atual + peso;

            // Inicializa o custo para ir ao vizinho com infinito
            double custo_vizinho = INFINITY;

            double custo_vizinho_lido = 0.0;
            bool found_vizinho = exhash_search(custos, id_vizinho, &custo_vizinho_lido);

            custo_vizinho = found_vizinho ? custo_vizinho_lido : custo_vizinho;

            // Teste principal: O custo para chegar a esse novo vizinho
            // é menor do que o menor custo conhecido até agora?

            if (novo_custo < custo_vizinho) {
                if (found_vizinho)
                    exhash_update(custos, id_vizinho, &novo_custo);
                else {
                    exhash_insert(custos, &novo_custo, id_vizinho);
                }
                
                void *old = exhash_remove(pais, id_vizinho);
                if (old != NULL) {
                    char *old_str = *(char **)old;
                    free(old_str);
                    free(old);
                }

                char *copia_id_esquina = my_strdup(id_esquina);
                exhash_insert(pais, &copia_id_esquina, id_vizinho);
                pq_enqueue(min_heap, id_vizinho, novo_custo);
            }
        }
        free(id_esquina);
    }

    list_t *caminho = list_init();

    // Teste para ver se foi possível alcançar o destino
    if (exhash_search(custos, id_dst, NULL) == false) {
        pq_destroy(min_heap);
        exhash_destroy(custos, NULL);
        libera_pais(pais);
        return caminho;
    }

    // Backtracking
    const char *atual = id_dst;
    while (atual != NULL) {
        list_push_front(caminho, my_strdup(atual));

        // O backtracking se conclui ao chegarmos na origem
        if (strcmp(atual, id_src) == 0) {
            break;
        }
        const char *prox = NULL;
        if (!exhash_search(pais, atual, &prox)) break;
        atual = prox;
    }

    double custo_final = 0.0;
    exhash_search(custos, id_dst, &custo_final);
    if (custo_out != NULL) *custo_out = custo_final;

    pq_destroy(min_heap);
    exhash_destroy(custos, NULL);
    libera_pais(pais);

    return caminho;

}




