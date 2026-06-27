#include "../include/via_handler.h"
#include "../include/grafo.h"
#include "../include/rua.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../include/ponto.h"

static void comando_v(char *linha_lida, graph_t *g);
static void comando_e(char *linha_lida, graph_t *g);

void via_handler(char *path_via, graph_t *g) {
    assert(path_via != NULL && g != NULL);

    FILE *arquivo_via = fopen(path_via, "r");
    if (arquivo_via == NULL) {
        fprintf(stderr, "[!] Erro ao abrir o arquivo (.via): %s (via_handler.c:%d)\n", path_via, __LINE__);
        return;
    }

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

    char id[64];
    double x = 0, y = 0;

    int lidos = sscanf(linha_lida, "v %63s %lf %lf", id, &x, &y);
    if (lidos != 3) {
        fprintf(stderr, "Linha mal formatada no arquivo (.via) - comando: 'v'. (via_handler.c:%d)\n", __LINE__);
        return;
    }

    ponto_t *p = ponto_init(x, y);
    assert(p);

    graph_add_vertex(g, p, id);

}

static void comando_e(char *linha_lida, graph_t *g) {
    char src_id[64] = "",
    target_id[64] = "",
    cep_esq[64] = "",
    cep_dir[64] = "",
    nome[128] = "";
    double cmp = 0, vm = 0;

    // %[^\r\n] captura tudo (incluindo espaços) até a quebra de linha
    int lidos = sscanf(linha_lida, "e %63s %63s %63s %63s %lf %lf %127[^\r\n]", src_id, target_id, cep_dir, cep_esq, &cmp, &vm, nome);
    if (lidos != 7) {
        fprintf(stderr, "Linha mal formatada no arquivo (.via) - comando: 'e'. (via_hander.c:%d)\n", __LINE__);
        return;
    }


    rua_t *r = rua_init(cep_esq, cep_dir, nome, vm, cmp);
    assert(r);

    graph_add_edge(g, r, src_id, target_id, nome);

}


