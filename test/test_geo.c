#include "../unity/unity.h"
#include "../include/geo_handler.h"
#include "../include/exhash.h"
#include "../include/quadra.h"
#include <stdio.h>
#include <stdlib.h>

#define GEO_TMP "/tmp/test_geo_handler.geo"

void setUp(void) {}
void tearDown(void) {}

static void escreve_geo(const char *conteudo) {
    FILE *f = fopen(GEO_TMP, "w");
    fputs(conteudo, f);
    fclose(f);
}


void arquivo_inexistente_retorna_null(void) {
    exhash_t *h = geo_handler("/tmp/nao_existe_jamais.geo");
    TEST_ASSERT_NULL(h);
}

void arquivo_vazio_retorna_exhash_valido(void) {
    escreve_geo("");
    exhash_t *h = geo_handler(GEO_TMP);
    TEST_ASSERT_NOT_NULL(h);
    exhash_destroy(h);
}

void comando_q_insere_quadra(void) {
    escreve_geo("q cep01 10.0 20.0 100.0 50.0\n");

    exhash_t *h = geo_handler(GEO_TMP);
    TEST_ASSERT_NOT_NULL(h);

    quadra_t *q = NULL;
    TEST_ASSERT_TRUE(exhash_search(h, "cep01", &q));
    TEST_ASSERT_NOT_NULL(q);

    exhash_destroy(h);
}

void comando_q_preserva_dados_espaciais(void) {
    escreve_geo("q cep01 10.0 20.0 100.0 50.0\n");

    exhash_t *h = geo_handler(GEO_TMP);
    quadra_t *q = NULL;
    exhash_search(h, "cep01", &q);

    TEST_ASSERT_EQUAL_STRING("cep01", quadra_get_cep(q));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 10.0,  quadra_get_x(q));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 20.0,  quadra_get_y(q));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 100.0, quadra_get_w(q));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 50.0,  quadra_get_h(q));

    exhash_destroy(h);
}

void cores_padrao_aplicadas_sem_cq(void) {
    escreve_geo("q cep01 0.0 0.0 10.0 10.0\n");

    exhash_t *h = geo_handler(GEO_TMP);
    quadra_t *q = NULL;
    exhash_search(h, "cep01", &q);

    TEST_ASSERT_EQUAL_STRING("white", quadra_get_corp(q));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_corb(q));
    TEST_ASSERT_EQUAL_STRING("1.0px", quadra_get_sw(q));

    exhash_destroy(h);
}

void comando_cq_altera_cores_das_quadras_seguintes(void) {
    escreve_geo(
        "q antes 0.0 0.0 10.0 10.0\n"
        "cq 2px red blue\n"
        "q depois 0.0 0.0 10.0 10.0\n"
    );

    exhash_t *h = geo_handler(GEO_TMP);

    quadra_t *antes = NULL, *depois = NULL;
    exhash_search(h, "antes",  &antes);
    exhash_search(h, "depois", &depois);

    TEST_ASSERT_EQUAL_STRING("white", quadra_get_corp(antes));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_corb(antes));

    TEST_ASSERT_EQUAL_STRING("red",  quadra_get_corp(depois));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_corb(depois));
    TEST_ASSERT_EQUAL_STRING("2px",  quadra_get_sw(depois));

    exhash_destroy(h);
}

void comando_cq_nao_afeta_quadras_anteriores(void) {
    escreve_geo(
        "q cep01 0.0 0.0 10.0 10.0\n"
        "cq 5px green yellow\n"
        "q cep02 0.0 0.0 10.0 10.0\n"
    );

    exhash_t *h = geo_handler(GEO_TMP);

    quadra_t *q1 = NULL;
    exhash_search(h, "cep01", &q1);

    TEST_ASSERT_EQUAL_STRING("white", quadra_get_corp(q1));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_corb(q1));
    TEST_ASSERT_EQUAL_STRING("1.0px", quadra_get_sw(q1));

    exhash_destroy(h);
}

void multiplos_cq_usa_o_mais_recente(void) {
    escreve_geo(
        "cq 1px red blue\n"
        "cq 3px lime pink\n"
        "q cep01 0.0 0.0 10.0 10.0\n"
    );

    exhash_t *h = geo_handler(GEO_TMP);

    quadra_t *q = NULL;
    exhash_search(h, "cep01", &q);

    TEST_ASSERT_EQUAL_STRING("lime", quadra_get_corp(q));
    TEST_ASSERT_EQUAL_STRING("pink", quadra_get_corb(q));
    TEST_ASSERT_EQUAL_STRING("3px",  quadra_get_sw(q));

    exhash_destroy(h);
}

void multiplas_quadras_todas_inseridas(void) {
    escreve_geo(
        "q cep01  0.0 0.0 10.0 10.0\n"
        "q cep02 20.0 0.0 10.0 10.0\n"
        "q cep03 40.0 0.0 10.0 10.0\n"
    );

    exhash_t *h = geo_handler(GEO_TMP);

    quadra_t *q1 = NULL, *q2 = NULL, *q3 = NULL;
    TEST_ASSERT_TRUE(exhash_search(h, "cep01", &q1));
    TEST_ASSERT_TRUE(exhash_search(h, "cep02", &q2));
    TEST_ASSERT_TRUE(exhash_search(h, "cep03", &q3));

    TEST_ASSERT_DOUBLE_WITHIN(1e-6,  0.0, quadra_get_x(q1));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 20.0, quadra_get_x(q2));
    TEST_ASSERT_DOUBLE_WITHIN(1e-6, 40.0, quadra_get_x(q3));

    exhash_destroy(h);
}

void linhas_desconhecidas_sao_ignoradas(void) {
    escreve_geo(
        "# comentario qualquer\n"
        "z foo bar\n"
        "q cep01 1.0 2.0 3.0 4.0\n"
    );

    exhash_t *h = geo_handler(GEO_TMP);
    TEST_ASSERT_NOT_NULL(h);

    quadra_t *q = NULL;
    TEST_ASSERT_TRUE(exhash_search(h, "cep01", &q));

    exhash_destroy(h);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(arquivo_inexistente_retorna_null);
    RUN_TEST(arquivo_vazio_retorna_exhash_valido);

    RUN_TEST(comando_q_insere_quadra);
    RUN_TEST(comando_q_preserva_dados_espaciais);
    RUN_TEST(cores_padrao_aplicadas_sem_cq);

    RUN_TEST(comando_cq_altera_cores_das_quadras_seguintes);
    RUN_TEST(comando_cq_nao_afeta_quadras_anteriores);
    RUN_TEST(multiplos_cq_usa_o_mais_recente);

    RUN_TEST(multiplas_quadras_todas_inseridas);
    RUN_TEST(linhas_desconhecidas_sao_ignoradas);

    return UNITY_END();
}