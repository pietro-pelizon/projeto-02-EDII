#include "../unity/unity.h"
#include "../include/qry_handler.h"
#include "../include/via_handler.h"
#include "../include/geo_handler.h"
#include "../include/grafo.h"
#include "../include/rua.h"
#include "../include/ponto.h"
#include "../include/quadra.h"
#include "../include/exhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QRY_TMP "/tmp/test_handler.qry"
#define VIA_TMP "/tmp/test_base.via"
#define GEO_TMP "/tmp/test_base.geo"
#define TXT_TMP "/tmp/test_saida.txt"

static void destrutor_ponto(void *data) { ponto_destroy((ponto_t *)data); }
static void destrutor_rua(void *data)   { rua_destroy((rua_t *)data); }
static void destrutor_quadra(void *data) {
    quadra_t *q = *(quadra_t **)data;
    if (q != NULL) {
            quadra_destroy(q);
        }
}

void setUp(void) {}
void tearDown(void) {}


static void escreve_arquivo(const char *path, const char *conteudo) {
    FILE *f = fopen(path, "w");
    fputs(conteudo, f);
    fclose(f);
}

static char *ler_txt(void) {
    FILE *f = fopen(TXT_TMP, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    rewind(f);
    char *buf = calloc(1, tam + 1);
    fread(buf, 1, tam, f);
    fclose(f);
    return buf;
}

// Cria um grafo simples (A -> B)
static graph_t *grafo_simples(void) {
    escreve_arquivo(VIA_TMP,
        "2\n"
        "v cruzA 0.0 0.0\n"
        "v cruzB 100.0 0.0\n"
        "e cruzA cruzB cep01 cep02 100.0 10.0 Rua_Teste\n"
    );
    graph_t *g = graph_init(destrutor_rua, destrutor_ponto);
    via_handler(VIA_TMP, g);
    return g;
}

// Cria uma base de quadras usando o próprio geo_handler
static exhash_t *quadras_simples(void) {
    escreve_arquivo(GEO_TMP,
        "q cep01 0.0 0.0 100.0 100.0\n"
        "q cep02 100.0 0.0 100.0 100.0\n"
    );
    return geo_handler(GEO_TMP);
}


void comando_regs_gera_saida_no_txt(void) {
    escreve_arquivo(QRY_TMP, "regs 10.0\n");

    graph_t *g = grafo_simples();
    exhash_t *quadras = exhash_init(sizeof(void*), 8);
    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    char *saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    TEST_ASSERT_NOT_NULL(strstr(saida, "[*] regs"));

    free(saida);
    exhash_destroy(quadras, NULL);
    graph_destroy(g);
}

void comando_mvm_altera_velocidade_da_aresta(void) {
    escreve_arquivo(QRY_TMP, "mvm 55.5 0.0 0.0 200.0 200.0\n");

    graph_t *g = grafo_simples();
    exhash_t *quadras = exhash_init(sizeof(void*), 8);
    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    edge_t *aresta = graph_get_edge(g, "cruzA", "cruzB");
    rua_t *rua = edge_get_data(aresta);
    TEST_ASSERT_EQUAL_DOUBLE(55.5, rua_get_velocidade_media(rua));

    exhash_destroy(quadras, NULL);
    graph_destroy(g);
}

void comando_exp_aumenta_velocidade_em_50_porcento(void) {
    escreve_arquivo(QRY_TMP, "exp 15.0\n");

    graph_t *g = grafo_simples();
    exhash_t *quadras = exhash_init(sizeof(void*), 8);
    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    edge_t *aresta = graph_get_edge(g, "cruzA", "cruzB");
    rua_t *rua = edge_get_data(aresta);
    TEST_ASSERT_EQUAL_DOUBLE(15.0, rua_get_velocidade_media(rua));

    exhash_destroy(quadras, NULL);
    graph_destroy(g);
}


void comando_ao_registra_ponto_e_gera_saida(void) {
    // Registra R1 na face Norte da quadra cep01
    escreve_arquivo(QRY_TMP, "@o? R1 cep01 N 50.0\n");

    graph_t *g = grafo_simples();
    exhash_t *quadras = quadras_simples();

    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    char *saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    // Verifica se a string foi gravada no arquivo corretamente
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(saida, "[*] @o? R1 cep01 N 50.0"), "Comando @o? não foi gravado no txt");

    free(saida);
    exhash_destroy(quadras, destrutor_quadra);
    graph_destroy(g);
}

void comando_p_executa_dijkstra_e_gera_saida(void) {
    // Um script QRY completo: registra origem, destino e pede a rota
    escreve_arquivo(QRY_TMP,
        "@o? R1 cep01 N 10.0\n"
        "@o? R2 cep02 S 10.0\n"
        "p? R1 R2 blue red\n"
    );

    graph_t *g = grafo_simples();
    exhash_t *quadras = quadras_simples();

    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    char *saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    // Como R1 e R2 existem e foram interligados no grafo simples, o Dijkstra deve passar
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(saida, "[*] p? R1 R2 blue red"), "Comando p? não foi concluído");

    free(saida);
    exhash_destroy(quadras, destrutor_quadra);
    graph_destroy(g);
}

void comando_p_ignora_registradores_inexistentes(void) {
    // Tenta traçar rota entre registradores que nunca foram criados
    escreve_arquivo(QRY_TMP, "p? R99 R100 blue red\n");

    graph_t *g = grafo_simples();
    exhash_t *quadras = quadras_simples();

    FILE *svg = fopen("/dev/null", "w");
    FILE *txt = fopen(TXT_TMP, "w");

    qry_handler(QRY_TMP, g, quadras, svg, txt);

    fclose(svg);
    fclose(txt);

    char *saida = ler_txt();
    TEST_ASSERT_NULL_MESSAGE(strstr(saida, "[*] p?"), "O comando p? prosseguiu mesmo com registradores inválidos!");

    free(saida);
    exhash_destroy(quadras, destrutor_quadra);
    graph_destroy(g);
}

// =============== MAIN ===============

int main(void) {
    UNITY_BEGIN();

    // Comandos de alteração
    RUN_TEST(comando_regs_gera_saida_no_txt);
    RUN_TEST(comando_mvm_altera_velocidade_da_aresta);
    RUN_TEST(comando_exp_aumenta_velocidade_em_50_porcento);

    // Comandos de roteamento
    RUN_TEST(comando_ao_registra_ponto_e_gera_saida);
    RUN_TEST(comando_p_executa_dijkstra_e_gera_saida);
    RUN_TEST(comando_p_ignora_registradores_inexistentes);

    return UNITY_END();
}