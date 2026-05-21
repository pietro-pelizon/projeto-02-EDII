#include "rua.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef struct stRua {
    char *cep_esquerda; // CEP quadra a esquerda
    char *cep_direita;  // CEP quadra a direita
    char *nome;
    double vm;          // Velocidade média (m/s)
    double cmp;         // Comprimento da rua (metros)
} rua_t;

rua_t *rua_init(const char *cep_esquerda, const char *cep_direita, const char *nome, double vm, double cmp) {
    assert(cep_esquerda != NULL || cep_direita != NULL || nome != NULL);

    rua_t *r = malloc(sizeof(rua_t));
    assert(r != NULL);

    // Lembrar que strdup faz malloc implicitamente
    r -> cep_esquerda = strdup(cep_esquerda);
    r -> cep_direita = strdup(cep_direita);
    r -> nome = strdup(nome);

    r -> vm = vm;
    r -> cmp = cmp;

    return r;
}

char *get_cep_esquerda(rua_t *r) {
    return r -> cep_esquerda;
}

void set_cep_esquerda(rua_t *r, char *novo_cep_esquerda) {
    assert(novo_cep_esquerda != NULL);

    if (r -> cep_esquerda != NULL) {
        free(r -> cep_esquerda);
    }

    r -> cep_esquerda = strdup(novo_cep_esquerda);
}

char *get_cep_direita(rua_t *r) {
    return r -> cep_direita;
}

void set_cep_direita(rua_t *r, char *novo_cep_direita) {
    assert(novo_cep_direita != NULL);

    if (r -> cep_direita != NULL) {
        free(r -> cep_direita);
    }

    r -> cep_direita = strdup(novo_cep_direita);
}

char *get_nome(rua_t *r) {
    return r -> nome;
}

void set_nome(rua_t *r, char *novo_nome) {
    assert(novo_nome != NULL);

    if (r -> nome != NULL) {
        free(r -> nome);
    }

    r -> nome = strdup(novo_nome);
}

double get_vm(rua_t *r) {
    return r -> vm;
}

void set_vm(rua_t *r, double nova_vm) {
    r -> vm = nova_vm;
}

double get_cmp(rua_t *r) {
    return r -> cmp;
}

void set_cmp(rua_t *r, double novo_cmp) {
    r -> cmp = novo_cmp;
}
