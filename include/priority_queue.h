#ifndef PROJETO_02_EDII_PRIORITY_QUEUE_H
#define PROJETO_02_EDII_PRIORITY_QUEUE_H

#include <stdbool.h>


typedef struct stNodePq pq_node_t;
typedef struct stPriorityQueue pqueue_t;

/// @brief Inicializa a Fila de Prioridade (Min-Heap)
/// @param capacidade_inicial Tamanho inicial do vetor dinâmico
/// @return Ponteiro para a fila alocada
pqueue_t *pq_init(int capacidade_inicial);

/// @brief Checa se a fila está vazia
/// @param pq Ponteiro para a fila
/// @return true se vazia, false caso contrário
bool pq_is_empty(const pqueue_t *pq);

/// @brief Insere um novo vértice na fila com sua respectiva prioridade (distância)
/// @param pq Ponteiro para a fila
/// @param id CEP/ID da quadra
/// @param priority Distância até a quadra (menor valor = maior prioridade)
void pq_enqueue(pqueue_t *pq, const char *id, double priority);

/// @brief Remove e retorna o ID do vértice com a MENOR prioridade (menor distância)
/// @param pq Ponteiro para a fila
/// @return Retorna uma cópia da string do ID (deve ser liberada com free) ou NULL se vazia
char *pq_dequeue(pqueue_t *pq);

/// @brief Libera toda a memória ocupada pela fila e seus nós internos
/// @param pq Ponteiro para a fila
void pq_destroy(pqueue_t *pq);

#endif // PROJETO_02_EDII_PRIORITY_QUEUE_H