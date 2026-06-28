#include "../include/geo_handler.h"
#include "../include/exhash.h"
#include "../include/quadra.h"

#include <string.h>
#include <stdio.h>

#include "utils.h"


static void comando_cq(const char *linha_lida, char *sw_atual, char *corp_atual, char *cor_borda_atual, int num_linha);
static void comando_q(const char *linha_lida, char *stroke_width_atual,
    const char *cor_preenchimento_atual, const char *cor_borda_atual, exhash_t *exhash_q, int num_linha);


exhash_t *geo_handler(const char *caminho_geo) {
    FILE *arquivo_geo = fopen(caminho_geo, "r");
    if (!arquivo_geo) {
        ERRO_LOG("Erro ao abrir o arquivo (.geo): %s", caminho_geo);
        return NULL;
    }

    exhash_t *exhash_quadras = exhash_init(sizeof(quadra_t *), 4096);

    char cor_preenchimento[20] = "white";
    char cor_borda[20] = "black";
    char espessura_borda[20] = "1.0px";

    char linha_leitura[256];

    int num_linha = 0;
    while (fgets(linha_leitura, sizeof(linha_leitura), arquivo_geo)) {
    num_linha++;

        char comando[3];
        sscanf(linha_leitura, "%2s", comando);

        if (strcmp(comando, "cq") == 0) {
            comando_cq(linha_leitura, espessura_borda, cor_preenchimento, cor_borda, num_linha);
        }
        else if (strcmp(comando, "q") == 0) {
            comando_q(linha_leitura, espessura_borda, cor_preenchimento, cor_borda, exhash_quadras, num_linha);
        }
    }

    fclose(arquivo_geo);

    return exhash_quadras;
}



static void comando_cq(const char *linha_lida, char *stroke_width_atual, char *cor_preenchimento_atual, char *cor_borda_atual, int num_linha) {
    if (sscanf(linha_lida, "%*s %11s %19s %19s", stroke_width_atual, cor_preenchimento_atual, cor_borda_atual) != 3) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'cq')", num_linha);
    }
}

static void comando_q(const char *linha_lida,  char *stroke_width_atual,
    const char *cor_preenchimento_atual, const char *cor_borda_atual,  exhash_t *exhash_q, int num_linha) {

    char cep_quadra[16];
    double x_quadra, y_quadra, width_quadra, height_quadra;

    if (sscanf(linha_lida, "%*s %15s %lf %lf %lf %lf", cep_quadra, &x_quadra, &y_quadra, &width_quadra, &height_quadra) != 5) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'q')", num_linha);
        return;
    }

    quadra_t *nova_quadra = quadra_init(cep_quadra, x_quadra, y_quadra, width_quadra, height_quadra);
    if (nova_quadra == NULL) return;

    quadra_set_cq(nova_quadra, stroke_width_atual, cor_preenchimento_atual, cor_borda_atual);

    exhash_insert(exhash_q, &nova_quadra, cep_quadra);
}