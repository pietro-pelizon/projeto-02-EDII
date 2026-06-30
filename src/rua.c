#include "rua.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


typedef struct stRua {
    char *cep_esquerda;
    char *cep_direita;
    char *nome;
    double velocidade_media;
    double comprimento;
} rua_t;

rua_t *rua_init(char *cep_esquerda, char *cep_direita, const char *nome, double vm, double cmp) {
    assert(nome != NULL);

    rua_t *r = malloc(sizeof(rua_t));
    assert(r != NULL);

    r -> cep_esquerda = my_strdup(cep_esquerda);
    r -> cep_direita = my_strdup(cep_direita);
    r -> nome = my_strdup(nome);

    r -> velocidade_media = vm;
    r -> comprimento = cmp;

    return r;
}

char *rua_get_cep_esquerda(rua_t *r) {
    return r -> cep_esquerda;
}

void rua_set_cep_esquerda(rua_t *r, char *cep_esquerda) {
    assert(cep_esquerda != NULL);

    if (r -> cep_esquerda != NULL) {
        free(r -> cep_esquerda);
    }

    r -> cep_esquerda = my_strdup(cep_esquerda);
}

char *rua_get_cep_direita(rua_t *r) {
    return r -> cep_direita;
}

void rua_set_cep_direita(rua_t *r, char *novo_cep_direita) {
    assert(novo_cep_direita != NULL);

    if (r -> cep_direita != NULL) {
        free(r -> cep_direita);
    }

    r -> cep_direita = my_strdup(novo_cep_direita);
}

char *rua_get_nome(rua_t *r) {
    return r -> nome;
}

void rua_set_nome(rua_t *r, char *novo_nome) {
    assert(novo_nome != NULL);

    if (r -> nome != NULL) {
        free(r -> nome);
    }

    r -> nome = my_strdup(novo_nome);
}

double rua_get_velocidade_media(rua_t *r) {
    return r -> velocidade_media;
}

void rua_set_velocidade_media(rua_t *r, double velocidade_media) {
    r -> velocidade_media = velocidade_media;
}

double rua_get_comprimento(rua_t *r) {
    return r -> comprimento;
}

void rua_set_comprimento(rua_t *r, double comprimento) {
    r -> comprimento = comprimento;
}

void rua_destroy(rua_t *r) {
    assert(r);

    free(r -> cep_direita);
    free(r -> cep_esquerda);
    free(r -> nome);

    free(r);
}
