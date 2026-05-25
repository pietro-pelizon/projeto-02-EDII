#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

typedef struct stQuadra {
    char *cep;
    char *corb;
    char *corp;
    char *sw;
    char faces[4];
    double x, y, w, h;

} quadra_t;

// Vou deixar o construtor dessa forma por enquanto, já que não sei quais parâmetros
// irão ser utilizados para criar a quadra no trabalho atual
quadra_t *quadra_init(const char *cep, double x, double y, double w, double h) {
    assert(cep != NULL);

    quadra_t *nova_quadra = calloc(1, sizeof(quadra_t));
    assert(nova_quadra != NULL);

    nova_quadra -> cep = my_strdup(cep);
    nova_quadra -> x = x;
    nova_quadra -> y = y;
    nova_quadra -> w = w;
    nova_quadra -> h = h;

    return nova_quadra;
}

void quadra_destroy(quadra_t *q) {
    if (q == NULL) return;

    free(q -> cep);
    free(q -> corb);
    free(q -> corp);
    free(q -> sw);

    free(q);
}


void quadra_set_cep(quadra_t *q, const char *novo_cep) {
    assert(q != NULL && novo_cep != NULL);

    q -> cep = my_strdup(novo_cep);
}

void quadra_set_x(quadra_t *q, double novo_x) {
    q -> x = novo_x;
}

void quadra_set_y(quadra_t *q, double novo_y) {
    q -> y = novo_y;
}

void quadra_set_w(quadra_t *q, double novo_w) {
    q -> w = novo_w;
}

void quadra_set_h(quadra_t *q, double novo_h) {
    q -> h = novo_h;
}

void quadra_set_corp(quadra_t *q, const char *nova_corp) {
    assert(q != NULL && nova_corp != NULL);

    if (q -> corp != NULL) {
        free(q->corp);
    }

    q -> corp = my_strdup(nova_corp);
}

void quadra_set_corb(quadra_t *q, const char *nova_corb) {
    assert(q != NULL && nova_corb != NULL);

    if (q -> corb != NULL) {
        free(q -> corb);
    }

    q -> corb = my_strdup(nova_corb);

}

void quadra_set_sw(quadra_t *q, const char *novo_sw) {
    assert(q != NULL && novo_sw != NULL);

    if (q -> sw != NULL) {
        free(q -> sw);
    }

    q -> sw = my_strdup(novo_sw);
}

double quadra_get_x(const quadra_t *q) {
    assert(q != NULL);

    return q -> x;
}

double quadra_get_y(const quadra_t *q) {
    assert(q != NULL);

    return q -> y;
}

double quadra_get_w(const quadra_t *q) {
    assert(q != NULL);

    return q -> w;
}

double quadra_get_h(const quadra_t *q) {
    assert(q != NULL);

    return q -> h;
}

const char *quadra_get_corp(const quadra_t *q) {
    assert(q != NULL);

    return q -> corp;
}

const char *quadra_get_corb(const quadra_t *q) {
    assert(q != NULL);

    return q -> corb;
}

const char *quadra_get_sw(const quadra_t *q) {
    assert(q != NULL);

    return q -> sw;
}

const char *quadra_get_cep(const quadra_t *q) {
    assert(q != NULL);

    return q -> cep;
}

void quadra_set_cq(quadra_t *q, const char *sw, const char *corp, const char *corb) {
    assert(q != NULL);

    quadra_set_sw(q, sw);
    quadra_set_corb(q, corb);
    quadra_set_corp(q, corp);
}

void quadra_get_coord(char face, double *x, double *y, quadra_t *quadra, double numero) {
    double qx = quadra_get_x(quadra);
    double qy = quadra_get_y(quadra);
    double qw = quadra_get_w(quadra);
    double qh = quadra_get_h(quadra);

    switch (face) {
        case 'S': {
            *x = qx + numero;
            *y = qy;
            break;
        }
        case 'N': {
            *x = qx + numero;
            *y = qy + qh;
            break;
        }
        case 'L': {
            *x = qx;
            *y = qy + numero;
            break;
        }
        case 'O': {
            *x = qx + qw;
            *y = qy + numero;
            break;
        }
        default: printf("Face %c inválida!\n", face);
    }
}