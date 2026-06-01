#include "../include/priority_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

#include "limits.h"

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
static void heapify_down(pqueue_t *pq, int current_index);
static void pq_swap_nodes(pq_node_t *a, pq_node_t *b);

pqueue_t *pq_init(int initial_capacity) {

    pqueue_t *pq = malloc(sizeof(pqueue_t));
    assert(pq != NULL);

    pq -> size = 0;

    if (initial_capacity < 0) {
        pq -> capacity = INT_MAX;
    }

    pq -> capacity = initial_capacity;

    // Aloca o vetor interno da fila com "initial_capacity" capacidade
    pq -> array = malloc(sizeof(pq_node_t) * initial_capacity);
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

bool pq_is_full(const pqueue_t *pq) {
    if (pq -> size == pq -> capacity) {
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

char *pq_dequeue(pqueue_t *pq) {
    assert(pq);

    // Se a fila estiver vazia, retorna NULL
    if (pq_is_empty(pq)) return NULL;

    // Salva o ID da primeira posição;
    char *return_id = pq -> array[0].id;

    // Sobrescreve a primeira posição com o dado na última posição
    pq -> array[0] = pq -> array[pq -> size - 1];

    pq -> size--;


    heapify_down(pq, 0);

    return return_id;

}

void pq_change_priority(pqueue_t *pq, const char *id, double new_priority) {
    assert(pq != NULL);

    // Busca pelo ID na fila
    int target_index = -1;
    for (int i = 0; i < pq -> size; i++) {
        if (strcmp(pq -> array[i].id, id) == 0) {
            target_index = i;
            break;
        }
    }

    // Se o vértice não está na fila, não faz nada
    if (target_index == -1) return;

    // Atualiza a prioridade
    double old_priority = pq -> array[target_index].priority;
    pq -> array[target_index].priority = new_priority;

    // Corrige o Min-Heap
    if (new_priority < old_priority) {
        // Se a distância encurtou, sobe na árvore
        heapify_up(pq, target_index);
    } else {
        // Se a distância aumentou, desce na árvore
        heapify_down(pq, target_index);
    }
}

void pq_destroy(pqueue_t *pq) {
    assert(pq);

    if (pq -> array == NULL) return;

    for (int i = 0; i < pq -> size; i++) {

        if (pq -> array[i].id) {
            free(pq -> array[i].id);
        }
    }

    free(pq -> array);
    free(pq);
}


// Troca nó para a inserção e remoção
static void pq_swap_nodes(pq_node_t *a, pq_node_t *b) {
    pq_node_t *temp = a;
    *a = *b;
    *b = *temp;
}

// Como a fila segue uma implementação utilizando minimum heap,
// comparamos a prioridade do nó filho com a do nó pai que está
// sempre a (i - 1)/2 posições de seu filho e, se o valor guardado
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

// Como trocamos o último nó da fila pelo nó da posição inicial,
// Agora precisamos que esse nó vá para a posição correta para
// Preservar a lógica de minimum heap da fila de prioridade
static void heapify_down(pqueue_t *pq, int current_index) {
    assert(pq);

    while ((current_index * 2) + 1 < pq -> size) {
        int left_index = (current_index * 2) + 1;
        int right_index = (current_index * 2) + 2;

        int min = left_index;

        if (right_index < pq -> size && pq -> array[right_index].priority < pq -> array[left_index].priority) {
            min = right_index;
        }

        if (pq -> array[current_index].priority <= pq -> array[min].priority) {
            break;

        }

        pq_swap_nodes(&pq -> array[current_index], &pq -> array[min]);

        current_index = min;
    }
}