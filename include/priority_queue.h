#ifndef PROJETO_02_EDII_PRIORITY_QUEUE_H
#define PROJETO_02_EDII_PRIORITY_QUEUE_H

/*
 * @file priority_queue.h
 * @brief Módulo de Fila de Prioridade (Min-Heap).
 *
 * @details
 * Estrutura de dados otimizada para manter e extrair rapidamente elementos com base
 * em seus custos. A implementação simula uma árvore binária completa utilizando um
 * vetor dinâmico. Essencial para garantir o desempenho O(N log N) no cálculo de
 * caminhos mínimos. O módulo protege sua estrutura interna operando como um TAD opaco.
 *
 * Funcionalidades principais:
 * - Inserção e reordenação automática baseada em Min-Heap (menor distância = maior prioridade).
 * - Atualização dinâmica de custos em O(N): a função `pq_change_priority` permite
 * alterar o custo de um vértice e rebalancear a árvore em tempo de execução (crucial para o Dijkstra).
 * - Proteção de memória interna durante remoções e acessos.
 */


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

/// @brief Checa se a fila está cheia
/// @param pq Ponteiro para a fila
/// @return true se a capacidade for igual a quantidade de números armazenados e
/// false caso o contrário
bool pq_is_full(const pqueue_t *pq);

/// @brief Busca pelo item e altera sua prioridade
/// @param pq Ponteiro para a fila de prioridade
/// @param id Identificador da chave a ser buscada
/// @param new_priority Nova prioridade do item buscado
void pq_change_priority(pqueue_t *pq, const char *id, double new_priority);

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