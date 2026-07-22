#include "../unity/unity.h"
#include "../include/grafo.h"
#include "../include/lista.h"
#include "../include/rua.h"
#include "../include/ponto.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void destrutor_ponto(void *data) { ponto_destroy((ponto_t *)data); }
static void destrutor_rua(void *data)   { rua_destroy((rua_t *)data); }

static graph_t *novo_grafo(void) {
    return graph_init(destrutor_rua, destrutor_ponto);
}

// Adiciona vértice com coordenadas fictícias
static void add_v(graph_t *g, const char *id) {
    graph_add_vertex(g, ponto_init(0.0, 0.0), id);
}

// Adiciona aresta com comprimento e velocidade média
static void add_e(graph_t *g, const char *src, const char *dst,
                  double cmp, double vm) {
    rua_t *r = rua_init("-", "-", "Rua_Teste", vm, cmp);
    graph_add_edge(g, r, src, dst, "Rua_Teste");
}

// Extrai os IDs do caminho retornado como array de strings.
// Retorna o número de vértices.
static int extrai_caminho(list_t *caminho, char ***out) {
    int n = 0;
    for (list_node_t *no = list_node_front(caminho); no != NULL; no = list_node_next(no))
        n++;

    *out = malloc(n * sizeof(char *));
    int i = 0;
    for (list_node_t *no = list_node_front(caminho); no != NULL; no = list_node_next(no))
        (*out)[i++] = (char *)list_node_data(no);

    return n;
}


void setUp(void)    {}
void tearDown(void) {}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Testes — caminho mais curto (flag_tempo = false)
// ---------------------------------------------------------------------------

// Grafo linear A->B->C: caminho mais curto de A até C passa por B
void curto_caminho_linear(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");
    add_e(g, "A", "B", 10.0, 1.0);
    add_e(g, "B", "C", 10.0, 1.0);

    list_t *cam = dijkstra(g, false, "A", "C", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    TEST_ASSERT_EQUAL_INT(3, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);
    TEST_ASSERT_EQUAL_STRING("B", ids[1]);
    TEST_ASSERT_EQUAL_STRING("C", ids[2]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// Grafo com atalho: A->B->D (20) é mais curto que A->C->D (25)
void curto_escolhe_menor_distancia(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C"); add_v(g, "D");
    add_e(g, "A", "B", 10.0, 1.0);
    add_e(g, "B", "D", 10.0, 1.0);
    add_e(g, "A", "C", 20.0, 1.0);
    add_e(g, "C", "D",  5.0, 1.0);

    list_t *cam = dijkstra(g, false, "A", "D", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    // Caminho correto: A -> B -> D (custo 20)
    TEST_ASSERT_EQUAL_INT(3, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);
    TEST_ASSERT_EQUAL_STRING("B", ids[1]);
    TEST_ASSERT_EQUAL_STRING("D", ids[2]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// Origem e destino são o mesmo vértice: caminho deve conter só ele
void curto_origem_igual_destino(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B");
    add_e(g, "A", "B", 10.0, 1.0);

    list_t *cam = dijkstra(g, false, "A", "A", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    TEST_ASSERT_EQUAL_INT(1, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// Destino inacessível: lista retornada deve ser vazia
void curto_destino_inacessivel_retorna_lista_vazia(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");
    add_e(g, "A", "B", 10.0, 1.0);

    // C existe, mas nenhuma aresta leva até lá

    list_t *cam = dijkstra(g, false, "A", "C", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);
    TEST_ASSERT_EQUAL_INT(0, n);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// Caminho direto vs caminho com desvio: direto deve vencer
void curto_aresta_direta_vence_desvio(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");
    add_e(g, "A", "C",  5.0, 1.0);   // direto, mais curto
    add_e(g, "A", "B", 10.0, 1.0);
    add_e(g, "B", "C",  1.0, 1.0);   // desvio: 10+1=11

    list_t *cam = dijkstra(g, false, "A", "C", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    TEST_ASSERT_EQUAL_INT(2, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);
    TEST_ASSERT_EQUAL_STRING("C", ids[1]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// ---------------------------------------------------------------------------
// Testes — caminho mais rápido (flag_tempo = true, peso = cmp/vm)
// ---------------------------------------------------------------------------

// A->B: cmp=100, vm=10 → tempo=10
// A->C->B: cmp=200+50, vm=1 → tempo=250 — A->B direto é mais rápido
void rapido_menor_tempo_vence(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");
    add_e(g, "A", "B", 100.0, 10.0);  // tempo = 10
    add_e(g, "A", "C", 200.0,  1.0);  // tempo = 200
    add_e(g, "C", "B",  50.0,  1.0);  // tempo = 50 → total = 250

    list_t *cam = dijkstra(g, true, "A", "B", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    TEST_ASSERT_EQUAL_INT(2, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);
    TEST_ASSERT_EQUAL_STRING("B", ids[1]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// Caminho mais longo em distância pode ser mais rápido se vm for maior
//
//  A --1000m, vm=100--> B   (tempo = 10)
//  A --100m,  vm=  1--> C   (tempo = 100)
//  C --100m,  vm=  1--> B   (tempo = 100, total = 200)
//
// Mais rápido: A->B direto (tempo 10), mesmo sendo mais longo em metros
void rapido_longo_mas_veloz_vence(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");
    add_e(g, "A", "B", 1000.0, 100.0);  // tempo = 10
    add_e(g, "A", "C",  100.0,   1.0);  // tempo = 100
    add_e(g, "C", "B",  100.0,   1.0);  // tempo = 100 → total = 200

    list_t *cam = dijkstra(g, true, "A", "B", NULL);

    char **ids; int n = extrai_caminho(cam, &ids);

    TEST_ASSERT_EQUAL_INT(2, n);
    TEST_ASSERT_EQUAL_STRING("A", ids[0]);
    TEST_ASSERT_EQUAL_STRING("B", ids[1]);

    free(ids);
    list_free(cam, free);
    graph_destroy(g);
}

// flag_tempo=false e flag_tempo=true podem retornar caminhos diferentes
// no mesmo grafo quando o caminho mais curto não é o mais rápido
void curto_e_rapido_diferem(void) {
    graph_t *g = novo_grafo();
    add_v(g, "A"); add_v(g, "B"); add_v(g, "C");

    // A->C direto: cmp=5, vm=1 → mais curto (5m), tempo=5
    // A->B->C: cmp=10+10=20, vm=10 → mais longo, tempo=10+10/10=2
    add_e(g, "A", "C",  5.0,  1.0);
    add_e(g, "A", "B", 10.0, 10.0);
    add_e(g, "B", "C", 10.0, 10.0);

    list_t *cam_curto  = dijkstra(g, false, "A", "C", NULL);
    list_t *cam_rapido = dijkstra(g, true,  "A", "C", NULL);

    char **ids_curto;  int n_curto  = extrai_caminho(cam_curto,  &ids_curto);
    char **ids_rapido; int n_rapido = extrai_caminho(cam_rapido, &ids_rapido);

    // Mais curto: A->C (2 vértices)
    TEST_ASSERT_EQUAL_INT(2, n_curto);
    TEST_ASSERT_EQUAL_STRING("A", ids_curto[0]);
    TEST_ASSERT_EQUAL_STRING("C", ids_curto[1]);

    // Mais rápido: A->B->C (3 vértices)
    TEST_ASSERT_EQUAL_INT(3, n_rapido);
    TEST_ASSERT_EQUAL_STRING("A", ids_rapido[0]);
    TEST_ASSERT_EQUAL_STRING("B", ids_rapido[1]);
    TEST_ASSERT_EQUAL_STRING("C", ids_rapido[2]);

    free(ids_curto); free(ids_rapido);
    list_free(cam_curto, free);
    list_free(cam_rapido, free);
    graph_destroy(g);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(curto_caminho_linear);
    RUN_TEST(curto_escolhe_menor_distancia);
    RUN_TEST(curto_origem_igual_destino);
    RUN_TEST(curto_destino_inacessivel_retorna_lista_vazia);
    RUN_TEST(curto_aresta_direta_vence_desvio);

    RUN_TEST(rapido_menor_tempo_vence);
    RUN_TEST(rapido_longo_mas_veloz_vence);
    RUN_TEST(curto_e_rapido_diferem);

    return UNITY_END();
}