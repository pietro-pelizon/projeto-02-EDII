#include "../unity/unity.h"
#include "../include/quadra.h"
#include <math.h>

#define EPSILON 1e-6

void setUp(void) {}
void tearDown(void) {}


void init_retorna_ponteiro_valido(void) {
    quadra_t *q = quadra_init("12345-678", 10.0, 20.0, 100.0, 50.0);
    TEST_ASSERT_NOT_NULL(q);
    quadra_destroy(q);
}

void init_preserva_dados(void) {
    quadra_t *q = quadra_init("12345-678", 10.0, 20.0, 100.0, 50.0);

    TEST_ASSERT_EQUAL_STRING("12345-678", quadra_get_cep(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 10.0,  quadra_get_x(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 20.0,  quadra_get_y(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 100.0, quadra_get_w(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 50.0,  quadra_get_h(q));

    quadra_destroy(q);
}

void set_cep_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_cep(q, "99999-999");
    TEST_ASSERT_EQUAL_STRING("99999-999", quadra_get_cep(q));

    quadra_destroy(q);
}

void set_x_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_x(q, 55.5);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 55.5, quadra_get_x(q));

    quadra_destroy(q);
}

void set_y_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_y(q, 77.7);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 77.7, quadra_get_y(q));

    quadra_destroy(q);
}

void set_w_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_w(q, 200.0);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 200.0, quadra_get_w(q));

    quadra_destroy(q);
}

void set_h_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_h(q, 300.0);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 300.0, quadra_get_h(q));

    quadra_destroy(q);
}

void set_corp_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_corp(q, "blue");
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_corp(q));

    quadra_destroy(q);
}

void set_corb_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_corb(q, "red");
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_corb(q));

    quadra_destroy(q);
}

void set_sw_altera_valor(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_sw(q, "3");
    TEST_ASSERT_EQUAL_STRING("3", quadra_get_sw(q));

    quadra_destroy(q);
}

void set_cq_altera_todas_as_visuais(void) {
    quadra_t *q = quadra_init("00000-000", 0.0, 0.0, 10.0, 10.0);

    quadra_set_cq(q, "2", "green", "black");
    TEST_ASSERT_EQUAL_STRING("green", quadra_get_corp(q));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_corb(q));
    TEST_ASSERT_EQUAL_STRING("2",     quadra_get_sw(q));

    quadra_destroy(q);
}

void set_cq_nao_altera_dados_espaciais(void) {
    quadra_t *q = quadra_init("12345-678", 10.0, 20.0, 100.0, 50.0);

    quadra_set_cq(q, "5", "yellow", "purple");

    TEST_ASSERT_EQUAL_STRING("12345-678", quadra_get_cep(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 10.0,  quadra_get_x(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 20.0,  quadra_get_y(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 100.0, quadra_get_w(q));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 50.0,  quadra_get_h(q));

    quadra_destroy(q);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(init_retorna_ponteiro_valido);
    RUN_TEST(init_preserva_dados);

    RUN_TEST(set_cep_altera_valor);
    RUN_TEST(set_x_altera_valor);
    RUN_TEST(set_y_altera_valor);
    RUN_TEST(set_w_altera_valor);
    RUN_TEST(set_h_altera_valor);

    RUN_TEST(set_corp_altera_valor);
    RUN_TEST(set_corb_altera_valor);
    RUN_TEST(set_sw_altera_valor);

    RUN_TEST(set_cq_altera_todas_as_visuais);
    RUN_TEST(set_cq_nao_altera_dados_espaciais);

    return UNITY_END();
}