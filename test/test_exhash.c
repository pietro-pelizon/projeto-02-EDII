#include "../unity/unity.h"
#include "../include/exhash.h"
#include <stdlib.h>
#include <string.h>

#define RECORD_SIZE  sizeof(registro_t)
#define BUCKET_SIZE  256

typedef struct {
    int   id;
    float valor;
    char  nome[32];
} registro_t;

void setUp(void) {}
void tearDown(void) {}


void init_retorna_ponteiro_valido(void) {
    exhash_t *m = exhash_init(sizeof(int), 128);
    TEST_ASSERT_NOT_NULL(m);
    exhash_destroy(m, NULL);
}

void insert_simples(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r = {1, 9.99f, "Alice"};
    TEST_ASSERT_TRUE(exhash_insert(map, &r, "alice-001"));

    exhash_destroy(map, NULL);
}

void insert_chave_duplicada_retorna_false(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r = {1, 9.99f, "Alice"};
    TEST_ASSERT_TRUE(exhash_insert(map, &r, "alice-001"));
    TEST_ASSERT_FALSE(exhash_insert(map, &r, "alice-001"));

    exhash_destroy(map, NULL);
}

void insert_multiplos_registros(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    for (int i = 0; i < 20; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key-%03d", i);
        registro_t r = {i, (float)i * 1.5f, ""};
        strncpy(r.nome, key, sizeof(r.nome) - 1);
        TEST_ASSERT_TRUE(exhash_insert(map, &r, key));
    }

    exhash_destroy(map, NULL);
}

void insert_forcando_split_de_bucket(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    for (int i = 0; i < 100; i++) {
        char key[32];
        snprintf(key, sizeof(key), "stress-key-%d", i);
        registro_t r = {i, (float)i, ""};
        strncpy(r.nome, key, sizeof(r.nome) - 1);
        TEST_ASSERT_TRUE_MESSAGE(exhash_insert(map, &r, key), key);
    }

    exhash_destroy(map, NULL);
}

void search_encontra_registro_inserido(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t original = {42, 3.14f, "Bob"};
    exhash_insert(map, &original, "bob-042");

    registro_t resultado;
    TEST_ASSERT_TRUE(exhash_search(map, "bob-042", &resultado));
    TEST_ASSERT_EQUAL_INT(42, resultado.id);
    TEST_ASSERT_EQUAL_FLOAT(3.14f, resultado.valor);
    TEST_ASSERT_EQUAL_STRING("Bob", resultado.nome);

    exhash_destroy(map, NULL);
}

void search_chave_inexistente_retorna_false(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t resultado;
    TEST_ASSERT_FALSE(exhash_search(map, "nao-existe", &resultado));

    exhash_destroy(map, NULL);
}

void search_apos_multiplas_insercoes(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    for (int i = 0; i < 30; i++) {
        char key[16];
        snprintf(key, sizeof(key), "item-%03d", i);
        registro_t r = {i, (float)i, ""};
        strncpy(r.nome, key, sizeof(r.nome) - 1);
        exhash_insert(map, &r, key);
    }

    int targets[] = {0, 7, 15, 22, 29};
    for (int t = 0; t < 5; t++) {
        char key[16];
        snprintf(key, sizeof(key), "item-%03d", targets[t]);
        registro_t resultado;
        TEST_ASSERT_TRUE(exhash_search(map, key, &resultado));
        TEST_ASSERT_EQUAL_INT(targets[t], resultado.id);
    }

    exhash_destroy(map, NULL);
}

void search_nao_confunde_chaves_similares(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r1 = {1, 1.0f, "Um"};
    registro_t r2 = {2, 2.0f, "Dois"};
    exhash_insert(map, &r1, "chave-1");
    exhash_insert(map, &r2, "chave-2");

    registro_t out;
    exhash_search(map, "chave-1", &out);
    TEST_ASSERT_EQUAL_INT(1, out.id);

    exhash_search(map, "chave-2", &out);
    TEST_ASSERT_EQUAL_INT(2, out.id);

    exhash_destroy(map, NULL);
}

void remove_retorna_dados_corretos(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t original = {99, 7.77f, "Carlos"};
    exhash_insert(map, &original, "carlos-099");

    void *removido = exhash_remove(map, "carlos-099");
    TEST_ASSERT_NOT_NULL(removido);

    registro_t *r = (registro_t *)removido;
    TEST_ASSERT_EQUAL_INT(99, r->id);
    TEST_ASSERT_EQUAL_FLOAT(7.77f, r->valor);
    TEST_ASSERT_EQUAL_STRING("Carlos", r->nome);

    free(removido);
    exhash_destroy(map, NULL);
}

void remove_chave_inexistente_retorna_null(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    TEST_ASSERT_NULL(exhash_remove(map, "fantasma"));

    exhash_destroy(map, NULL);
}

void remove_impede_busca_posterior(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r = {5, 5.5f, "Deletado"};
    exhash_insert(map, &r, "del-key");

    void *removido = exhash_remove(map, "del-key");
    TEST_ASSERT_NOT_NULL(removido);
    free(removido);

    registro_t out;
    TEST_ASSERT_FALSE(exhash_search(map, "del-key", &out));

    exhash_destroy(map, NULL);
}

void remove_permite_reinsercao_da_chave(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r1 = {1, 1.0f, "V1"};
    exhash_insert(map, &r1, "reusar");

    void *removido = exhash_remove(map, "reusar");
    TEST_ASSERT_NOT_NULL(removido);
    free(removido);

    registro_t r2 = {2, 2.0f, "V2"};
    TEST_ASSERT_TRUE(exhash_insert(map, &r2, "reusar"));

    registro_t out;
    exhash_search(map, "reusar", &out);
    TEST_ASSERT_EQUAL_INT(2, out.id);

    exhash_destroy(map, NULL);
}

void remove_nao_corrompe_outros_registros(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t a = {1, 1.0f, "A"};
    registro_t b = {2, 2.0f, "B"};
    registro_t c = {3, 3.0f, "C"};
    exhash_insert(map, &a, "ka");
    exhash_insert(map, &b, "kb");
    exhash_insert(map, &c, "kc");

    void *removido = exhash_remove(map, "kb");
    TEST_ASSERT_NOT_NULL(removido);
    free(removido);

    registro_t out;
    TEST_ASSERT_TRUE(exhash_search(map, "ka", &out));
    TEST_ASSERT_EQUAL_INT(1, out.id);

    TEST_ASSERT_TRUE(exhash_search(map, "kc", &out));
    TEST_ASSERT_EQUAL_INT(3, out.id);

    TEST_ASSERT_FALSE(exhash_search(map, "kb", &out));

    exhash_destroy(map, NULL);
}

void insert_chave_vazia(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t r = {0, 0.0f, "Vazio"};
    bool ok = exhash_insert(map, &r, "");
    (void)ok;

    exhash_destroy(map, NULL);
}

void search_chave_vazia(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    registro_t out;
    bool ok = exhash_search(map, "", &out);

    TEST_ASSERT_FALSE(ok);

    exhash_destroy(map, NULL);
}

void insert_chave_longa(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);

    const char *chave_longa =
        "esta-e-uma-chave-muito-longa-para-testar-robustez-do-hash-"
        "extendivel-em-situacoes-extremas-de-comprimento-de-string";
    registro_t r = {777, 7.0f, "Longa"};
    TEST_ASSERT_TRUE(exhash_insert(map, &r, chave_longa));

    registro_t out;
    TEST_ASSERT_TRUE(exhash_search(map, chave_longa, &out));
    TEST_ASSERT_EQUAL_INT(777, out.id);

    exhash_destroy(map, NULL);
}

void carga_pesada_sem_corrupcao(void) {
    exhash_t *map = exhash_init(RECORD_SIZE, BUCKET_SIZE);
    const int N = 200;

    for (int i = 0; i < N; i++) {
        char key[32];
        snprintf(key, sizeof(key), "carga-%d", i);
        registro_t r = {i, (float)i * 0.1f};
        strncpy(r.nome, key, sizeof(r.nome) - 1);
        TEST_ASSERT_TRUE(exhash_insert(map, &r, key));
    }

    for (int i = 0; i < N; i++) {
        char key[32];
        snprintf(key, sizeof(key), "carga-%d", i);
        registro_t out;
        TEST_ASSERT_TRUE(exhash_search(map, key, &out));
        TEST_ASSERT_EQUAL_INT(i, out.id);
    }

    exhash_destroy(map, NULL);
}

void test_exhash_update(void) {
    exhash_t *hash = exhash_init(sizeof(double), 32);
    TEST_ASSERT_NOT_NULL(hash);

    double valor_inicial = 15.5;
    exhash_insert(hash, &valor_inicial, "v1");

    double valor_novo = 42.0;
    bool atualizou = exhash_update(hash, "v1", &valor_novo);
    TEST_ASSERT_TRUE_MESSAGE(atualizou, "O update deveria retornar true para chave existente");

    double valor_lido = 0.0;
    bool achou = exhash_search(hash, "v1", &valor_lido);
    TEST_ASSERT_TRUE(achou);
    TEST_ASSERT_EQUAL_DOUBLE(42.0, valor_lido);

    double outro_valor = 99.9;
    bool atualizou_falso = exhash_update(hash, "v_fantasma", &outro_valor);
    TEST_ASSERT_FALSE_MESSAGE(atualizou_falso, "O update deveria retornar false para chave inexistente");

    exhash_destroy(hash, NULL);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(init_retorna_ponteiro_valido);

    RUN_TEST(insert_simples);
    RUN_TEST(insert_chave_duplicada_retorna_false);
    RUN_TEST(insert_multiplos_registros);
    RUN_TEST(insert_forcando_split_de_bucket);

    RUN_TEST(search_encontra_registro_inserido);
    RUN_TEST(search_chave_inexistente_retorna_false);
    RUN_TEST(search_apos_multiplas_insercoes);
    RUN_TEST(search_nao_confunde_chaves_similares);

    RUN_TEST(remove_retorna_dados_corretos);
    RUN_TEST(remove_chave_inexistente_retorna_null);
    RUN_TEST(remove_impede_busca_posterior);
    RUN_TEST(remove_permite_reinsercao_da_chave);
    RUN_TEST(remove_nao_corrompe_outros_registros);

    RUN_TEST(insert_chave_vazia);
    RUN_TEST(search_chave_vazia);
    RUN_TEST(insert_chave_longa);
    RUN_TEST(carga_pesada_sem_corrupcao);

    RUN_TEST(test_exhash_update);

    return UNITY_END();
}