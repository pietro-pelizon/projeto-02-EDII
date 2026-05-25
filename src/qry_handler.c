//
// Created by pietro on 25/05/2026.
//

#include "../include/qry_handler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/exhash.h"
#include "../include/lista.h"
#include "../include/quadra.h"
#include "../include/svg_handler.h"
#include "../include/utils.h"


typedef struct stRegistrador {
    char *id; // Identificador do registrador para achar na lista
    double x, y; // coordenada (x, y)
}registrador_t;

static int cmp_registradores(void *a, void *b) {
    char *id1 = a;
    char *id2 = b;


    if (strcmp(id1, id2) == 0) {
        return 0;
    }

    return 1;
}


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
        reg -> x = x;
        reg -> y = y;
        fprintf(stderr, "Registrador já existe na lista, coordenadas atualizadas.\n");
    }

    else {
        // Criando o novo registrador e atribuindo seus dados
        registrador_t * novo_reg = malloc(sizeof(registrador_t));
        assert(novo_reg);
        novo_reg -> id = my_strdup(id_reg);
        novo_reg -> x = x;
        novo_reg -> y = y;

        insert_head(registradores, novo_reg);

    }


    // Escrevendo info no (.txt)
    fprintf(txt, "[*] @o? %15s %31s %c %lf\n", id_reg, cep, face, numero);
    fprintf(txt, "Ponto do endereço: (%.2lf, %.2lf)\n", x, y);


    // Colocando informações visuais no (.svg)
    pos_endereco(svg, x, y, id_reg);
}