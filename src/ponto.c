#include "../include/ponto.h"

#include <stdio.h>
#include <stdlib.h>


typedef struct stPonto {
    double x, y;
} ponto_t;

ponto_t *ponto_init(double x, double y) {
    ponto_t *p = malloc (sizeof(ponto_t));
    if (p == NULL) {
        printf("PONTO: Erro ao alocar memória para o ponto!\n");
        return NULL;
    }

    p -> x = x;
    p -> y = y;

    return p;
}

double ponto_get_x(ponto_t *p) {
    return p -> x;
}

double ponto_get_y(ponto_t *p) {
    return p -> y;
}

void ponto_set_x(ponto_t *p, double x) {
    p -> x = x;
}

void ponto_set_y(ponto_t *p, double y) {
    p -> y = y;
}

void ponto_destroy(ponto_t *p) {
    if (p) free(p);
}

ponto_t *ponto_copia(ponto_t *p) {
    ponto_t *copia = malloc (sizeof(ponto_t));
    if (!copia) {
        return NULL;
    }


    copia -> x = p -> x;
    copia -> y = p -> y;

    return copia;
}