#include "../include/via_handler.h"
#include "../include/grafo.h"
#include "../include/rua.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#include "../include/ponto.h"

static void comando_v(char *linha_lida, graph_t *g, int num_linha);
static void comando_e(char *linha_lida, graph_t *g, int num_linha);

bool via_handler(char *path_via, graph_t *g) {
    assert(path_via != NULL && g != NULL);

    FILE *arquivo_via = fopen(path_via, "r");
    if (arquivo_via == NULL) {
        ERRO_LOG("Erro ao abrir o arquivo (.via): %s", path_via);
        return false;
    }

    char linha_leitura[512];
    int nv = 0;

    fscanf(arquivo_via, "%d\n", &nv);
    graph_set_total_vertices(g, nv);

    int num_linha = 1;
    while (fgets(linha_leitura, sizeof(linha_leitura), arquivo_via)) {
        num_linha++;

        char comando[2];
        sscanf(linha_leitura, "%1s", comando);

        if (strcmp(comando, "v") == 0) {
            comando_v(linha_leitura, g, num_linha);
        }

        if (strcmp(comando, "e") == 0) {
            comando_e(linha_leitura, g, num_linha);
        }
    }

    fclose(arquivo_via);
    return true;
}

static void comando_v(char *linha_lida, graph_t *g, int num_linha) {
    assert(g);

    char id[64];
    double x = 0, y = 0;

    if (sscanf(linha_lida, "v %63s %lf %lf", id, &x, &y) != 3) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'v')", num_linha);
        return;
    }

    ponto_t *p = ponto_init(x, y);
    assert(p);

    graph_add_vertex(g, p, id);

}

static void comando_e(char *linha_lida, graph_t *g, int num_linha   ) {
    char src_id[64] = "",
    target_id[64] = "",
    cep_esq[64] = "",
    cep_dir[64] = "",
    nome[128] = "";
    double cmp = 0, vm = 0;

    // %[^\r\n] captura tudo (incluindo espaços) até a quebra de linha
    if (sscanf(linha_lida, "e %63s %63s %63s %63s %lf %lf %127[^\r\n]", src_id, target_id, cep_dir, cep_esq, &cmp, &vm, nome) != 7) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'e'", num_linha);
        return;
    }


    rua_t *r = rua_init(cep_esq, cep_dir, nome, vm, cmp);
    assert(r);

    graph_add_edge(g, r, src_id, target_id, nome);

}
