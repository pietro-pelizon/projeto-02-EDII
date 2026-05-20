#include "rua.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef struct stRua {
    char *cep_esquerda; // CEP quadra a esquerda
    char *cep_direita;  // CEP quadra a direita
    char *nome;
    double vm;          // Velocidade média
    double cmp;         // Comprimento da rua
} rua_t;

rua_t *rua_init(const char *cep_esquerda, const char *cep_direita, const char *nome, double vm, double cmp) {
    assert(cep_esquerda != NULL || cep_direita != NULL || nome != NULL);

    rua_t *r = malloc(sizeof(rua_t));
    assert(r != NULL);

    r -> cep_esquerda = malloc(strlen(cep_esquerda) + 1);
    strncpy(r -> cep_esquerda, cep_esquerda, strlen(cep_esquerda) + 1);

    r -> cep_direita = malloc (strlen(cep_direita) + 1);
    strncpy(r -> cep_direita, cep_direita, strlen(cep_direita) + 1);

    r -> nome = malloc(strlen(nome) + 1);
    strncpy(r -> nome, nome, strlen(nome) + 1);

    r -> vm = vm;
    r -> cmp = cmp;

    return r;
}

char *get_cep_esquerda(rua_t *r) {
    return r -> cep_esquerda;
}

void set_cep_esquerda(rua_t *r, char *novo_cep_esquerda) {
    assert(novo_cep_esquerda != NULL);

    r -> cep_esquerda = malloc(strlen(novo_cep_esquerda) + 1);
    strncpy(r -> cep_esquerda, novo_cep_esquerda, strlen(novo_cep_esquerda) + 1);
}

char *get_cep_direita(rua_t *r) {
    return r -> cep_direita;
}

void set_cep_direita(rua_t *r, char *novo_cep_direita) {
    assert(novo_cep_direita != NULL);

    r -> cep_direita = malloc(strlen(novo_cep_direita) + 1);
    strncpy(r -> cep_direita, novo_cep_direita, strlen(novo_cep_direita) + 1);
}

char *get_nome(rua_t *r) {
    return r -> nome;
}

void set_nome(rua_t *r, char *novo_nome) {
    assert(novo_nome != NULL);

    r -> nome = malloc(strlen(novo_nome) + 1);
    strncpy(r -> nome, novo_nome, strlen(novo_nome) + 1);
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
