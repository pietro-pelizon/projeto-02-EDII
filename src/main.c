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

#define PATH_SIZE 512

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
        fprintf(stderr, "Parâmetros obrigatórios -f <arquivo.geo> e -o <dir_saida> faltando! (main.c:%d)\n", __LINE__);
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
        fprintf(stderr, "Falha crítica ao processar %s. (main.c:%d)\n", path_completo, __LINE__);
        return NULL;
    }

    return quadras;
}

static graph_t *inicializa_grafo(const char *dir_entrada, const char *path_via) {
    graph_t *g = graph_init(destrutor_rua, destrutor_ponto);

    if (g == NULL) {
        fprintf(stderr, "Falha ao criar o grafo. (main.c:%d)\n", __LINE__);
        return NULL;
    }


    if (path_via != NULL) {
        char path_completo[PATH_SIZE];
        monta_caminho(path_completo, dir_entrada, path_via);
        via_handler(path_completo, g);
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
        fprintf(stderr, "Não foi possível criar %s. (main.c:%d)\n", path_svg, __LINE__);
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
        fprintf(stderr, "Não foi possível criar %s. (main.c:%d)\n", path_txt, __LINE__);
        return;
    }
    qry_handler(path_qry_completo, g, quadras, svg, txt);
    fclose(txt);
}


