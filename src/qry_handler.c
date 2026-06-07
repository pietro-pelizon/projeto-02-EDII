//
// Created by Pietro on 25/05/2026.
//

#include "../include/qry_handler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "float.h"


#include "../include/exhash.h"
#include "../include/lista.h"
#include "../include/quadra.h"
#include "../include/svg_handler.h"
#include "../include/utils.h"
#include "../include/grafo.h"
#include "../include/ponto.h"
#include "../include/rua.h"

static void descobre_componente_conexo(const char *id_start, void *v_data, lista_t *adj_start, void *context);

typedef struct stRegistrador {
    char *id; // Identificador do registrador para achar na lista
    ponto_t *p; // coordenada (x, y)
}registrador_t;

// Outra struct de contexto para poder passar para nossa
// função de iterar pelos vértices do vetor, agora no comando 'regs'
typedef struct {
    double vl;
    exhash_t *visitados;    // Guarda a informação de quais vértices já foram visitados,
    graph_t *g;             // evitando passar pelo mesmo vértice duas vezes
    int qtd_componentes;
    FILE *svg;
}regs_ctx_t;

static int cmp_registradores(void *a, void *b) {
    registrador_t *reg = a;
    char *id = b;

    if (strcmp(reg -> id, id) == 0) {
        return 0;
    }

    return 1;
}

// Empacotando tudo que o graph_foreach
// precisa para atualizar a velocidade
// média das arestas na região especificada
typedef struct stContextoMvm {
    double rx, ry, rh, rw;
    double nova_vm;
    graph_t *g;
} mvm_ctx_t;

static void comando_ao(char *linha_atual, exhash_t *quadras, lista_t *registradores, FILE *svg, FILE *txt) {
    char id_reg[16], cep[32], face;
    double numero;

    // Lendo a linha atual e coletando os dados
    sscanf(linha_atual, "@o? %15s %31s %c %lf", id_reg, cep, &face, &numero);


    // Procurando a quadra para fazer os cálculos
    // a partir de seu ponto de ancoragem
    quadra_t *quadra_procurada = NULL;
    exhash_search(quadras, cep, &quadra_procurada);

    double x = 0, y = 0;

    // Pegamos a coordenada (x, y) que será salva no registrador
    quadra_get_coord(face, &x, &y, quadra_procurada, numero);

    registrador_t *reg = search_lista(registradores, id_reg, cmp_registradores);

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

        insert_head(registradores, novo_reg);

    }


    // Escrevendo info no (.txt)
    fprintf(txt, "[*] @o? %15s %31s %c %lf\n", id_reg, cep, face, numero);
    fprintf(txt, "Ponto do endereço: (%.2lf, %.2lf)\n", x, y);


    // Colocando informações visuais no (.svg)
    pos_endereco(svg, x, y, id_reg);
}


/*Atualiza a velocidade média das arestas dentro da
região (x, y, w, h) para v.*/

// Checa se determinado ponto está dentro da região especificada
static bool dentro_da_regiao(double px, double py, double rx, double ry, double rw, double rh) {
    if (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh) {
        return true;
    }
    return false;
}

static void atualiza_vm_aresta(const char *id_origem, void *vertex_data, lista_t *adjacencia, void *contexto) {
    mvm_ctx_t *ctx = contexto;
    ponto_t *pt_origem = vertex_data;

    // Checa se o esquina (vértice/pt_origem) está dentro do retângulo determinado,
    // caso não esteja, ignoramos todas as ruas (arestas) que saem dele
    if (!dentro_da_regiao(ponto_get_x(pt_origem), ponto_get_y(pt_origem), ctx -> rx, ctx -> ry, ctx -> rw, ctx -> rh)) {
        return;
    }

    // Itera sobre as arestas que saem desse vértice
    for (nodel_t *no = get_head_node(adjacencia); no != NULL; go_next_node(no)) {

        // Aresta como tipo opaco
        void *aresta = get_node_data(no);

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
        if (dentro_da_regiao(ponto_get_x(pt_destino), ponto_get_y(pt_destino), ctx -> rx, ctx -> ry, ctx -> rw, ctx -> rh)) {
            rua_t *r = edge_get_data(aresta);
            set_vm(r, ctx -> nova_vm);
        }
    }
}

static void comando_mvm(char *linha_atual, graph_t *g) {
    double nova_vm = 0, x = 0, y = 0, w = 0, h = 0;

    // Coleta os dados da linha atual
    sscanf(linha_atual, "mvm %lf %lf %lf %lf %lf", &nova_vm, &x, &y, &w, &h);

    // Prepara a wrapper (struct de contexto) com os dados
    mvm_ctx_t contexto = { x, y, w, h, nova_vm, g };

    // Varre o grafo inteiro, checa se as arestas estão
    // na região especificada e atualiza sua velocidade média
    graph_foreach_vertex(g, atualiza_vm_aresta, &contexto);
}

static void comando_regs(char *linha_atual, graph_t *g, FILE *txt, FILE *svg) {
    double vm_ignorada = 0;

    // Coleta os dados da linha atual
    sscanf(linha_atual, "regs %lf", &vm_ignorada);

    // Cria um hashmap temporário só para guardar as strings dos IDs visitados
    exhash_t *visitados = exhash_init(sizeof(int), graph_get_nv(g));

    regs_ctx_t contexto = {vm_ignorada, visitados, g, 0, svg};

    // Manda o grafo varrer TODOS os vértices
    graph_foreach_vertex(g, descobre_componente_conexo, &contexto);

    // Gravando no .txt
    fprintf(txt, "[*] regs %lf\n", vm_ignorada);
    fprintf(txt, "%d\n", contexto.qtd_componentes);

    // Destruindo hashmap temporário
    exhash_destroy(visitados);
}

// Basicamente uma operação de BFS só que com o critério
// de só considerar ruas com velocidade média ≥ vl
static void descobre_componente_conexo(const char *id_start, void *v_data, lista_t *adj_start, void *context) {
    assert(id_start != NULL && v_data != NULL && adj_start != NULL && context != NULL);

    regs_ctx_t *ctx = context;

    // Se o exhash_destroy(visitados) retorna true, o vértice está presente no hashmap, já o visitamos, então encerra a função
    if (exhash_search(ctx -> visitados, id_start, NULL)) {
        return;
    }

    // Aumenta o contador de componentes
    ctx -> qtd_componentes++;

    double min_x = DBL_MAX, min_y = DBL_MAX;
    double max_x = DBL_MIN, max_y = DBL_MIN;

    lista_t *bfs_fila = init_lista();
    assert(bfs_fila != NULL);

    // Enfileira a origem
    insert_tail(bfs_fila, (void*)id_start);

    // Marca a origem como visitada no hashmap
    int dummy = 1;
    exhash_insert(ctx -> visitados, &dummy, id_start);

    while (get_head_node(bfs_fila) != NULL) {

        // Remove o ID do início da lista
        char *id_atual = remove_head(bfs_fila);

        // Pega as coordenadas para o Bounding Box
        vertex_t *v_atual = graph_get_vertex(ctx -> g, id_atual);
        ponto_t *pt_atual = (ponto_t *)vertex_get_data(v_atual);
        double px = ponto_get_x(pt_atual);
        double py = ponto_get_y(pt_atual);

        if (px < min_x) min_x = px;
        if (px > max_x) max_x = px;
        if (py < min_y) min_y = py;
        if (py > max_y) max_y = py;


        // Pega a lista de adjacência do vértice atual
        lista_t *adj_atual = graph_get_neighbors(ctx -> g, id_atual);

        // Itera sobre todas as arestas (ruas) que saem desse vértice
        for (nodel_t *no = get_head_node(adj_atual); no != NULL; no = go_next_node(no)) {
            edge_t *aresta = get_node_data(no);
            rua_t *rua = edge_get_data(aresta);

            // Checa se a rua atende o vl especificado
            if (get_vm(rua) >= ctx -> vl) {
                const char *id_destino = edge_get_target_id(aresta);

                // Se não foi visitado, adiciona no exhash de visitados
                if (!exhash_search(ctx -> visitados, id_destino, NULL)) {
                    exhash_insert(ctx -> visitados, &dummy, id_destino);

                    insert_tail(bfs_fila, (void *)id_destino);
                }
            }
        }
    }


    free_lista(bfs_fila, free);

    // Gera uma cor para cada componente conexo
    char cor[10];
    cor_aleatoria(cor);

    // Chama função que cuida da manipulação do (.svg) para esse comando
    rect_componente_conexo(ctx -> svg, cor, min_x, min_y, max_x, max_y);
}

// Struct que será utilizada na execução
// do algoritmo de Kruskal para achar MSTs
typedef struct {
    const char *id_origem;
    const char *id_destino;
    edge_t *aresta;
    double comprimento;
} kruskal_edge_t;

typedef struct {
    kruskal_edge_t *array;
    int index;
}exp_ctx_t;

// Função de callback para algoritmo de Kruskal -
// Varre o hashmap e coloca todas as arestas (ruas) num array linear
static void extrai_arestas(const char *id_origem, void *vertex_data, lista_t *adjacent, void *context) {
    exp_ctx_t *ctx = (exp_ctx_t *)context;

    // Varre todas as ruas que saem desta esquina (vértice)
    for (nodel_t *no = get_head_node(adjacent); no != NULL; no = go_next_node(no)) {
        edge_t *aresta = (edge_t *)get_node_data(no);
        rua_t *rua = (rua_t *)edge_get_data(aresta);

        // Salva tudo na posição atual do array linear
        ctx -> array[ctx -> index].id_origem = id_origem;
        ctx -> array[ctx -> index].id_destino = edge_get_target_id(aresta);
        ctx -> array[ctx -> index].aresta = aresta;
        ctx -> array[ctx -> index].comprimento = get_cmp(rua);

        ctx -> index++;
    }
}

static int compara_cmp_arestas(const void *a, const void *b) {
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

static void comando_exp(char *linha_atual, graph_t *g, FILE *svg, FILE *txt) {
    assert (g != NULL && linha_atual != NULL);

    double vl;

    // Lendo a linha e pegando info
    sscanf(linha_atual, "exp %lf", &vl);

    // Assumindo que pela cidade ser nXn,
    // o máximo de arestas não passa 4 * total vértices
    int max_arestas = graph_get_nv(g) * 4;

    // ALocando memória para array de arestas
    kruskal_edge_t *all_arestas = malloc(max_arestas * sizeof(kruskal_edge_t));
    assert (all_arestas != NULL);

    // Passando para a struct de contexto
    exp_ctx_t contexto = {all_arestas, 0};

    // Iterando sobre o grafo e colocando as ruas no array
    graph_foreach_vertex(g, extrai_arestas, &contexto);


    // Ordena as arestas por ordem de tamanho (requerido pelo alg de Kruskal)
    qsort(contexto.array, contexto.index, sizeof(kruskal_edge_t), compara_cmp_arestas);

    exhash_t *pais = exhash_init(sizeof(char *), graph_get_nv(g));

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
            if (get_vm(rua) < vl) {

                // Se sim, aumenta sua velocidade média em 50%
                set_vm(rua, get_vm(rua) * 1.5);

                // Chama a função que pinta a aresta de vermelho no (.svg)
                linha_vermelha_exp(svg, id_origem, id_destino, g);

            }
        }
    }

    
    fprintf(txt, "[*] exp %.2lf\n", vl);

    exhash_destroy(pais);
    free(contexto.array);

}

