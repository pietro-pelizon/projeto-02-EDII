#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/geo_handler.h"
#include "../include/via_handler.h"
#include "../include/qry_handler.h"
#include "../include/svg_handler.h"
#include "../include/grafo.h"
#include "../include/rua.h"
#include "../include/ponto.h"
#include "../include/utils.h"

#define PATH_SIZE 512

#define IMG_BASE64 "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"

static void monta_caminho(char *path_completo, const char *base_dir, const char *nome_arquivo);
static exhash_t *inicializa_mapa(const char *dir_entrada, const char *path_geo);
static graph_t *inicializa_grafo(const char *dir_entrada, const char *path_via);
static FILE *inicializa_svg(const char *dir_saida, const char *nome_base_geo, const char *nome_base_qry, exhash_t *quadras);
static void processa_consultas(const char *dir_entrada, const char *dir_saida,
                                const char *path_qry, const char *nome_base_geo,
                                const char *nome_base_qry, graph_t *g,
                                exhash_t *quadras, FILE *svg);
static void extrair_nome_base(const char *caminho, char *nome_base);
static void bbox_quadra_cb(void *record_data, void *context);




static void destrutor_ponto(void *data) { ponto_destroy((ponto_t *)data); }
static void destrutor_rua(void *data) { rua_destroy((rua_t *)data); }
static void destrutor_quadra(void *data) {
    quadra_t *q = *(quadra_t **)data;
    quadra_destroy(q);
}

int main(int argc, char *argv[]) {
    clock_t start = clock();


    char *path_qry = NULL,
    *path_via = NULL,
    *path_geo = NULL,
    *dir_entrada = NULL,
    *dir_saida = NULL;

    // Lê os argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            dir_entrada = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            path_geo = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            dir_saida = argv[++i];
        } else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            path_qry = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            path_via = argv[++i];
        }
    }

    // Checa parâmetros obrigatórios
    if (path_geo == NULL || dir_saida == NULL) {
        ERRO_LOG("Parâmetros obrigatórios -f <arquivo.geo> e -o <dir_saida> faltando!");
        return 1;
    }


    char nome_base_geo[PATH_SIZE] = "", nome_base_qry[PATH_SIZE] = "";
    extrair_nome_base(path_geo, nome_base_geo);
    if (path_qry != NULL) extrair_nome_base(path_qry, nome_base_qry);

    // Inicializa estruturas e arquivos necessários
    exhash_t *quadras = inicializa_mapa(dir_entrada, path_geo);
    if (quadras == NULL) return 1;

    FILE *svg = inicializa_svg(dir_saida, nome_base_geo, nome_base_qry, quadras);
    if (svg == NULL) {
        exhash_destroy(quadras, destrutor_quadra); return 1;
    }

    graph_t *g = inicializa_grafo(dir_entrada, path_via);
    if (g == NULL) {
        svg_close(svg); exhash_destroy(quadras, destrutor_quadra); return 1;
    }

    svg_desenha_mapa_base(svg, g);



    if (path_qry != NULL) {
        processa_consultas(dir_entrada, dir_saida, path_qry,
                           nome_base_geo, nome_base_qry, g, quadras, svg);
    }

    // Libera a memória utilizada
    svg_close(svg);
    graph_destroy(g);
    exhash_destroy(quadras, destrutor_quadra);

    clock_t end = clock();

    double tempo = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n[!]Tempo de execução do programa: %f segundos\n\n", tempo);


    return 0;
}

// ===============================================
// -- Implementações static --
// ===============================================

static void monta_caminho(char *path_completo, const char *base_dir, const char *nome_arquivo) {
    if (base_dir != NULL && strlen(base_dir) > 0) {
        sprintf(path_completo, "%s/%s", base_dir, nome_arquivo);
    }

    else {
        strcpy(path_completo, nome_arquivo);
    }
}

static void extrair_nome_base(const char *caminho, char *nome_base) {
    const char *ultimo_slash = strrchr(caminho, '/');

    strcpy(nome_base, ultimo_slash ? ultimo_slash + 1 : caminho);

    char *ponto_ext = strrchr(nome_base, '.');
    if (ponto_ext) {
        *ponto_ext = '\0';
    }
}

static exhash_t *inicializa_mapa(const char *dir_entrada, const char *path_geo) {
    char path_completo[PATH_SIZE];

    monta_caminho(path_completo, dir_entrada, path_geo);
    exhash_t *quadras = geo_handler(path_completo);

    if (quadras == NULL) {
        ERRO_LOG("Falha crítica ao processar o arquivo (.geo): %s", path_completo);
        return NULL;
    }

    return quadras;
}

static graph_t *inicializa_grafo(const char *dir_entrada, const char *path_via) {
    graph_t *g = graph_init(destrutor_rua, destrutor_ponto);

    if (g == NULL) {
        ERRO_LOG("Falha ao ler o arquivo (.via): '%s' e criar o grafo", path_via);
        return NULL;
    }


    if (path_via != NULL) {
        char path_completo[PATH_SIZE];
        monta_caminho(path_completo, dir_entrada, path_via);

        if (!via_handler(path_completo, g)) {
            return NULL;
        }
    }

    return g;
}

static FILE *inicializa_svg(const char *dir_saida, const char *nome_base_geo,
                             const char *nome_base_qry, exhash_t *quadras) {

    double bb[4] = { INFINITY, INFINITY, -INFINITY, -INFINITY };
    exhash_foreach(quadras, bbox_quadra_cb, bb);

    char path_svg[PATH_SIZE];
    if (nome_base_qry != NULL && strlen(nome_base_qry) > 0) {
        snprintf(path_svg, PATH_SIZE, "%s/%s-%s.svg", dir_saida, nome_base_geo, nome_base_qry);
    }

    else {
        snprintf(path_svg, PATH_SIZE, "%s/%s.svg", dir_saida, nome_base_geo);
    }

    FILE *svg = svg_init(path_svg, bb[0], bb[1], bb[2], bb[3]);
    if (svg == NULL) {
        ERRO_LOG("Não foi possível criar o arquivo (.svg): '%s'", path_svg)
        return NULL;
    }

    exhash_foreach(quadras, svg_quadra_foreach_cb, svg);
    return svg;
}

static void bbox_quadra_cb(void *record_data, void *context) {
    double *bb = (double *)context;
    quadra_t *q = *(quadra_t **)record_data;

    double x  = quadra_get_x(q);
    double y  = quadra_get_y(q);
    double x2 = x + quadra_get_width(q);
    double y2 = y + quadra_get_height(q);

    if (x  < bb[0]) bb[0] = x;
    if (y  < bb[1]) bb[1] = y;
    if (x2 > bb[2]) bb[2] = x2;
    if (y2 > bb[3]) bb[3] = y2;
}

static void processa_consultas(const char *dir_entrada, const char *dir_saida,
                                const char *path_qry, const char *nome_base_geo,
                                const char *nome_base_qry, graph_t *g,
                                exhash_t *quadras, FILE *svg) {
    char path_qry_completo[PATH_SIZE];
    monta_caminho(path_qry_completo, dir_entrada, path_qry);

    char path_txt[PATH_SIZE];
    snprintf(path_txt, PATH_SIZE, "%s/%s-%s.txt", dir_saida, nome_base_geo, nome_base_qry);

    FILE *txt = fopen(path_txt, "w");
    if (txt == NULL) {
        ERRO_LOG("Não foi possível criar %s", path_txt);
        return;
    }
    qry_handler(path_qry_completo, g, quadras, svg, txt);
    fclose(txt);
}


