#ifndef PROJETO_02_EDII_RUA_H
#define PROJETO_02_EDII_RUA_H

typedef struct stRua rua_t;

rua_t *rua_init(const char *cep_esquerda, const char *cep_direita, const char *nome, double vm, double cmp);

char *get_cep_esquerda(rua_t *r);

void set_cep_esquerda(rua_t *r, char *novo_cep_esquerda);

char *get_cep_direita(rua_t *r);

void set_cep_direita(rua_t *r, char *novo_cep_direita);

char *get_nome(rua_t *r);

void set_nome(rua_t *r, char *novo_nome);

double get_vm(rua_t *r);

void set_vm(rua_t *r, double nova_vm);

double get_cmp(rua_t *r);

void set_cmp(rua_t *r, double novo_cmp);

#endif //PROJETO_02_EDII_RUA_H
