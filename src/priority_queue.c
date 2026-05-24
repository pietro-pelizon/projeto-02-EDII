#include "../include/priority_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"

typedef struct stNodePq{
    char *id; // CEP da quadra
    double priority; // Prioridade (menor distância)
}pq_node_t;

typedef struct stPriorityQueue{
    pq_node_t *array; // Vetor que imita o funcionamento da árvore
    int size; // Tamanho do vetor
    int capacity; // Capacidade alocada
}pqueue_t;

static void heapify_up(pqueue_t *pq, int index);
static void pq_swap_nodes(pq_node_t *a, pq_node_t *b);

pqueue_t *pq_init(int initial_capacity) {
    if (initial_capacity <= 0) return NULL;

    pqueue_t *pq = malloc(sizeof(pqueue_t));
    assert(pq != NULL);

    pq -> size = 0;
    pq -> capacity = initial_capacity;

    // Aloca o vetor interno da fila com "initial_capacity" capacidade
    pq -> array = malloc(sizeof(pqueue_t) * initial_capacity);
    assert(pq -> array != NULL);

    if (!pq -> array) {
        free(pq);
        return NULL;
    }

    return pq;
}

bool pq_is_empty(const pqueue_t *pq) {
    assert(pq != NULL);

    if (pq -> size == 0) {
        return true;
    }

    return false;
}

void pq_enqueue(pqueue_t *pq, const char *id, double priority) {
    assert(pq != NULL);

    if (pq -> size >= pq -> capacity) return;

    // A inserção ocorre sempre no final da fila
    int current_index = pq -> size;

    pq -> array[current_index].priority = priority;
    pq -> array[current_index].id = my_strdup(id);

    pq -> size++;

    // Arruma a "árvore" seguindo o padrão de minimum heap,
    // fazendo o dado ir para a posição correta no array
    heapify_up(pq, current_index);

}

// TODO
char *pq_dequeue(pqueue_t *pq);

// TODO
void pq_destroy(pqueue_t *pq);


// Troca nó para a inserção e remoção
static void pq_swap_nodes(pq_node_t *a, pq_node_t *b) {
    pq_node_t *temp = a;
    *a = *b;
    *b = *temp;
}

// Como a fila segue uma implementação utilizando minimum heap,
// comparamos a prioridade do nó filho com a do nó pai que está
// sempre a (i - 1)/2 posições de seu filho e. se o valor guardado
// no pai for maior, trocamos ele de posição. A função roda até
// o nó alcançar a raiz ou algum nó pai tiver um valor menor
static void heapify_up(pqueue_t *pq, int index) {
    assert(pq != NULL);

    while (index > 0) {
        int parent_index = (index - 1) / 2;

        if (pq -> array[parent_index].priority > pq -> array[index].priority) {
            pq_swap_nodes(&pq -> array[index], &pq -> array[parent_index]);
            index = parent_index;
        }

        else {
            break;
        }
    }
}