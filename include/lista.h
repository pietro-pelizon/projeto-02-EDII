#ifndef PROJETO_02_EDII_LISTA_H
#define PROJETO_02_EDII_LISTA_H

#include <stdbool.h>

#include "lista.h"

typedef struct stNode node;
typedef struct stLista lista;

/// @brief Cria uma lista duplamente ligada
/// @return Retorna uma lista duplamente ligada
lista *init_lista();

/// @brief Verifica se uma lista está vazia
/// @param l Ponteiro para a lista
/// @return Retorna true caso esteja vazia e false caso o contrário
bool is_empty_lista(lista *l);

/// @brief Pega e retorna o tamanho de uma lista
/// @param l Ponteiro para a lista
/// @return Retorna o tamanho atual da lista
int get_tam_lista(lista *l);

/// @brief Insere um novo dado no início da lista
/// @param l Ponteiro para a lista
/// @param new_data Dado a ser inserido
void insert_head(lista *l, void *new_data);

/// @brief Insere um novo dado no fim da lista
/// @param l Ponteiro para a lista
/// @param new_data Dado a ser inserido
void insert_tail(lista *l, void *new_data);

/// @brief Insere um novo no na posição da lista determinada pelo index
/// @param l Ponteiro para a lista
/// @param new_data Dado a ser inserido
/// @param index Posição da lista onde o dado será inserido
void index_insert(lista *l, void *new_data, int index);

/// @brief Mostra a cada nó da lista com todos os seus dados na tela
/// @param l Ponteiro para a lista
/// @param print_data Callback para a função que sabe como imprimir determinado dado na lista
void print_lista(lista *l, void (*print_data)(void *data));

/// @brief Dado certo dado, procura na lista e retorna caso encontre
/// @param l Ponteiro para a lista
/// @param key Dado que será procurado na lista
/// @param compare Função que compara o dado atual com a key
/// @return Retorna o dado caso encontrado, caso o contrário, retorna NULL
void *search_lista(lista *l, void *key, int (*compare)(void *a, void *b));

/// @brief Remove e retorna o primeiro elemento da lista
/// @param l Ponteiro para a lista
/// @return Retorna o dado removido ou NULL se a lista estiver vazia
void *remove_head(lista *l);

/// @brief Remove e retorna o último elemento da lista
/// @param l Ponteiro para a lista
/// @return Retorna o dado removido ou NULL se a lista estiver vazia
void *remove_tail(lista *l);

/// @brief Remove o elemento na posição especificada
/// @param l Ponteiro para a lista
/// @param index Índice do elemento a ser removido
/// @return Retorna o dado removido ou NULL se o índice for inválido
void *remove_index(lista *l, int index);

/// @brief Remove a primeira ocorrência de um dado específico
/// @param l Ponteiro para a lista
/// @param key Dado a ser removido
/// @param compare Função de comparação
/// @return Retorna o dado removido ou NULL se não encontrado
void *remove_first_data(lista *l, void *key, int (*compare)(void *a, void *b));

/// @brief Libera o conteúdo da lista
/// @param l Ponteiro para a lista
/// @param free_data Callback da função que sabe liberar os dados da lista
void free_lista(lista *l, void (*free_data)(void *data));

/// @brief Pega e retorna os dados contidos dentro do nó referenciado
/// @param n Ponteiro para o nó
/// @return Dados contidos no nó
void *get_node_data(node *n);

/// @brief Dado um nó, retorna o nó seguinte a ele
/// @param n Ponteiro para o nó da lista
/// @return O próximo nó da lista, NULL caso não exista
node *go_next_node(node *n);

/// @brief Dado um nó, retorna o nó anterior a ele
/// @param n Ponteiro para o nó da lista
/// @return Retorna o nó anterior da lista, NULL caso não exista
node *go_prev_node(node *n);

/// @brief Retorna o nó localizado no início da lista
/// @param l Ponteiro para a lista
/// @return O nó localizado no início da lista
node *get_head_node(lista *l);

/// @brief Retorna o dado contido no nó seguinte ao inicial;
/// @param l Ponteiro para a lista
/// @return Nó adjacente ao início da lista
node *get_head_next(lista *l);

/// @brief Retorna o nó que pertence ao fim da lista
/// @param l Ponteiro para a lista
/// @return O nó localizado no fim da lista
node *get_tail_node(lista *l);

/// @brief Retorna o nó adjacente ao fim da lista
/// @param l Ponteiro para a lista
/// @return Nó adjacente ao fim da lista
node *get_tail_prev(lista *l);

/// @brief Retorna o nó na posição indíce espeficada
/// @param l Ponteiro para a lista
/// @param index Posição na lista do nó desejado
/// @return Retorna o nó
node *get_node_index(lista *l, int index);

/// @brief Retorna o dado no índice especificado sem remover
/// @param l Ponteiro para a lista
/// @param index Índice do elemento desejado
/// @return Retorna o dado ou NULL se o índice for inválido
void *get_data_index(lista *l, int index);

/// @brief Retorna o primeiro elemento sem remover
/// @param l Ponteiro para a lista
/// @return Retorna o dado ou NULL se a lista estiver vazia
void *get_head_data(lista *l);

/// @brief Retorna o último elemento sem remover
/// @param l Ponteiro para a lista
/// @return Retorna o dado ou NULL se a lista estiver vazia
void *get_tail_data(lista *l);

/// @brief Inverte a ordem dos elementos na lista
/// @param l Ponteiro para a lista
void reverse_lista(lista *l);

/// @brief Libera os elementos de uma lista sem apagar a estatura
/// @param l Ponteiro para a lista
/// @param free_data Callback para função que sabe como liberar determinado dado
void clear_lista(lista *l, void (*free_data)(void *data));

/// @brief Aplica uma ação a cada elemento contido na lista
/// @param l Ponteiro para a lista
/// @param apply Callback para função que aplica a ação
void foreach_lista(lista *l, void (*apply)(void *data));

/// @brief Encontra o maior elemento da lista
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna positivo se a > b)
/// @return Ponteiro para o maior elemento ou NULL se vazia
void *find_max(lista *l, int (*compare)(void *a, void *b));

/// @brief Encontra o menor elemento da lista
/// @param l Ponteiro para a lista
/// @param compare Função de comparação (retorna negativo se a < b)
/// @return Ponteiro para o menor elemento ou NULL se vazia
void *find_min(lista *l, int (*compare)(void *a, void *b));

/// @brief Checa se um determinado dado está contido na lista
/// @param l Ponteiro para a lista
/// @param key Dado que será procurado na lista
/// @param compare Callback para a função que compara o tipo da lista
/// @return Retorna 1 se encontrar, 0 se não;
int contains(lista *l, void *key, int (*compare)(void *a, void *b));

/// @brief Dada uma lista com n elementos, cria uma lista nova
/// com determinada elementos que satisfazem uma condição
/// @param l Ponteiro para a lista
/// @param predicate Função que checa se determinado dado
/// satisfaz determinada condição
/// @return Nova lista com elementos filtrados
lista *filter_lista(lista *l, int (*predicate)(void *data));

/// @brief Dada uma lista com n elementos, aplica uma transformação em cada elemento
/// @param l Ponteiro para a lista
/// @param transform Função que modifica o elemento
void map_lista(lista *l, void (*transform)(void *data));

/// @brief Remove os elementos que satisfazem determinada condição
/// @param l Ponteiro para a lista
/// @param predicate Callback para a função que checa a condição
/// @param free_data Callback para a função que sabe apagar um dado
/// @return Quantidade de elementos removidos
int remove_all_if(lista *l, int (*predicate)(void *data), void (*free_data)(void *data));

/// @brief Percorre a lista até encontrar o indíce do nó
/// @param l Ponteiro para a lista
/// @param n Ponteiro para o nó que deseja encontrar o indíce
/// @return Retorna o indíce do nó
int get_index(lista *l, node *n);

/// @brief Efetua a troca do dado entre dois nós
/// @param n1 Nó 1
/// @param n2 Nó 2
void swap_node_data(node *n1, node *n2);

/// @brief Usa insertion sort para ordenar uma lista
/// @param l Ponteiro para a lista a ser ordenada
/// @param cmp_data Função que sabe como comparar o dado contido na lista
void insertion_sort_lista(lista *l, int (*cmp_data)(void *a, void *b));

/// @brief Ordena uma lista utilizando o algoritmo de Merge Sort
/// @param l Ponteiro para a lista a ser ordenada
/// @param cmp Função que definirá o parâmetro de comparação do dado
void merge_sort_lista(lista *l, int (*cmp)(void *a, void *b));

#endif //PROJETO_02_EDII_LISTA_H
