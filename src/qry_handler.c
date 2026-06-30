#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qry_handler.h"
#include "../include/exhash.h"
#include "../include/lista.h"
#include "../include/quadra.h"
#include "../include/svg_handler.h"
#include "../include/utils.h"
#include "../include/grafo.h"
#include "../include/ponto.h"
#include "../include/rua.h"

/*------------------------------------------------------------------------------------------*/
/* ----- Structs auxiliares ----- */
/*------------------------------------------------------------------------------------------*/

typedef struct stRegistrador {
    char *id;
    char *id_mais_proximo;
} registrador_t;

// Empacotando tudo que o graph_foreach
// precisa para atualizar a velocidade
// média das arestas na região especificada
typedef struct stContextoMvm {
    double rx, ry, rw, rh;
    double nova_velocidade_media;
    graph_t *g;
} mvm_ctx_t;

// Struct que será utilizada na execução
// do algoritmo de Kruskal para achar MSTs
typedef struct stKruskal {
    const char *id_origem;
    const char *id_destino;
    edge_t *aresta;
    double comprimento;
} kruskal_edge_t;

typedef struct stContextoExp {
    kruskal_edge_t *array;
    int index;
} exp_ctx_t;

typedef struct stContextoMaisProximo {
    double x, y;
    double menor_dist;
    char *id_mais_proximo;
} mais_proximo_ctx_t;


static void comando_ao(char *linha_atual, graph_t *g, exhash_t *quadras, list_t *registradores, FILE *svg, FILE *txt, int num_linha);
static void comando_mvm(char *linha_atual, graph_t *g, FILE *txt, int num_linha);
static void comando_regs(char *linha_atual, graph_t *g, FILE *txt, FILE *svg, int num_linha);
static void comando_exp(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, int num_linha);
static void comando_p(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, list_t *registradores, int num_linha);

static bool is_dentro_da_regiao(double px, double py, double rx, double ry, double rw, double rh);
static int compara_comprimento_arestas(const void *a, const void *b);
static void atualiza_velocidade_media_aresta(const char *id_origem, void *vertex_data, list_t *adjacencia, void *contexto);
static void extrai_arestas(const char *id_origem, void *vertex_data, list_t *adjacent, void *context);
static int compara_registradores(void *a, void *b);
static void registrador_destroy(void *reg);
static void checa_vertice_mais_proximo(const char *id, void *vertex_data, list_t *adj, void *context);
static char *achar_vertice_mais_proximo(graph_t *g, double x, double y);
static double calcula_tempo_caminho(list_t *caminho, graph_t *g);
static void iterator_att_vm(list_t *adjacencia, mvm_ctx_t *ctx);
static void iterator_extrai_arestas(list_t *adjacent, exp_ctx_t *ctx, const char *id_origem);
static void cria_caminho(FILE *svg, list_t *caminho_curto, list_t *caminho_rapido, char *cor_rapido, char *cor_curto, graph_t *g,
    double duracao_curto, double duracao_rapido, registrador_t *src, registrador_t *dst);
static void decide_tempo(double tempo_rapido, double tempo_curto, double *duracao_rapido, double *duracao_curto);
static void iterator_exp(exp_ctx_t contexto, exhash_t *pais, double vl, graph_t *g, FILE *svg);
static bool filtro_velocidade(void *edge_data, void *context);

/*------------------------------------------------------------------------------------------*/
/* ----- Função principal do módulo ----- */
/*------------------------------------------------------------------------------------------*/

void qry_handler(char *path_qry, graph_t *g, exhash_t *quadras, FILE *svg, FILE *txt) {
    assert(path_qry);

    FILE *arquivo_qry = fopen(path_qry, "r");
    if (arquivo_qry == NULL) {
       ERRO_LOG("Erro ao abrir o arquivo (.qry): %s", path_qry);
        return;
    }

    list_t *registradores = list_init();

    char linha[512];

    int num_linha = 0;
    while (fgets(linha, sizeof(linha), arquivo_qry)) {
    num_linha++;

        char comando[5] = "";
        sscanf(linha, "%4s", comando);

        if (strcmp(comando, "@o?") == 0) {
            comando_ao(linha, g, quadras, registradores, svg, txt, num_linha);
        } else if (strcmp(comando, "mvm") == 0) {
            comando_mvm(linha, g, txt, num_linha);
        } else if (strcmp(comando, "regs") == 0) {
            comando_regs(linha, g, txt, svg, num_linha);
        } else if (strcmp(comando, "exp") == 0) {
            comando_exp(linha, g, svg, txt, num_linha);
        } else if (strcmp(comando, "p?") == 0) {
            comando_p(linha, g, svg, txt, registradores, num_linha);
        }
    }

    list_free(registradores, registrador_destroy);
    fclose(arquivo_qry);
}

/*------------------------------------------------------------------------------------------*/
/* ----- Implementação das funções que fazem parsing dos comandos ----- */
/*------------------------------------------------------------------------------------------*/

static void comando_ao(char *linha_atual, graph_t *g, exhash_t *quadras, list_t *registradores, FILE *svg, FILE *txt, int num_linha) {
    char id_reg[64], cep[64], face;
    double numero;

    if (sscanf(linha_atual, "@o? %63s %63s %c %lf", id_reg, cep, &face, &numero) != 4) {
        ERRO_LOG("Linha %d mal formatada! (comando: '@o?')", num_linha);
        return;
    }


    // Procurando a quadra para fazer os cálculos
    // a partir de seu ponto de ancoragem
    quadra_t *quadra_procurada = NULL;
    bool found = exhash_search(quadras, cep, &quadra_procurada);

    if (!found) {
        ERRO_LOG("Quadra de CEP %s não encontrada.  (comando '@o?').", cep);
        return;
    }


    double x = 0, y = 0;


    // Pegamos a coordenada (x, y) que será salva no registrador
    quadra_get_coord(face, &x, &y, quadra_procurada, numero);

    registrador_t *reg = list_search(registradores, id_reg, compara_registradores);
    char *id_vertice_mais_proximo = achar_vertice_mais_proximo(g, x, y);

    // Se registrador já existe, só atualiza coordenadas
    if (reg != NULL) {
        free(reg -> id_mais_proximo);
        reg -> id_mais_proximo = my_strdup(id_vertice_mais_proximo);
        fprintf(stdout, "Registrador já existe na lista, coordenadas atualizadas.\n");
    }

    else {
        // Criando o novo registrador e atribuindo seus dados
        registrador_t *novo_reg = malloc(sizeof(registrador_t));
        assert(novo_reg);
        novo_reg -> id = my_strdup(id_reg);
        novo_reg -> id_mais_proximo = my_strdup(id_vertice_mais_proximo);
        list_push_front(registradores, novo_reg);

    }

    // Escrevendo info no (.txt)
    fprintf(txt, "\n[*] @o? %s %s %c %.2lf\n", id_reg, cep, face, numero);
    fprintf(txt, "Ponto do endereço: (%.2lf, %.2lf)\n", x, y);


    // Colocando informações visuais no (.svg)
    svg_posicao_endereco(svg, x, y, id_reg);
}

static void comando_mvm(char *linha_atual, graph_t *g, FILE *txt, int num_linha) {
    double nova_vm = 0, x = 0, y = 0, w = 0, h = 0;

    if (sscanf(linha_atual, "mvm %lf %lf %lf %lf %lf", &nova_vm, &x, &y, &w, &h) != 5) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'mvm')", num_linha);
        return;
    }

    // Prepara a wrapper (struct de contexto) com os dados
    mvm_ctx_t contexto = { x, y, w, h, nova_vm, g };

    // Varre o grafo inteiro, checa se as arestas estão
    // na região especificada e atualiza sua velocidade média
    graph_foreach_vertex(g, atualiza_velocidade_media_aresta, &contexto);

    fprintf(txt, "\n[*] mvm %.2lf %.2lf %.2lf %.2lf %.2lf\n", nova_vm, x, y, w, h);
}

static void comando_regs(char *linha_atual, graph_t *g, FILE *txt, FILE *svg, int num_linha) {
    double vl_min = 0;

    if (sscanf(linha_atual, "regs %lf", &vl_min) != 1) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'regs')", num_linha);
        return;
    }


    // Retorna uma lista de listas

    // Desabilitando (ou ignorando) arestas com vm < vl
    // ANTES de rodar o Tarjan, conforme exigido na especificação
    // (trechos insuficientes) utilizando "filtro_velocidade"
    list_t *todos_os_sccs = tarjan(g, filtro_velocidade, &vl_min);

    int num_bairros = 0;

    // Itera sobre a lista principal (cada item é um SCC/Bairro)
    for (list_node_t *no_scc = list_node_front(todos_os_sccs); no_scc != NULL; no_scc = list_node_next(no_scc)) {
        list_t *scc_atual = list_node_data(no_scc);

        if (list_size(scc_atual) <= 0) {
            continue;
        }

        num_bairros++;

        // Inicializa os limites da Bounding Box com valores extremos
        double min_x = INFINITY, min_y = INFINITY;
        double max_x = -INFINITY, max_y = -INFINITY;
        bool tem_vertice = false;

        // Itera sobre as esquinas (IDs) DESTE bairro específico
        for (list_node_t *no_id = list_node_front(scc_atual); no_id != NULL; no_id = list_node_next(no_id)) {


            char *id_esquina = list_node_data(no_id);

            vertex_t *dado_vertice = graph_get_vertex(g, id_esquina);


            if (dado_vertice != NULL) {
                ponto_t *vertice = vertex_get_data(dado_vertice);
                double x = ponto_get_x(vertice);
                double y = ponto_get_y(vertice);

                // Atualiza a Bounding Box
                double padding = 10.0;
                if (x < min_x) min_x = x - padding;
                if (y < min_y) min_y = y - padding;
                if (x > max_x) max_x = x + padding;
                if (y > max_y) max_y = y + padding;

                tem_vertice = true;
            }
        }

        // Desenha o retângulo do bairro no SVG
        if (tem_vertice) {

            char cor[16] = "";
            gera_cor_aleatoria(cor);

            svg_rect_componente_conexo(svg, cor, min_x, min_y, max_x, max_y);
        }
    }

    fprintf(txt, "\n[*] regs %.2lf\n", vl_min);
    fprintf(txt, "Quantidade de SCCs: %d\n", num_bairros);

    for (list_node_t *no_scc = list_node_front(todos_os_sccs); no_scc != NULL; no_scc = list_node_next(no_scc)) {
        list_t *scc_atual = list_node_data(no_scc);
        list_free(scc_atual, free);
    }
    list_free(todos_os_sccs, NULL);
}

static void comando_exp(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, int num_linha) {
    assert (g != NULL && linha_atual != NULL);

    double vl = 0;

    // Lendo a linha e pegando info
    if (sscanf(linha_atual, "exp %lf", &vl) != 1) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'exp')", num_linha);
        return;
    }

    // Assumindo que pela cidade ser nXn,
    // o máximo de arestas não passa 4 * total vértices
    int max_arestas = graph_get_total_vertices(g) * 4;

    // Alocando memória para array de arestas
    kruskal_edge_t *all_arestas = malloc(max_arestas * sizeof(kruskal_edge_t));
    assert (all_arestas != NULL);

    // Passando para a struct de contexto
    exp_ctx_t contexto = {all_arestas, 0};

    // Iterando sobre o grafo e colocando as ruas no array
    graph_foreach_vertex(g, extrai_arestas, &contexto);


    // Ordena as arestas por ordem de tamanho (requerido pelo alg de Kruskal)
    qsort(contexto.array, contexto.index, sizeof(kruskal_edge_t), compara_comprimento_arestas);

    exhash_t *pais = exhash_init(sizeof(char *), EXHASH_BUCKET_BYTES(sizeof(char *), 8));

    // Itera sobre as arestas (ruas) do grafo
    iterator_exp(contexto, pais, vl, g, svg);


    fprintf(txt, "\n[*] exp %.2lf\n", vl);

    exhash_destroy(pais, NULL);
    free(contexto.array);

}

static void comando_p(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, list_t *registradores, int num_linha) {

    char id_src[64] = "",
    id_dst[64] = "",
    cor_rapido[64] = "",
    cor_curto[64] = "";

    if (sscanf(linha_atual, "p? %63s %63s %63s %63s", id_src, id_dst, cor_curto, cor_rapido) != 4) {
        ERRO_LOG("Linha %d mal formatada! (comando: 'p?')", num_linha);
        return;
    }

    registrador_t *src = list_search(registradores, id_src, compara_registradores);
    if (!src) {
        ERRO_LOG("Registrador de origem não encontrado!");
        return;
    }

    registrador_t *dst = list_search(registradores, id_dst, compara_registradores);
    if (!dst) {
        ERRO_LOG("Registrador de destino não encontrado!");
        return;
    }

    double custo_curto = 0.0, custo_rapido = 0.0;
    list_t *caminho_rapido = dijkstra(g, true, src -> id_mais_proximo, dst -> id_mais_proximo, &custo_rapido);
    list_t *caminho_curto = dijkstra(g, false, src -> id_mais_proximo, dst -> id_mais_proximo, &custo_curto);

    double tempo_curto  = calcula_tempo_caminho(caminho_curto,  g);
    double tempo_rapido = calcula_tempo_caminho(caminho_rapido, g);

    // Ancora o mais rápido em 6s, escala o mais lento proporcionalmente
    double duracao_rapido = 0.0, duracao_curto = 0.0;
    decide_tempo(tempo_rapido, tempo_curto, &duracao_rapido, &duracao_curto);



    fprintf(txt, "\n[*] p? %s %s %s %s\n", id_src, id_dst, cor_curto, cor_rapido);

    if (list_size(caminho_curto) == 0 || list_size(caminho_rapido) == 0) {
        fprintf(txt, "Caminho inacessível!\n");
    }

    else {
        cria_caminho(svg, caminho_curto, caminho_rapido, cor_rapido, cor_curto, g,
            duracao_curto, duracao_rapido, src, dst);
        }


    list_free(caminho_rapido, free);
    list_free(caminho_curto, free);

}

/*------------------------------------------------------------------------------------------*/
/* ----- Implementações static ----- */
/*------------------------------------------------------------------------------------------*/

static void iterator_exp(exp_ctx_t contexto, exhash_t *pais, double vl, graph_t *g, FILE *svg) {
    for (int i = 0; i < contexto.index; i++) {
        const char *id_origem = contexto.array[i].id_origem;
        const char *id_destino = contexto.array[i].id_destino;

        // Checa se src e dst já estão interligadas
        if (strcmp(uf_find(pais, id_origem), uf_find(pais, id_destino)) != 0) {

            // Conecta src e dst (coloca na MST)
            uf_union(pais, id_origem, id_destino);

            rua_t *rua = edge_get_data(contexto.array[i].aresta);

            // Checa se a rua atende o requisito de vl
            if (rua_get_velocidade_media(rua) < vl) {

                // Se sim, aumenta sua velocidade média em 50%
                rua_set_velocidade_media(rua, rua_get_velocidade_media(rua) * 1.5);

                // Chama a função que pinta a aresta de vermelho no (.svg)
                svg_linha_caminho(svg, id_origem, id_destino, g, "red");

            }
        }
    }
}

static void decide_tempo(double tempo_rapido, double tempo_curto, double *duracao_rapido, double *duracao_curto) {
    if (tempo_rapido <= tempo_curto) {
        *duracao_rapido = 6.0;
       *duracao_curto  = 6.0 * (tempo_curto / tempo_rapido);
    } else {
        *duracao_curto  = 6.0;
        *duracao_rapido = 6.0 * (tempo_rapido / tempo_curto);
    }
}

static void cria_caminho(FILE *svg, list_t *caminho_curto, list_t *caminho_rapido, char *cor_rapido, char *cor_curto, graph_t *g,
    double duracao_curto, double duracao_rapido, registrador_t *src, registrador_t *dst) {

    static int p_contador = 0;
    p_contador++;

    char id_curto[32], id_rapido[32];
    snprintf(id_curto,  sizeof(id_curto),  "path_curto_%d",  p_contador);
    snprintf(id_rapido, sizeof(id_rapido), "path_rapido_%d", p_contador);

    svg_anima_caminho(svg, caminho_curto,  g, id_curto, duracao_curto, cor_curto);
    svg_anima_caminho(svg, caminho_rapido, g, id_rapido, duracao_rapido, cor_rapido);
    svg_desenha_placas(svg, src -> id_mais_proximo, dst -> id_mais_proximo, g);
}

static void atualiza_velocidade_media_aresta(const char *id_origem, void *vertex_data, list_t *adjacencia, void *contexto) {

    // Diz ao compilador que o parâmetro está sendo ignorado.

    // @note: não posso retirar, pois a assinatura da função precisa
    // ser compatível com o callback da função "graph_foreach_vertex"

    (void)id_origem;

    mvm_ctx_t *ctx = contexto;
    ponto_t *pt_origem = vertex_data;

    // Checa se o esquina (vértice/pt_origem) está dentro do retângulo determinado,
    // caso não esteja, ignoramos todas as ruas (arestas) que saem dele
    if (!is_dentro_da_regiao(ponto_get_x(pt_origem), ponto_get_y(pt_origem), ctx -> rx, ctx -> ry, ctx -> rw, ctx -> rh)) {
        return;
    }


    // Itera sobre as arestas que saem desse vértice

    iterator_att_vm(adjacencia, ctx);


}

static void iterator_att_vm(list_t *adjacencia, mvm_ctx_t *ctx) {
    for (list_node_t *no = list_node_front(adjacencia); no != NULL; no = list_node_next(no)) {

        // Aresta como tipo opaco
        void *aresta = list_node_data(no);

        // Descobre para onde a aresta vai
        const char *id_destino = edge_get_target_id(aresta);

        // Pega o vértice de destino
        void *v_destino = graph_get_vertex(ctx -> g, id_destino);

        // Se não existir, pula a iteração
        if (!v_destino) continue;

        // Pega as coordenadas do vértice de destino
        ponto_t *pt_destino = vertex_get_data(v_destino);

        // Se o ponto de destino da aresta estiver dentro do retângulo, ela está
        // totalmente contida nele, então alteramos sua velocidade média
        if (is_dentro_da_regiao(ponto_get_x(pt_destino), ponto_get_y(pt_destino), ctx -> rx, ctx -> ry, ctx -> rw, ctx -> rh)) {
            rua_t *r = edge_get_data(aresta);
            rua_set_velocidade_media(r, ctx -> nova_velocidade_media);
        }
    }
}

// Varre o hashmap e coloca todas as arestas (ruas) num array linear
static void extrai_arestas(const char *id_origem, void *vertex_data, list_t *adjacent, void *context) {
    exp_ctx_t *ctx = context;

    // Diz ao compilador que o parâmetro está sendo ignorado.

    // @note: não posso retirar, pois a assinatura da função precisa
    // ser compatível com o callback da função "graph_foreach_vertex"
    (void)vertex_data;

    // Varre todas as ruas que saem desta esquina (vértice)
    iterator_extrai_arestas(adjacent, ctx, id_origem);

}

static void iterator_extrai_arestas(list_t *adjacent, exp_ctx_t *ctx, const char *id_origem) {
    for (list_node_t *no = list_node_front(adjacent); no != NULL; no = list_node_next(no)) {
        edge_t *aresta = list_node_data(no);
        rua_t *rua = edge_get_data(aresta);

        // salva tudo na posição atual do array linear
        ctx -> array[ctx -> index].id_origem = id_origem;
        ctx -> array[ctx -> index].id_destino = edge_get_target_id(aresta);
        ctx -> array[ctx -> index].aresta = aresta;
        ctx -> array[ctx -> index].comprimento = rua_get_comprimento(rua);

        ctx -> index++;
    }
}

static bool is_dentro_da_regiao(double px, double py, double rx, double ry, double rw, double rh) {
    if (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh) {
        return true;
    }
    return false;
}

static int compara_comprimento_arestas(const void *a, const void *b) {
    kruskal_edge_t *k1 = (kruskal_edge_t *)a;
    kruskal_edge_t *k2 = (kruskal_edge_t *)b;

    if (k1 -> comprimento > k2 -> comprimento) {
        return 1;
    }

    if (k1 -> comprimento < k2 -> comprimento) {
        return -1;
    }

    return 0;
}

static int compara_registradores(void *a, void *b) {
    char *id = a;
    registrador_t *reg = b;

    if (strcmp(reg -> id, id) == 0) {
        return 0;
    }

    return 1;
}

static void registrador_destroy(void *reg) {
    registrador_t *r = reg;

    free(r -> id);
    free(r -> id_mais_proximo);

    free(r);
}

static void checa_vertice_mais_proximo(const char *id, void *vertex_data, list_t *adj, void *context) {
    (void)adj;
    mais_proximo_ctx_t *ctx = context;
    ponto_t *p = vertex_data;

    double dx = ponto_get_x(p) - ctx -> x;
    double dy = ponto_get_y(p) - ctx -> y;
    double dist = dx * dx + dy * dy; // sem sqrt, só para comparação

    if (dist < ctx -> menor_dist) {
        ctx -> menor_dist = dist;
        ctx -> id_mais_proximo = (char *)id;
    }
}

static char *achar_vertice_mais_proximo(graph_t *g, double x, double y) {
    mais_proximo_ctx_t ctx = { x, y, INFINITY, NULL };
    graph_foreach_vertex(g, checa_vertice_mais_proximo, &ctx);
    return ctx.id_mais_proximo;  // aponta para o id interno do grafo — não liberar
}

static double calcula_tempo_caminho(list_t *caminho, graph_t *g) {
    double total = 0.0;
    list_node_t *no = list_node_front(caminho);

    while (no != NULL && list_node_next(no) != NULL) {
        const char *id1 = list_node_data(no);
        const char *id2 = list_node_data(list_node_next(no));

        edge_t *aresta = graph_get_edge(g, id1, id2);
        if (aresta != NULL) {
            rua_t *rua = edge_get_data(aresta);
            total += rua_get_comprimento(rua) / rua_get_velocidade_media(rua);
        }

        no = list_node_next(no);
    }


    return total;
}

static bool filtro_velocidade(void *edge_data, void *context) {
    rua_t *rua = (rua_t *)edge_data;
    double *vl_min = (double *)context;

    return rua_get_velocidade_media(rua) >= *vl_min;
}