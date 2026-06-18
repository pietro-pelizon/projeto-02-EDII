#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ponto.h"
#include "utils.h"

typedef struct stQuadra {
    char *cep;
    char *cor_borda;
    char *cor_preenchimento;
    char *stroke_width;
    ponto_t *p;
    double width, height;

} quadra_t;

quadra_t *quadra_init(const char *cep, double x, double y, double w, double h) {
    assert(cep != NULL);

    quadra_t *nova_quadra = calloc(1, sizeof(quadra_t));
    assert(nova_quadra != NULL);

    nova_quadra -> cep = my_strdup(cep);
    nova_quadra -> p = ponto_init(x, y);
    nova_quadra -> width = w;
    nova_quadra -> height = h;

    return nova_quadra;
}

void quadra_destroy(quadra_t *q) {
    if (q == NULL) return;

    free(q -> cep);
    free(q -> cor_borda);
    free(q -> cor_preenchimento);
    free(q -> stroke_width);
    free(q -> p);

    free(q);
}


void quadra_set_cep(quadra_t *q, const char *novo_cep) {
    assert(q != NULL && novo_cep != NULL);

    free(q -> cep);
    q -> cep = my_strdup(novo_cep);
}

void quadra_set_x(quadra_t *q, double novo_x) {
    ponto_set_x(q -> p, novo_x);
}

void quadra_set_y(quadra_t *q, double novo_y) {
    ponto_set_y(q -> p, novo_y);
}

void quadra_set_width(quadra_t *q, double width) {
    q -> width = width;
}

void quadra_set_height(quadra_t *q, double nova_height) {
    q -> height = nova_height;
}

void quadra_set_cor_preenchimento(quadra_t *q, const char *cor_preenchimento) {
    assert(q != NULL && cor_preenchimento != NULL);

    if (q -> cor_preenchimento != NULL) {
        free(q -> cor_preenchimento);
    }

    q -> cor_preenchimento = my_strdup(cor_preenchimento);
}

void quadra_set_cor_borda(quadra_t *q, const char *cor_borda) {
    assert(q != NULL && cor_borda != NULL);

    if (q -> cor_borda != NULL) {
        free(q -> cor_borda);
    }

    q -> cor_borda = my_strdup(cor_borda);

}

void quadra_set_stroke_width(quadra_t *q, const char *stroke_width) {
    assert(q != NULL && stroke_width != NULL);

    if (q -> stroke_width != NULL) {
        free(q -> stroke_width);
    }

    q -> stroke_width = my_strdup(stroke_width);
}

double quadra_get_x(const quadra_t *q) {
    assert(q != NULL);

    return ponto_get_x(q -> p);
}

double quadra_get_y(const quadra_t *q) {
    assert(q != NULL);

    return ponto_get_y(q -> p);

}

double quadra_get_width(const quadra_t *q) {
    assert(q != NULL);

    return q -> width;
}

double quadra_get_height(const quadra_t *q) {
    assert(q != NULL);

    return q -> height;
}

const char *quadra_get_cor_preenchimento(const quadra_t *q) {
    assert(q != NULL);

    return q -> cor_preenchimento;
}

const char *quadra_get_cor_borda(const quadra_t *q) {
    assert(q != NULL);

    return q -> cor_borda;
}

const char *quadra_get_stroke_width(const quadra_t *q) {
    assert(q != NULL);

    return q -> stroke_width;
}

const char *quadra_get_cep(const quadra_t *q) {
    assert(q != NULL);

    return q -> cep;
}

void quadra_set_cq(quadra_t *q, const char *stroke_width, const char *cor_preenchimento, const char *cor_borda) {
    assert(q != NULL);

    quadra_set_stroke_width(q, stroke_width);
    quadra_set_cor_borda(q, cor_borda);
    quadra_set_cor_preenchimento(q, cor_preenchimento);
}

void quadra_get_coord(char face, double *x, double *y, quadra_t *q, double numero) {
    double x_quadra = quadra_get_x(q);
    double y_quadra = quadra_get_y(q);
    double width_quadra = quadra_get_width(q);
    double height_quadra = quadra_get_height(q);

    switch (face) {
        case 'S': {
            *x = x_quadra + numero;
            *y = y_quadra;
            break;
        }
        case 'N': {
            *x = x_quadra + numero;
            *y = y_quadra + height_quadra;
            break;
        }
        case 'L': {
            *x = x_quadra;
            *y = y_quadra + numero;
            break;
        }
        case 'O': {
            *x = x_quadra + width_quadra;
            *y = y_quadra + numero;
            break;
        }
        default: fprintf(stderr, "Face %c inválida! (quadra.c:%d)\n", face, __LINE__);
    }
}