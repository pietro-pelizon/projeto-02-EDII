#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *my_strdup(const char *src) {
    char *dst = malloc(strlen (src) + 1);
    if (dst == NULL) return NULL;
    strcpy(dst, src);
    return dst;
}

void cor_aleatoria(char *buffer) {
    sprintf(buffer, "#%06X", rand() % 0XFFFFFF);
}

const char *uf_find(exhash_t *pais, const char *id) {
    const char *pai = (const char *)exhash_search(pais, id, NULL);

    // Se ele é pai dele mesmo, achamos a raiz
    if (strcmp(pai, id) == 0) {
        return id;
    }
    // Se não, sobe na árvore recursivamente
    return uf_find(pais, pai);
}

void uf_union(exhash_t *pais, const char *id1, const char *id2) {
    const char *raiz1 = uf_find(pais, id1);
    const char *raiz2 = uf_find(pais, id2);

    if (strcmp(raiz1, raiz2) != 0) {
        // Faz a raiz1 apontar para a raiz2 no hashmap
        exhash_insert(pais, (void *)raiz2, raiz1);
    }
}