#include "../unity/unity.h"
#include "../include/rua.h"
#include <stdlib.h>

// Rua global reutilizada nos testes que não precisam de estado especial
static rua_t *rua;

void setUp(void) {
    // Cria uma rua padrão antes de cada teste
    rua = rua_init("cep01", "cep02", "Rua_das_Flores", 13.9, 150.0);
}

void tearDown(void) {
    // Libera a rua após cada teste
    rua_destroy(rua);
}

// Verifica se rua_init retorna um ponteiro válido (não NULL)
void init_retorna_nao_nulo(void) {
    TEST_ASSERT_NOT_NULL(rua);
}

// Verifica se os getters retornam os valores passados no construtor
void getters_retornam_valores_iniciais(void) {
    TEST_ASSERT_EQUAL_STRING("cep01",         get_cep_esquerda(rua));
    TEST_ASSERT_EQUAL_STRING("cep02",         get_cep_direita(rua));
    TEST_ASSERT_EQUAL_STRING("Rua_das_Flores", get_nome(rua));
    TEST_ASSERT_EQUAL_DOUBLE(13.9,  get_vm(rua));
    TEST_ASSERT_EQUAL_DOUBLE(150.0, get_cmp(rua));
}

// set_vm deve alterar o valor retornado por get_vm
void set_vm_altera_velocidade(void) {
    set_vm(rua, 25.0);
    TEST_ASSERT_EQUAL_DOUBLE(25.0, get_vm(rua));
}

// set_cmp deve alterar o valor retornado por get_cmp
void set_cmp_altera_comprimento(void) {
    set_cmp(rua, 300.0);
    TEST_ASSERT_EQUAL_DOUBLE(300.0, get_cmp(rua));
}

// set_nome deve alterar o valor retornado por get_nome
void set_nome_altera_nome(void) {
    set_nome(rua, "Av_Paulista");
    TEST_ASSERT_EQUAL_STRING("Av_Paulista", get_nome(rua));
}

// set_cep_esquerda deve alterar o valor retornado por get_cep_esquerda
void set_cep_esquerda_altera_cep(void) {
    set_cep_esquerda(rua, "cep99");
    TEST_ASSERT_EQUAL_STRING("cep99", get_cep_esquerda(rua));
}

// set_cep_direita deve alterar o valor retornado por get_cep_direita
void set_cep_direita_altera_cep(void) {
    set_cep_direita(rua, "cep88");
    TEST_ASSERT_EQUAL_STRING("cep88", get_cep_direita(rua));
}

// Rua criada com CEPs nulos (lado sem quadra, indicado por "-" no (.via))
// não deve crashar nem nos getters
void init_com_cep_nulo(void) {
    rua_t *r = rua_init(NULL, NULL, "Rua_Borda", 10.0, 80.0);
    TEST_ASSERT_NOT_NULL(r);
    rua_destroy(r);
}


// Após set_nome, a rua não deve depender do buffer original
// (a função deve copiar a string, não apenas guardar o ponteiro)
void set_nome_copia_string(void) {
    char nome_temporario[] = "Rua_Temporaria";
    set_nome(rua, nome_temporario);

    // Modifica o buffer original
    nome_temporario[0] = 'X';

    // O nome interno da rua não deve ter mudado
    TEST_ASSERT_EQUAL_STRING("Rua_Temporaria", get_nome(rua));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(init_retorna_nao_nulo);
    RUN_TEST(getters_retornam_valores_iniciais);
    RUN_TEST(set_vm_altera_velocidade);
    RUN_TEST(set_cmp_altera_comprimento);
    RUN_TEST(set_nome_altera_nome);
    RUN_TEST(set_cep_esquerda_altera_cep);
    RUN_TEST(set_cep_direita_altera_cep);
    RUN_TEST(init_com_cep_nulo);
    RUN_TEST(set_nome_copia_string);

    return UNITY_END();
}