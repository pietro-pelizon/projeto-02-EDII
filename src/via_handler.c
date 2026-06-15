#include "../include/via_handler.h"
#include "../include/grafo.h"
#include "../include/quadra.h"
#include "../include/rua.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/exhash.h"
#include "../include/ponto.h"

static void comando_v(char *linha_lida, graph_t *g);
static void comando_e(char *linha_lida, graph_t *g);

void via_handler(char *path_via, graph_t *g) {
    assert(path_via != NULL && g != NULL);

    FILE *arquivo_via = fopen(path_via, "r");
    assert(arquivo_via);

    char linha_leitura[512];
    int nv = 0;

    fscanf(arquivo_via, "%d\n", &nv);
    graph_set_total_vertices(g, nv);

    while (fgets(linha_leitura, sizeof(linha_leitura), arquivo_via)) {


        char comando[2];
        sscanf(linha_leitura, "%1s", comando);

        if (strcmp(comando, "v") == 0) {
            comando_v(linha_leitura, g);
        }

        if (strcmp(comando, "e") == 0) {
            comando_e(linha_leitura, g);
        }
    }

    fclose(arquivo_via);
}

static void comando_v(char *linha_lida, graph_t *g) {
    assert(g);

    char id[16];
    double x, y;

    sscanf(linha_lida, "v %15s %lf %lf", id, &x, &y);

    ponto_t *p = ponto_init(x, y);
    assert(p);

    graph_add_vertex(g, p, id);

}

static void comando_e(char *linha_lida, graph_t *g) {
    char src_id[16] = "",
    target_id[16] = "",
    cep_esq[16] = "",
    cep_dir[16] = "",
    nome[128] = "";
    double cmp, vm;

    // %[^\r\n] captura tudo (incluindo espaços) até a quebra de linha
    sscanf(linha_lida, "e %15s %15s %15s %15s %lf %lf %[^\r\n]", src_id, target_id, cep_dir, cep_esq, &cmp, &vm, nome);

    rua_t *r = rua_init(cep_esq, cep_dir, nome, vm, cmp);
    assert(r);

    graph_add_edge(g, r, src_id, target_id, nome);

}


