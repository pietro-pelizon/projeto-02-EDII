#include "../unity/unity.h"
#include "../include/priority_queue.h"
#include <stdlib.h>

void setUp(void) {}

void tearDown(void) {}

// Teste se o removido será o de maior prioridade (menor valor)
void ordem_min_heap(void) {
    pqueue_t *pq = pq_init(5);

    pq_enqueue(pq, "Rua_C", 30.5);
    pq_enqueue(pq, "Rua_A", 10.2);
    pq_enqueue(pq, "Rua_B", 20.0);

    TEST_ASSERT_FALSE(pq_is_empty(pq));

    // O primeiro a sair DEVE ser a Rua_A
    char *removido = pq_dequeue(pq);
    TEST_ASSERT_EQUAL_STRING("Rua_A", removido);

    free(removido);
    pq_destroy(pq);
}

// Deve ser capaz de inserir a mais do que a capacidade
// total, já que agora o array interno é "reallocado"
void inserindo_mais_do_que_capacidade(void) {
    pqueue_t *q = pq_init(5);

    pq_enqueue(q, "rua_9", 30.48);
    pq_enqueue(q, "rua_8", 30.31);
    pq_enqueue(q, "rua_7", 30.32);
    pq_enqueue(q, "rua_6", 30.13);
    pq_enqueue(q, "rua_5", 30.12);
    pq_enqueue(q, "rua_4", 30.23);
    pq_enqueue(q, "rua_3", 30.53);
    pq_enqueue(q, "rua_2", 12.3);
    pq_enqueue(q, "rua_1", 30.94);

    char *removido = pq_dequeue(q);

    TEST_ASSERT_EQUAL_STRING("rua_2", removido);

    free(removido);
    pq_destroy(q);
}

void subindo_prioridade(void) {
    pqueue_t *q = pq_init(5);

    pq_enqueue(q, "rua_9", 60.48);
    pq_enqueue(q, "rua_8", 30.31);
    pq_enqueue(q, "rua_7", 30.32);
    pq_enqueue(q, "rua_6", 30.13);
    pq_enqueue(q, "rua_5", 30.12);

    pq_change_priority(q, "rua_9", 10.0);

    char *removido = pq_dequeue(q);

    // Deve ser ID "rua_9", pois a prioridade foi alterada
    TEST_ASSERT_EQUAL_STRING("rua_9", removido);

    free(removido);
    pq_destroy(q);
}

void diminuindo_prioridade(void) {
    pqueue_t *q = pq_init(5);

    pq_enqueue(q, "rua_9", 60.48);
    pq_enqueue(q, "rua_8", 30.31);
    pq_enqueue(q, "rua_7", 30.32);
    pq_enqueue(q, "rua_6", 30.13);
    pq_enqueue(q, "rua_5", 30.12);

    pq_change_priority(q, "rua_5", 99.0);

    char *removido = pq_dequeue(q);

    // Deve ser ID "rua_6", pois passou a ser o dado de maior prioridade
    TEST_ASSERT_EQUAL_STRING("rua_6", removido);

    free(removido);
    pq_destroy(q);
}

void fila_esta_cheia(void) {
    pqueue_t *q = pq_init(5);
    pq_enqueue(q, "rua_9", 10);
    pq_enqueue(q, "rua_8", 9);
    pq_enqueue(q, "rua_7", 8);
    pq_enqueue(q, "rua_6", 7);
    pq_enqueue(q, "rua_5", 6);
    TEST_ASSERT_TRUE(pq_is_full(q));

    pq_enqueue(q, "rua_4", 5);
    TEST_ASSERT_FALSE(pq_is_full(q));

    pq_destroy(q);
}

void fila_esta_vazia(void) {
    pqueue_t *q = pq_init(5);

    TEST_ASSERT_TRUE(pq_is_empty(q));

    pq_destroy(q);
}

void dequeue_em_fila_vazia(void) {
    pqueue_t *q = pq_init(5);

    char *removido = pq_dequeue(q);
    TEST_ASSERT_NULL(removido);

    pq_destroy(q);
}

void is_full_apos_e_antes_de_insercao(void) {
    pqueue_t *q = pq_init(3);

    pq_enqueue(q, "rua_1", 10.0);
    pq_enqueue(q, "rua_2", 20.0);
    pq_enqueue(q, "rua_3", 30.0);

    TEST_ASSERT_TRUE(pq_is_full(q));

    char *removido = pq_dequeue(q);
    free(removido);

    TEST_ASSERT_FALSE(pq_is_full(q));

    pq_destroy(q);
}

void fila_vazia_apos_remover_tudo(void) {
    pqueue_t *q = pq_init(3);

    pq_enqueue(q, "rua_1", 10.0);
    pq_enqueue(q, "rua_2", 20.0);
    pq_enqueue(q, "rua_3", 30.0);

    char *r1 = pq_dequeue(q);
    char *r2 = pq_dequeue(q);
    char *r3 = pq_dequeue(q);

    free(r1);
    free(r2);
    free(r3);

    TEST_ASSERT_TRUE(pq_is_empty(q));

    pq_destroy(q);
}

// Testa se a ordem de saída do min-heap está correta para múltiplos dequeues
void ordem_completa_min_heap(void) {
    pqueue_t *q = pq_init(3);

    pq_enqueue(q, "rua_C", 30.0);
    pq_enqueue(q, "rua_A", 10.0);
    pq_enqueue(q, "rua_B", 20.0);

    char *primeiro  = pq_dequeue(q);
    char *segundo   = pq_dequeue(q);
    char *terceiro  = pq_dequeue(q);

    TEST_ASSERT_EQUAL_STRING("rua_A", primeiro);
    TEST_ASSERT_EQUAL_STRING("rua_B", segundo);
    TEST_ASSERT_EQUAL_STRING("rua_C", terceiro);

    free(primeiro); free(segundo); free(terceiro);

    pq_destroy(q);
}

// Testa change_priority para um id inexistente (não deve quebrar)
void change_priority_id_inexistente(void) {
    pqueue_t *q = pq_init(3);

    pq_enqueue(q, "rua_1", 10.0);
    pq_change_priority(q, "rua_X", 1.0);   // id que não existe

    char *removido = pq_dequeue(q);
    TEST_ASSERT_EQUAL_STRING("rua_1", removido);
    free(removido);

    pq_destroy(q);
}

// Testa enqueue com prioridades iguais (ambos devem entrar sem quebrar)
void enqueue_prioridades_iguais(void) {
    pqueue_t *q = pq_init(3);

    pq_enqueue(q, "rua_A", 10.0);
    pq_enqueue(q, "rua_B", 10.0);

    TEST_ASSERT_FALSE(pq_is_empty(q));

    char *r1 = pq_dequeue(q);
    char *r2 = pq_dequeue(q);

    free(r1); free(r2);

    pq_destroy(q);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(ordem_min_heap);
    RUN_TEST(inserindo_mais_do_que_capacidade);
    RUN_TEST(subindo_prioridade);
    RUN_TEST(diminuindo_prioridade);
    RUN_TEST(fila_esta_cheia);
    RUN_TEST(fila_esta_vazia);
    RUN_TEST(dequeue_em_fila_vazia);
    RUN_TEST(is_full_apos_e_antes_de_insercao);
    RUN_TEST(fila_vazia_apos_remover_tudo);
    RUN_TEST(ordem_completa_min_heap);
    RUN_TEST(change_priority_id_inexistente);
    RUN_TEST(enqueue_prioridades_iguais);
    return UNITY_END();
}