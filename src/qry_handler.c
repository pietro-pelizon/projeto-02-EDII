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

