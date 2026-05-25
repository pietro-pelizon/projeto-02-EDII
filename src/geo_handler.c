//
// Created by pietro on 24/05/2026.
//

#include "../include/geo_handler.h"

#include <string.h>

#include "../include/exhash.h"
#include "../include/quadra.h"
#include "../include/grafo.h"

static void comando_cq(const char *linha_lida, char *sw_atual, char *corp_atual, char *corb_atual);
static void comando_q(const char *linha_lida, char *sw_atual,
    const char *corp_atual, const char *corb_atual, exhash_t *exhash_q);


exhash_t *geo_handler(const char *caminho_geo, graph_t *g) {
    FILE *arquivo_geo = fopen(caminho_geo, "r");
    if (!arquivo_geo) {
        printf("Erro ao abrir %s\n", caminho_geo);
        return NULL;
    }

    exhash_t *exhash_q = exhash_init(sizeof(quadra_t *), 4096);

    char cor_preenchimento[20] = "white";
    char cor_borda[20] = "black";
    char espessura_borda[20] = "1.0px";

    char linha_leitura[256];

    while (fgets(linha_leitura, sizeof(linha_leitura), arquivo_geo)) {

        char comando[3];
        sscanf(linha_leitura, "%2s", comando);

        if (strcmp(comando, "cq") == 0) {
            comando_cq(linha_leitura, espessura_borda, cor_preenchimento, cor_borda);
        }
        else if (strcmp(comando, "q") == 0) {
            comando_q(linha_leitura, espessura_borda, cor_preenchimento, cor_borda, exhash_q);
        }
    }

    fclose(arquivo_geo);

    return exhash_q;
}



static void comando_cq(const char *linha_lida, char *sw_atual, char *corp_atual, char *corb_atual) {
    sscanf(linha_lida, "%*s %11s %19s %19s", sw_atual, corp_atual, corb_atual);
}

static void comando_q(const char *linha_lida,  char *sw_atual,
    const char *corp_atual, const char *corb_atual,  exhash_t *exhash_q) {

    char cep[16];
    double x, y, w, h;

    sscanf(linha_lida, "%*s %15s %lf %lf %lf %lf", cep, &x, &y, &w, &h);

    quadra_t *nova_quadra = quadra_init(cep, x, y, w, h);
    if (nova_quadra == NULL) return;

    quadra_set_cq(nova_quadra, sw_atual, corp_atual, corb_atual);

    exhash_insert(exhash_q, &nova_quadra, cep);
}