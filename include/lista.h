#ifndef PROJETO_02_EDII_LISTA_H
#define PROJETO_02_EDII_LISTA_H

/*
 * @file lista.h
 * @brief Estrutura de dados Lista Duplamente Encadeada.
 *
 * @details
 * Coleção linear e dinâmica projetada para servir como estrutura base do sistema,
 * operando como listas de adjacência no Grafo, bem como Pilhas ou Filas convencionais.
 * Implementada com Ponteiros Opacos e "void *", não possui conhecimento sobre
 * os dados que transporta, oferecendo máxima reutilização de código.
 *
 * Funcionalidades principais:
 * - Operações de inserção e remoção nas extremidades (front/back) em tempo O(1).
 * - Iteração bidirecional (next/previous) utilizando nós opacos.
 * - Gerenciamento automático de alocação de nós na memória Heap.
 */

#include <stdbool.h>

typedef struct stNode list_node_t;
typedef struct stLista list_t;

/* ___________________________________________________________________ */
// Ciclo de vida

/// @brief Cria e aloca dinamicamente uma lista duplamente ligada
/// @return Ponteiro para a lista criada, ou NULL em caso de falha
list_t *list_init(void);

/// @brief Libera todos os nós e a estrutura da lista
/// @param l Ponteiro para a lista
/// @param free_data Callback que sabe liberar cada dado; NULL para não liberar os dados
void list_free(list_t *l, void (*free_data)(void *data));

/// @brief Libera os nós da lista sem destruir a estrutura em si
/// @param l Ponteiro para a lista
/// @param free_data Callback que sabe liberar cada dado; NULL para não liberar os dados
void list_clear(list_t *l, void (*free_data)(void *data));

/* ___________________________________________________________________ */
// Consulta de estado

/// @brief Verifica se a lista está vazia
/// @param l Ponteiro para a lista
/// @return true se vazia, false caso contrário
bool list_is_empty(list_t *l);

/// @brief Retorna o número de elementos da lista
/// @param l Ponteiro para a lista
/// @return Quantidade de elementos
int list_size(list_t *l);

/// @brief Checa se um determinado dado está contido na lista
/// @param l Ponteiro para a lista
/// @param key Dado a ser procurado
/// @param compare Função de comparação (retorna 0 se igual)
/// @return 1 se encontrado, 0 caso contrário
int list_contains(list_t *l, void *key, int (*compare)(void *a, void *b));

/* ___________________________________________________________________ */
// Inserção

/// @brief Insere um novo dado no início da lista em O(1)
/// @param l Ponteiro para a lista
/// @param data Dado a ser inserido
void list_push_front(list_t *l, void *data);

/// @brief Insere um novo dado no fim da lista em O(1)
/// @param l Ponteiro para a lista
/// @param data Dado a ser inserido
void list_push_back(list_t *l, void *data);

/// @brief Insere um dado na posição indicada pelo índice
/// @param l Ponteiro para a lista
/// @param data Dado a ser inserido
/// @param index Posição (0-based) onde o dado será inserido
void list_insert_at(list_t *l, void *data, int index);

/* ___________________________________________________________________ */
// Remoção

/// @brief Remove e retorna o primeiro elemento da lista em O(1)
/// @param l Ponteiro para a lista
/// @return Dado removido, ou NULL se a lista estiver vazia
void *list_pop_front(list_t *l);

/// @brief Remove e retorna o último elemento da lista em O(1)
/// @param l Ponteiro para a lista
/// @return Dado removido, ou NULL se a lista estiver vazia
void *list_pop_back(list_t *l);

/// @brief Remove e retorna o elemento na posição especificada
/// @param l Ponteiro para a lista
/// @param index Índice (0-based) do elemento a ser removido
/// @return Dado removido, ou NULL se o índice for inválido
void *list_remove_at(list_t *l, int index);

/// @brief Remove e retorna a primeira ocorrência de um dado específico
/// @param l Ponteiro para a lista
/// @param key Dado a ser removido
/// @param compare Função de comparação (retorna 0 se igual)
/// @return Dado removido, ou NULL se não encontrado
void *list_remove_first(list_t *l, void *key, int (*compare)(void *a, void *b));

/// @brief Remove todos os elementos que satisfazem uma condição
/// @param l Ponteiro para a lista
/// @param predicate Função que retorna não-zero para elementos a remover
/// @param free_data Callback para liberar os dados removidos; NULL para não liberar
/// @return Quantidade de elementos removidos
int list_remove_all_if(list_t *l, int (*predicate)(void *data), void (*free_data)(void *data));

/* ___________________________________________________________________ */
// Acesso a dados (sem remoção)

/// @brief Retorna o dado do primeiro elemento sem o remover
/// @param l Ponteiro para a lista
/// @return Dado do primeiro elemento, ou NULL se vazia
void *list_front(list_t *l);

/// @brief Retorna o dado do último elemento sem o remover
/// @param l Ponteiro para a lista
/// @return Dado do último elemento, ou NULL se vazia
void *list_back(list_t *l);

/// @brief Retorna o dado no índice especificado sem remover
/// @param l Ponteiro para a lista
/// @param index Índice (0-based) do elemento desejado
/// @return Dado no índice, ou NULL se inválido
void *list_data_at(list_t *l, int index);

/// @brief Procura na lista e retorna o dado que satisfaz a comparação
/// @param l Ponteiro para a lista
/// @param key Dado a ser procurado
/// @param compare Função de comparação (retorna 0 se igual)
/// @return Dado encontrado, ou NULL se não encontrado
void *list_search(list_t *l, void *key, int (*compare)(void *a, void *b));

/// @brief Encontra o maior elemento da lista
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna positivo se a > b)
/// @return Ponteiro para o maior elemento, ou NULL se vazia
void *list_find_max(list_t *l, int (*compare)(void *a, void *b));

/// @brief Encontra o menor elemento da lista
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna negativo se a < b)
/// @return Ponteiro para o menor elemento, ou NULL se vazia
void *list_find_min(list_t *l, int (*compare)(void *a, void *b));

/* ___________________________________________________________________ */
// Acesso a nós (iteração)

/// @brief Retorna o nó do início da lista
/// @param l Ponteiro para a lista
/// @return Nó do início, ou NULL se vazia
list_node_t *list_node_front(list_t *l);

/// @brief Retorna o nó do fim da lista
/// @param l Ponteiro para a lista
/// @return Nó do fim, ou NULL se vazia
list_node_t *list_node_back(list_t *l);

/// @brief Retorna o nó na posição especificada
/// @param l Ponteiro para a lista
/// @param index Índice (0-based) do nó desejado
/// @return Nó na posição, ou NULL se inválido
list_node_t *list_node_at(list_t *l, int index);

/// @brief Retorna o índice de um nó na lista
/// @param l Ponteiro para a lista
/// @param n Nó cujo índice se deseja encontrar
/// @return Índice do nó, ou −1 se não pertencer à lista
int list_index_of(list_t *l, list_node_t *n);

/// @brief Retorna o dado contido em um nó
/// @param n Ponteiro para o nó
/// @return Dado contido no nó, ou NULL se o nó for inválido
void *list_node_data(list_node_t *n);

/// @brief Retorna o próximo nó da lista
/// @param n Ponteiro para o nó atual
/// @return Próximo nó, ou NULL se não existir
list_node_t *list_node_next(list_node_t *n);

/// @brief Retorna o nó anterior da lista
/// @param n Ponteiro para o nó atual
/// @return Nó anterior, ou NULL se não existir
list_node_t *list_node_prev(list_node_t *n);

/// @brief Troca os dados entre dois nós
/// @param a Primeiro nó
/// @param b Segundo nó
void list_node_swap(list_node_t *a, list_node_t *b);

/* ___________________________________________________________________ */
// Operações funcionais

/// @brief Aplica uma função a cada elemento da lista
/// @param l Ponteiro para a lista
/// @param apply Callback aplicado a cada dado
void list_foreach(list_t *l, void (*apply)(void *data));

/// @brief Aplica uma transformação in-place em cada elemento da lista
/// @param l Ponteiro para a lista
/// @param transform Função que modifica cada dado
void list_map(list_t *l, void (*transform)(void *data));

/// @brief Cria uma nova lista com os elementos que satisfazem uma condição
/// @param l Ponteiro para a lista original
/// @param predicate Função que retorna 1 para elementos a incluir
/// @return Nova lista filtrada (deve ser liberada pelo chamador)
list_t *list_filter(list_t *l, int (*predicate)(void *data));

/// @brief Imprime cada elemento da lista usando um callback de impressão
/// @param l Ponteiro para a lista
/// @param print_data Callback que sabe imprimir cada dado
void list_print(list_t *l, void (*print_data)(void *data));

/// @brief Inverte a ordem dos elementos na lista in-place
/// @param l Ponteiro para a lista
void list_reverse(list_t *l);

/* ___________________________________________________________________ */
// Ordenação

/// @brief Ordena a lista usando Insertion Sort — eficiente para listas pequenas ou quase ordenadas
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna positivo se a > b)
void list_sort_insertion(list_t *l, int (*compare)(void *a, void *b));

/// @brief Ordena a lista usando Merge Sort — O(n log n) garantido
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna positivo se a > b)
void list_sort_merge(list_t *l, int (*compare)(void *a, void *b));

#endif //PROJETO_02_EDII_LISTA_H