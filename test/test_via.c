#include "../unity/unity.h"
#include "../include/via_handler.h"
#include "../include/grafo.h"
#include "../include/rua.h"
#include "../include/ponto.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Destrutor de ponto_t usado como payload de vértice
static void destrutor_ponto(void *data) { ponto_destroy((ponto_t *)data); }

// Destrutor de rua_t usado como payload de aresta
static void destrutor_rua(void *data) { rua_destroy((rua_t *)data); }

static graph_t *novo_grafo(void) {
    return graph_init(NULL, destrutor_rua, destrutor_ponto);
}

// Cria um arquivo .via temporário com o conteúdo fornecido e retorna o path.
static char *criar_via_temp(const char *conteudo) {
    char *path = malloc(L_tmpnam);
    tmpnam(path);
    FILE *f = fopen(path, "w");
    fputs(conteudo, f);
    fclose(f);
    return path;
}


void setUp(void)    {}
void tearDown(void) {}


// Arquivo com apenas vértices, sem arestas
void carrega_vertices_simples(void) {
    // Formato: <nv>\nv <id> <x> <y>\n...
    const char *conteudo =
        "3\n"
        "v cruzA 10.0 20.0\n"
        "v cruzB 30.0 40.0\n"
        "v cruzC 50.0 60.0\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    // Todos os três vértices devem existir
    TEST_ASSERT_NOT_NULL(graph_get_vertex(g, "cruzA"));
    TEST_ASSERT_NOT_NULL(graph_get_vertex(g, "cruzB"));
    TEST_ASSERT_NOT_NULL(graph_get_vertex(g, "cruzC"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Vértice inexistente não deve aparecer no grafo
void vertice_nao_inserido_retorna_null(void) {
    const char *conteudo =
        "1\n"
        "v cruzA 0.0 0.0\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_NULL(graph_get_vertex(g, "fantasma"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// O payload do vértice deve ter as coordenadas corretas
void coordenadas_do_vertice_estao_corretas(void) {
    const char *conteudo =
        "1\n"
        "v cruzA 12.5 99.9\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    vertex_t *v = graph_get_vertex(g, "cruzA");
    TEST_ASSERT_NOT_NULL(v);

    ponto_t *p = (ponto_t *)vertex_get_data(v);
    TEST_ASSERT_EQUAL_DOUBLE(12.5, ponto_get_x(p));
    TEST_ASSERT_EQUAL_DOUBLE(99.9, ponto_get_y(p));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Aresta simples: os dois vértices devem se tornar adjacentes
void carrega_aresta_simples(void) {
    const char *conteudo =
        "2\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 1.0 1.0\n"
        "e cruzA cruzB cep01 cep02 150.0 13.9 Rua_das_Flores\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_TRUE(is_adjacente(g, "cruzA", "cruzB"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// O payload da aresta deve conter os dados corretos da rua
void payload_da_aresta_contem_dados_corretos(void) {
    const char *conteudo =
        "2\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 1.0 1.0\n"
        "e cruzA cruzB cep01 cep02 200.0 16.7 Av_Brasil\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    edge_t *e = graph_get_edge(g, "cruzA", "cruzB");
    TEST_ASSERT_NOT_NULL(e);

    rua_t *r = (rua_t *)edge_get_data(e);
    TEST_ASSERT_EQUAL_STRING("Av_Brasil", get_nome(r));
    TEST_ASSERT_EQUAL_DOUBLE(200.0, get_cmp(r));
    TEST_ASSERT_EQUAL_DOUBLE(16.7,  get_vm(r));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Aresta é direcional: A -> B não implica B -> A
void aresta_e_direcional(void) {
    const char *conteudo =
        "2\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 1.0 1.0\n"
        "e cruzA cruzB cep01 cep02 100.0 10.0 Rua_Uni\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_TRUE (is_adjacente(g, "cruzA", "cruzB"));
    TEST_ASSERT_FALSE(is_adjacente(g, "cruzB", "cruzA"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Múltiplas arestas saindo do mesmo vértice
void multiplas_arestas_do_mesmo_vertice(void) {
    const char *conteudo =
        "3\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 1.0 0.0\n"
        "v cruzC 0.0 1.0\n"
        "e cruzA cruzB cep01 cep02 50.0 8.0 Rua_X\n"
        "e cruzA cruzC cep03 cep04 80.0 12.0 Rua_Y\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_TRUE(is_adjacente(g, "cruzA", "cruzB"));
    TEST_ASSERT_TRUE(is_adjacente(g, "cruzA", "cruzC"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// graph_set_nv deve ser chamado com o valor lido da primeira linha
void nv_do_grafo_e_setado_corretamente(void) {
    const char *conteudo =
        "4\n"
        "v cruzA 0.0 0.0\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_EQUAL_INT(4, graph_get_nv(g));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Arquivo sem nenhuma linha v ou e: grafo deve ficar vazio mas nv setado
void arquivo_so_com_nv(void) {
    const char *conteudo = "0\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    TEST_ASSERT_EQUAL_INT(0, graph_get_nv(g));
    TEST_ASSERT_NULL(graph_get_vertex(g, "qualquer"));

    graph_destroy(g);
    remove(path);
    free(path);
}

// Nome de rua com espaços deve ser lido corretamente
void nome_de_rua_com_espacos(void) {
    const char *conteudo =
        "2\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 1.0 1.0\n"
        "e cruzA cruzB cep01 cep02 300.0 20.0 Avenida Sete de Setembro\n";

    char *path = criar_via_temp(conteudo);
    graph_t *g = novo_grafo();

    via_handler(path, g);

    edge_t *e = graph_get_edge(g, "cruzA", "cruzB");
    TEST_ASSERT_NOT_NULL(e);

    rua_t *r = (rua_t *)edge_get_data(e);
    TEST_ASSERT_EQUAL_STRING("Avenida Sete de Setembro", get_nome(r));

    graph_destroy(g);
    remove(path);
    free(path);
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(carrega_vertices_simples);
    RUN_TEST(vertice_nao_inserido_retorna_null);
    RUN_TEST(coordenadas_do_vertice_estao_corretas);
    RUN_TEST(carrega_aresta_simples);
    RUN_TEST(payload_da_aresta_contem_dados_corretos);
    RUN_TEST(aresta_e_direcional);
    RUN_TEST(multiplas_arestas_do_mesmo_vertice);
    RUN_TEST(nv_do_grafo_e_setado_corretamente);
    RUN_TEST(arquivo_so_com_nv);
    RUN_TEST(nome_de_rua_com_espacos);

    return UNITY_END();
}