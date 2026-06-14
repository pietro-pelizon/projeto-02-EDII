#include "../unity/unity.h"
#include "../include/grafo.h"
#include "../include/lista.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Payloads minimos para os testes
typedef struct { int valor; } dado_v_t;
typedef struct { double peso; } dado_e_t;

static void destrutor_v(void *data) { free(data); }
static void destrutor_e(void *data) { free(data); }

static graph_t *novo_grafo(void) {
    return graph_init(NULL, destrutor_e, destrutor_v);
}

static dado_v_t *payload_v(int valor) {
    dado_v_t *d = malloc(sizeof(dado_v_t));
    d -> valor = valor;
    return d;
}

static dado_e_t *payload_e(double peso) {
    dado_e_t *d = malloc(sizeof(dado_e_t));
    d -> peso = peso;
    return d;
}

// graph_init deve retornar um ponteiro válido (não NULL)
void init_retorna_ponteiro_valido(void) {
    graph_t *g = novo_grafo();
    TEST_ASSERT_NOT_NULL(g);
    graph_destroy(g);
}

// Inserção simples de um vértice deve retornar true
void add_vertex_simples(void) {
    graph_t *g = novo_grafo();

    bool ok = graph_add_vertex(g, payload_v(1), "v1");
    TEST_ASSERT_TRUE(ok);

    graph_destroy(g);
}

// Inserir um vértice com ID já existente deve retornar false
void add_vertex_duplicado_retorna_false(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    bool ok = graph_add_vertex(g, payload_v(2), "v1");
    TEST_ASSERT_FALSE(ok);

    graph_destroy(g);
}

// Vértice inserido deve ser encontrado pelo seu ID, com dado interno correto
void get_vertex_encontra_inserido(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(42), "v1");
    vertex_t *v = graph_get_vertex(g, "v1");
    TEST_ASSERT_NOT_NULL(v);

    dado_v_t *d = vertex_get_data(v);
    TEST_ASSERT_EQUAL_INT(42, d -> valor);

    graph_destroy(g);
}

// Buscar um ID que nunca foi inserido deve retornar NULL
void get_vertex_inexistente_retorna_null(void) {
    graph_t *g = novo_grafo();

    vertex_t *v = graph_get_vertex(g, "fantasma");
    TEST_ASSERT_NULL(v);

    graph_destroy(g);
}

// Inserção simples de uma aresta entre dois vértices existentes deve retornar true
void add_edge_simples(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");

    bool ok = graph_add_edge(g, payload_e(5.0), "v1", "v2", "Rua_A");
    TEST_ASSERT_TRUE(ok);

    graph_destroy(g);
}

// Inserir uma aresta já existente deve retornar false
void add_edge_duplicada_retorna_false(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");

    graph_add_edge(g, payload_e(5.0), "v1", "v2", "Rua_A");
    bool ok = graph_add_edge(g, payload_e(5.0), "v1", "v2", "Rua_A");
    TEST_ASSERT_FALSE(ok);

    graph_destroy(g);
}

// Inserir aresta cujo vértice de destino não existe deve retornar false
void add_edge_vertice_inexistente_retorna_false(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");

    bool ok = graph_add_edge(g, payload_e(5.0), "v1", "fantasma", "Rua_A");
    TEST_ASSERT_FALSE(ok);

    graph_destroy(g);
}

// Aresta inserida deve ser encontrada, com dado interno e ID de destino corretos
void get_edge_encontra_aresta_inserida(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_edge(g, payload_e(7.5), "v1", "v2", "Rua_B");

    edge_t *e = graph_get_edge(g, "v1", "v2");
    TEST_ASSERT_NOT_NULL(e);

    dado_e_t *d = edge_get_data(e);
    TEST_ASSERT_EQUAL_DOUBLE(7.5, d -> peso);

    TEST_ASSERT_EQUAL_STRING("v2", edge_get_target_id(e));

    graph_destroy(g);
}

// Buscar uma aresta que não foi inserida deve retornar NULL
void get_edge_inexistente_retorna_null(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");

    edge_t *e = graph_get_edge(g, "v1", "v2");
    TEST_ASSERT_NULL(e);

    graph_destroy(g);
}

// is_adjacente deve retornar true quando a aresta v1->v2 existe
void is_adjacente_retorna_true_para_aresta_existente(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_edge(g, payload_e(1.0), "v1", "v2", "Rua_C");

    TEST_ASSERT_TRUE(is_adjacente(g, "v1", "v2"));

    graph_destroy(g);
}

// O grafo é dirigido: v1->v2 não implica v2->v1
void is_adjacente_nao_e_simetrico(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_edge(g, payload_e(1.0), "v1", "v2", "Rua_C");

    // Aresta v1 -> v2, mas NAO v2 -> v1
    TEST_ASSERT_FALSE(is_adjacente(g, "v2", "v1"));

    graph_destroy(g);
}

// is_adjacente deve retornar false quando nenhuma aresta foi inserida
void is_adjacente_retorna_false_sem_aresta(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");

    TEST_ASSERT_FALSE(is_adjacente(g, "v1", "v2"));

    graph_destroy(g);
}

// Vértice com arestas de saída deve retornar lista de vizinhos não nula
void get_neighbors_retorna_lista_com_vizinhos(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_vertex(g, payload_v(3), "v3");
    graph_add_edge(g, payload_e(1.0), "v1", "v2", "Rua_A");
    graph_add_edge(g, payload_e(2.0), "v1", "v3", "Rua_B");

    lista_t *viz = graph_get_neighbors(g, "v1");
    TEST_ASSERT_NOT_NULL(viz);

    graph_destroy(g);
}

// Vértice sem arestas de saída deve retornar lista vazia (não NULL)
void get_neighbors_vertice_sem_arestas_retorna_lista_vazia(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");

    lista_t *viz = graph_get_neighbors(g, "v1");
    TEST_ASSERT_NOT_NULL(viz);
    TEST_ASSERT_TRUE(is_empty_lista(viz));

    graph_destroy(g);
}

// Pedir vizinhos de um vértice inexistente deve retornar NULL
void get_neighbors_vertice_inexistente_retorna_null(void) {
    graph_t *g = novo_grafo();

    lista_t *viz = graph_get_neighbors(g, "fantasma");
    TEST_ASSERT_NULL(viz);

    graph_destroy(g);
}

// Após remover a aresta, ela não deve ser encontrada nem indicar adjacência
void remove_edge_impede_busca_posterior(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_edge(g, payload_e(1.0), "v1", "v2", "Rua_A");

    TEST_ASSERT_TRUE(graph_remove_edge(g, "v1", "v2"));
    TEST_ASSERT_NULL(graph_get_edge(g, "v1", "v2"));
    TEST_ASSERT_FALSE(is_adjacente(g, "v1", "v2"));

    graph_destroy(g);
}

// Tentar remover aresta inexistente deve retornar false sem quebrar
void remove_edge_inexistente_retorna_false(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");

    TEST_ASSERT_FALSE(graph_remove_edge(g, "v1", "v2"));

    graph_destroy(g);
}

// Após remover o vértice, ele não deve ser encontrado no grafo
void remove_vertex_impede_busca_posterior(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    TEST_ASSERT_TRUE(graph_remove_vertex(g, "v1"));
    TEST_ASSERT_NULL(graph_get_vertex(g, "v1"));

    graph_destroy(g);
}

// Tentar remover vértice inexistente deve retornar false sem quebrar
void remove_vertex_inexistente_retorna_false(void) {
    graph_t *g = novo_grafo();

    TEST_ASSERT_FALSE(graph_remove_vertex(g, "fantasma"));

    graph_destroy(g);
}

// Remover um vértice deve limpar automaticamente as arestas de outros vértices que apontavam para ele
void remove_vertex_limpa_arestas_dos_vizinhos(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_vertex(g, payload_v(3), "v3");

    // v2 -> v1 e v3 -> v1
    graph_add_edge(g, payload_e(1.0), "v2", "v1", "Rua_A");
    graph_add_edge(g, payload_e(2.0), "v3", "v1", "Rua_B");

    // Remove v1: as arestas de v2 e v3 que apontavam para ele
    // devem sumir automaticamente
    graph_remove_vertex(g, "v1");

    TEST_ASSERT_FALSE(is_adjacente(g, "v2", "v1"));
    TEST_ASSERT_FALSE(is_adjacente(g, "v3", "v1"));

    graph_destroy(g);
}

// Remover um vértice não deve afetar vértices e arestas não relacionados a ele
void remove_vertex_nao_corrompe_outros_vertices(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_vertex(g, payload_v(3), "v3");
    graph_add_edge(g, payload_e(1.0), "v1", "v3", "Rua_A");

    graph_remove_vertex(g, "v2");

    TEST_ASSERT_NOT_NULL(graph_get_vertex(g, "v1"));
    TEST_ASSERT_NOT_NULL(graph_get_vertex(g, "v3"));
    TEST_ASSERT_TRUE(is_adjacente(g, "v1", "v3"));

    graph_destroy(g);
}

// graph_set_nv e graph_get_nv devem ser consistentes
void set_get_nv(void) {
    graph_t *g = novo_grafo();

    graph_set_total_vertices(g, 10);
    TEST_ASSERT_EQUAL_INT(10, graph_get_total_vertices(g));

    graph_destroy(g);
}

// graph_get_exhash deve retornar o hashmap interno (não NULL)
void get_exhash_retorna_nao_null(void) {
    graph_t *g = novo_grafo();

    TEST_ASSERT_NOT_NULL(graph_get_exhash(g));

    graph_destroy(g);
}

// Conta quantos vértices o foreach visita
static void conta_vertices(const char *id, void *data, lista_t *adj, void *ctx) {
    (void)id; (void)data; (void)adj;
    int *contador = ctx;
    (*contador)++;
}

// graph_foreach_vertex deve visitar exatamente todos os vértices inseridos
void foreach_visita_todos_os_vertices(void) {
    graph_t *g = novo_grafo();

    graph_add_vertex(g, payload_v(1), "v1");
    graph_add_vertex(g, payload_v(2), "v2");
    graph_add_vertex(g, payload_v(3), "v3");

    int contador = 0;
    graph_foreach_vertex(g, conta_vertices, &contador);
    TEST_ASSERT_EQUAL_INT(3, contador);

    graph_destroy(g);
}

// graph_foreach_vertex em grafo vazio não deve chamar o callback nenhuma vez
void foreach_em_grafo_vazio_nao_chama_callback(void) {
    graph_t *g = novo_grafo();

    int contador = 0;
    graph_foreach_vertex(g, conta_vertices, &contador);
    TEST_ASSERT_EQUAL_INT(0, contador);

    graph_destroy(g);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(init_retorna_ponteiro_valido);

    RUN_TEST(add_vertex_simples);
    RUN_TEST(add_vertex_duplicado_retorna_false);
    RUN_TEST(get_vertex_encontra_inserido);
    RUN_TEST(get_vertex_inexistente_retorna_null);

    RUN_TEST(add_edge_simples);
    RUN_TEST(add_edge_duplicada_retorna_false);
    RUN_TEST(add_edge_vertice_inexistente_retorna_false);
    RUN_TEST(get_edge_encontra_aresta_inserida);
    RUN_TEST(get_edge_inexistente_retorna_null);

    RUN_TEST(is_adjacente_retorna_true_para_aresta_existente);
    RUN_TEST(is_adjacente_nao_e_simetrico);
    RUN_TEST(is_adjacente_retorna_false_sem_aresta);

    RUN_TEST(get_neighbors_retorna_lista_com_vizinhos);
    RUN_TEST(get_neighbors_vertice_sem_arestas_retorna_lista_vazia);
    RUN_TEST(get_neighbors_vertice_inexistente_retorna_null);

    RUN_TEST(remove_edge_impede_busca_posterior);
    RUN_TEST(remove_edge_inexistente_retorna_false);

    RUN_TEST(remove_vertex_impede_busca_posterior);
    RUN_TEST(remove_vertex_inexistente_retorna_false);
    RUN_TEST(remove_vertex_limpa_arestas_dos_vizinhos);
    RUN_TEST(remove_vertex_nao_corrompe_outros_vertices);

    RUN_TEST(set_get_nv);
    RUN_TEST(get_exhash_retorna_nao_null);

    RUN_TEST(foreach_visita_todos_os_vertices);
    RUN_TEST(foreach_em_grafo_vazio_nao_chama_callback);

    return UNITY_END();
}