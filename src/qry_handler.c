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

// Macro para definir tamanho do bucket independente
// do tamanho em bytes do dado
#define EXHASH_BUCKET_BYTES(record_size, n_entries) \
((sizeof(uint64_t) + (record_size)) * (n_entries) + sizeof(uint16_t) * 2)

/*------------------------------------------------------------------------------------------*/
/* ----- Structs auxiliares ----- */
/*------------------------------------------------------------------------------------------*/

// Outra struct de contexto para poder passar para nossa
// função de iterar pelos vértices do vetor, agora no comando 'regs'
typedef struct stContextoRegs {
    double vl;
    exhash_t *visitados;    // evita passar pelo mesmo vértice duas vezes
    graph_t *g;
    int qtd_componentes;
    FILE *svg;
} regs_ctx_t;

typedef struct stRegistrador {
    char *id;
    ponto_t *p;
} registrador_t;

// Empacotando tudo que o graph_foreach
// precisa para atualizar a velocidade
// média das arestas na região especificada
typedef struct stContextoMvm {
    double rx, ry, rh, rw;
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

static void comando_ao(char *linha_atual, exhash_t *quadras, list_t *registradores, FILE *svg, FILE *txt);
static void comando_mvm(char *linha_atual, graph_t *g);
static void comando_regs(char *linha_atual, graph_t *g, FILE *txt, FILE *svg);
static void comando_exp(char *linha_atual, graph_t *g, FILE *svg, FILE *txt);
static void comando_p(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, list_t *registradores);

static void descobre_componente_conexo(const char *id_start, void *v_data, list_t *adj_start, void *context);
static void desenha_caminho(FILE *svg, list_t *caminho, const char *cor_caminho, graph_t *g);
static bool is_dentro_da_regiao(double px, double py, double rx, double ry, double rw, double rh);
static int compara_comprimento_arestas(const void *a, const void *b);
static void atualiza_velocidade_media_aresta(const char *id_origem, void *vertex_data, list_t *adjacencia, void *contexto);
static void extrai_arestas(const char *id_origem, void *vertex_data, list_t *adjacent, void *context);
static int compara_registradores(void *a, void *b);
static void registrador_destroy(void *reg);


/*------------------------------------------------------------------------------------------*/
/* ----- Função principal do módulo ----- */
/*------------------------------------------------------------------------------------------*/

void qry_handler(char *path_qry, graph_t *g, exhash_t *quadras, FILE *svg, FILE *txt) {
    assert(path_qry);

    FILE *arquivo_qry = fopen(path_qry, "r");
    assert(arquivo_qry != NULL);

    list_t *registradores = list_init();

    char linha[512];

    while (fgets(linha, sizeof(linha), arquivo_qry)) {

        char comando[5] = "";
        sscanf(linha, "%4s", comando);

        if (strcmp(comando, "@o?") == 0) {
            comando_ao(linha, quadras, registradores, svg, txt);
        } else if (strcmp(comando, "mvm") == 0) {
            comando_mvm(linha, g);
        } else if (strcmp(comando, "regs") == 0) {
            comando_regs(linha, g, txt, svg);
        } else if (strcmp(comando, "exp") == 0) {
            comando_exp(linha, g, svg, txt);
        } else if (strcmp(comando, "p?") == 0) {
            comando_p(linha, g, svg, txt, registradores);
        }
    }

    list_free(registradores, registrador_destroy);
}

/*------------------------------------------------------------------------------------------*/
/* ----- Implementação das funções que fazem parsing dos comandos ----- */
/*------------------------------------------------------------------------------------------*/

static void comando_ao(char *linha_atual, exhash_t *quadras, list_t *registradores, FILE *svg, FILE *txt) {
    char id_reg[16], cep[32], face;
    double numero;

    int lidos = sscanf(linha_atual, "@o? %15s %31s %c %lf", id_reg, cep, &face, &numero);
    if (lidos != 4) {
        fprintf(stderr, "ERRO: linha mal formatada no comando '@a?'\n");
        return;
    }



    // Procurando a quadra para fazer os cálculos
    // a partir de seu ponto de ancoragem
    quadra_t *quadra_procurada = NULL;
    exhash_search(quadras, cep, &quadra_procurada);

    double x = 0, y = 0;

    // Pegamos a coordenada (x, y) que será salva no registrador
    quadra_get_coord(face, &x, &y, quadra_procurada, numero);

    registrador_t *reg = list_search(registradores, id_reg, compara_registradores);

    // Se registrador já existe, só atualiza coordenadas
    if (reg != NULL) {
        ponto_set_x(reg -> p, x);
        ponto_set_y(reg -> p, y);
        fprintf(stderr, "Registrador já existe na lista, coordenadas atualizadas.\n");
    }

    else {
        // Criando o novo registrador e atribuindo seus dados
        registrador_t *novo_reg = malloc(sizeof(registrador_t));
        assert(novo_reg);
        novo_reg -> id = my_strdup(id_reg);
        ponto_set_x(novo_reg -> p, x);
        ponto_set_y(novo_reg -> p, y);

        list_push_front(registradores, novo_reg);

    }

    // Escrevendo info no (.txt)
    fprintf(txt, "[*] @o? %15s %31s %c %lf\n", id_reg, cep, face, numero);
    fprintf(txt, "Ponto do endereço: (%.2lf, %.2lf)\n", x, y);


    // Colocando informações visuais no (.svg)
    svg_posicao_endereco(svg, x, y, id_reg);
}

static void comando_mvm(char *linha_atual, graph_t *g) {
    double nova_vm = 0, x = 0, y = 0, w = 0, h = 0;

    // Coleta os dados da linha atual
    int lidos = sscanf(linha_atual, "mvm %lf %lf %lf %lf %lf", &nova_vm, &x, &y, &w, &h);
    if (lidos != 5) {
        fprintf(stderr, "ERRO: linha mal formatada no comando 'mvm'\n");
        return;
    }

    // Prepara a wrapper (struct de contexto) com os dados
    mvm_ctx_t contexto = { x, y, w, h, nova_vm, g };

    // Varre o grafo inteiro, checa se as arestas estão
    // na região especificada e atualiza sua velocidade média
    graph_foreach_vertex(g, atualiza_velocidade_media_aresta, &contexto);
}

static void comando_regs(char *linha_atual, graph_t *g, FILE *txt, FILE *svg) {
    double vm_ignorada = 0;

    // Coleta os dados da linha atual
    int lidos = sscanf(linha_atual, "regs %lf", &vm_ignorada);
    if (lidos != 1) {
        fprintf(stderr, "ERRO: linha mal formatada no comando 'regs'\n");
        return;
    }

    // Cria um hashmap temporário só para guardar as strings dos IDs visitados
    exhash_t *visitados = exhash_init(sizeof(int), EXHASH_BUCKET_BYTES(sizeof(int), 8));

    regs_ctx_t contexto = {vm_ignorada, visitados, g, 0, svg};

    // Manda o grafo varrer TODOS os vértices
    graph_foreach_vertex(g, descobre_componente_conexo, &contexto);

    // Gravando no .txt
    fprintf(txt, "[*] regs %lf\n", vm_ignorada);
    fprintf(txt, "%d\n", contexto.qtd_componentes);

    // Destruindo hashmap temporário
    exhash_destroy(visitados, NULL);
}


static void comando_exp(char *linha_atual, graph_t *g, FILE *svg, FILE *txt) {
    assert (g != NULL && linha_atual != NULL);

    double vl;

    // Lendo a linha e pegando info
    int lidos = sscanf(linha_atual, "exp %lf", &vl);
    if (lidos != 1) {
        fprintf(stderr, "ERRO: linha mal formatada no comando 'exp'\n");
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

    
    fprintf(txt, "[*] exp %.2lf\n", vl);

    exhash_destroy(pais, NULL);
    free(contexto.array);

}

static void comando_p(char *linha_atual, graph_t *g, FILE *svg, FILE *txt, list_t *registradores) {

    char id_src[32] = "",
    id_dst[32] = "",
    cor_rapido[32] = "",
    cor_curto[32] = "";

    int lidos = sscanf(linha_atual, "p? %31s %31s %31s %31s", id_src, id_dst, cor_curto, cor_rapido);
    if (lidos != 4) {
        fprintf(stderr, "ERRO: linha mal formatada no comando 'p?'\n");
        return;
    }

    registrador_t *src = list_search(registradores, id_src, compara_registradores);
    if (!src) {
        fprintf(stderr, "Registrador de origem não encontrado!\n");
        return;
    }
    
    registrador_t *dst = list_search(registradores, id_dst, compara_registradores);
    if (!dst) {
        fprintf(stderr, "Registrador de destino não encontrado!\n");
        return;
    }
    list_t *caminho_rapido = dijkstra(g, true, src -> id, dst -> id);
    list_t *caminho_curto = dijkstra(g, false, src -> id, dst -> id);

    fprintf(txt, "[*] p? %s %s %s %s\n", id_src, id_dst, cor_curto, cor_rapido);

    if (caminho_curto == NULL || caminho_rapido == NULL) {
        fprintf(txt, "Caminho inacessível!\n");
        return;
    }

    desenha_caminho(svg, caminho_rapido, cor_rapido, g);
    desenha_caminho(svg, caminho_curto, cor_curto, g);

    svg_desenha_placas(svg, id_src, id_dst, g);

}

/*------------------------------------------------------------------------------------------*/
/* ----- Implementações static ----- */
/*------------------------------------------------------------------------------------------*/

// Basicamente uma operação de BFS só que com o critério
// de só considerar ruas com velocidade média ≥ vl
static void descobre_componente_conexo(const char *id_start, void *v_data, list_t *adj_start, void *context) {
    assert(id_start != NULL && v_data != NULL && adj_start != NULL && context != NULL);

    regs_ctx_t *ctx = context;

    // Se o exhash_destroy(visitados) retorna true, o vértice está presente no hashmap, já o visitamos, então encerra a função
    if (exhash_search(ctx -> visitados, id_start, NULL)) {
        return;
    }

    // Aumenta o contador de componentes
    ctx -> qtd_componentes++;

    double min_x = INFINITY, min_y = INFINITY;
    double max_x = -INFINITY, max_y = -INFINITY;

    list_t *bfs_fila = list_init();
    assert(bfs_fila != NULL);

    // Enfileira a origem
    list_push_back(bfs_fila, (void*)id_start);

    // Marca a origem como visitada no hashmap
    int dummy = 1;
    exhash_insert(ctx -> visitados, &dummy, id_start);

    while (list_node_front(bfs_fila) != NULL) {

        // Remove o ID do início da lista
        char *id_atual = list_pop_front(bfs_fila);

        // Pega as coordenadas para o Bounding Box
        vertex_t *v_atual = graph_get_vertex(ctx -> g, id_atual);
        ponto_t *pt_atual = vertex_get_data(v_atual);
        double px = ponto_get_x(pt_atual);
        double py = ponto_get_y(pt_atual);

        if (px < min_x) min_x = px;
        if (px > max_x) max_x = px;
        if (py < min_y) min_y = py;
        if (py > max_y) max_y = py;


        // Pega a lista de adjacência do vértice atual
        list_t *adj_atual = graph_get_neighbors(ctx -> g, id_atual);

        // Itera sobre todas as arestas (ruas) que saem desse vértice
        for (list_node_t *no = list_node_front(adj_atual); no != NULL; no = list_node_next(no)) {
            edge_t *aresta = list_node_data(no);
            rua_t *rua = edge_get_data(aresta);

            // Checa se a rua atende o vl especificado
            if (rua_get_velocidade_media(rua) >= ctx -> vl) {
                const char *id_destino = edge_get_target_id(aresta);

                // Se não foi visitado, adiciona no exhash de visitados
                if (!exhash_search(ctx -> visitados, id_destino, NULL)) {
                    exhash_insert(ctx -> visitados, &dummy, id_destino);

                    list_push_back(bfs_fila, (void *)id_destino);
                }
            }
        }
    }


    list_free(bfs_fila, free);

    char cor[10];
    gera_cor_aleatoria(cor);

    svg_rect_componente_conexo(ctx -> svg, cor, min_x, min_y, max_x, max_y);
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

static void desenha_caminho(FILE *svg, list_t *caminho, const char *cor_caminho, graph_t *g) {
    list_node_t *atual = list_node_front(caminho);

    while (atual != NULL) {
        const char *id1 = list_node_data(atual);
        const char *id2 = list_node_data(list_node_next(atual));

        svg_linha_caminho(svg, id1, id2, g, cor_caminho);

        atual = list_node_next(atual);
    }
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
    registrador_t *reg = a;
    char *id = b;

    if (strcmp(reg -> id, id) == 0) {
        return 0;
    }

    return 1;
}

static void registrador_destroy(void *reg) {
    registrador_t *r = reg;

    free(r -> id);
    ponto_destroy(r -> p);

    free(r);
}

