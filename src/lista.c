#include "../include/lista.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct stNode {
    void *data;
    struct stNode *next;
    struct stNode *prev;
} list_node_t;

typedef struct stLista {
    int tam;
    list_node_t *head;
    list_node_t *tail;
} list_t;

// ========== FUNÇÕES BASE ==========

list_t *list_init() {
    list_t *l = malloc (sizeof(list_t));
    if (l == NULL) {
        printf("LISTA: Erro ao alocar memoria para a lista!\n");
        return NULL;
    }

    l -> tam = 0;
    l -> head = NULL;
    l -> tail = NULL;

    return l;
}

int list_size(list_t *l) {
    return l -> tam;
}

bool list_is_empty(list_t *l) {
    if (l -> tam == 0 || l -> head == NULL) {
        return true;
    }

    return false;
}

void list_push_front(list_t *l, void *new_data) {
    list_node_t *new = malloc (sizeof(list_node_t));
    if (new == NULL) {
        printf("LISTA: Erro ao alocar memoria para o novo node da lista!\n");
        return;
    }

    new -> data = new_data;
    new -> next = l -> head;
    new -> prev = NULL;

    if (l -> head != NULL) {
        l -> head -> prev = new;
    } else {
        l -> tail = new;
    }

    l -> head = new;
    l -> tam++;

}

void list_push_back(list_t *l, void *new_data) {
    list_node_t *new = malloc (sizeof(list_node_t));
    if (new == NULL) {
        printf("LISTA: Erro ao alocar memoria para o novo node\n");
        return;
    }

    new -> data = new_data;
    new -> next = NULL;

   if (list_is_empty(l)) {
        new -> prev = NULL;
        l -> head = new;
        l -> tail = new;
    }

   else {
        new -> prev = l -> tail;
        l -> tail -> next = new;
        l -> tail = new;
    }

    l -> tam ++;

}

void list_insert_at(list_t *l, void *new_data, int index) {
    if (index > l -> tam || index < 0) {
        return;
    }

    if (index == 0) {
        list_push_front(l, new_data);
        return;
    }

    if (index == l -> tam) {
        list_push_back(l, new_data);
        return;
    }

    list_node_t *new = malloc (sizeof(list_node_t));
    if (new == NULL) {
        printf("LISTA: Erro ao alocar memoria o novo no da lista!\n");
        return;
    }
    new -> data = new_data;

    list_node_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }


    new -> next = current;
    new -> prev = current -> prev;

    current -> prev -> next = new;
    current -> prev = new;

    l -> tam++;

}

void *list_pop_front(list_t *l) {
    if (l == NULL) {
        return NULL;
    }

    if (l -> head == NULL) {
        printf("LISTA: Nada a remover, a lista ja esta vazia!\n");
        return NULL;
    }

    list_node_t *removed = l -> head;

    void *data = l -> head -> data;

    if (l -> head == l -> tail) {
        l -> head = NULL;
        l -> tail = NULL;
    }

    else {
        l->head = l -> head -> next;
        l->head->prev = NULL;
    }

    l -> tam--;

    free(removed);

    return data;
}

void *list_pop_back(list_t *l) {
    if (l == NULL) {
        return NULL;
    }

    if (list_is_empty(l)) {
        printf("LISTA: Nada a remover, a lista ja esta vazia!\n");
        return NULL;
    }

    list_node_t *removed = l -> tail;
    void *data = l -> tail -> data;

    if (l -> head == l -> tail) {
        l -> head = NULL;
        l -> tail = NULL;
    }

    else {
        l -> tail = l -> tail -> prev;
        l -> tail -> next = NULL;
    }

    l -> tam--;

    free(removed);

    return data;

}

void *list_remove_at(list_t *l, int index) {
    if (l == NULL || index < 0 || index >= l -> tam ) {
        return NULL;
    }

    if (index == 0) {
        return list_pop_front(l);

    }

    if (index == l -> tam - 1) {
        return list_pop_back(l);
    }

    list_node_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }

    void *data = current -> data;

    current -> prev -> next = current -> next;
    current -> next -> prev = current -> prev;

    l -> tam--;
    free(current);
    return data;

}

void list_free(list_t *l, void (*free_data)(void *data)) {
    if (l == NULL) {
        return;
    }

    list_node_t *current = l -> head;
    while (current != NULL) {
        list_node_t *next = current -> next;

        if (free_data != NULL && current -> data != NULL) {
            free_data(current -> data);
        }

        free(current);
        current = next;
    }

    free(l);
}

// ========== FUNÇÕES GET ==========

void *list_front(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> head -> data;
}

void *list_node_data(list_node_t *n) {
    if (n == NULL || n -> data == NULL) {
        return NULL;
    }

    return n -> data;
}

list_node_t *list_node_front(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> head;
}

void *list_back(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> tail -> data;
}

list_node_t *list_node_back(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> tail;
}

list_node_t *list_node_at(list_t *l, int index) {
    if (list_is_empty(l)) {
        return NULL;
    }

    if (index < 0 || index > l -> tam - 1) {
        printf("LISTA: Index inválido!\n");
        return NULL;
    }

    if (index == 0) {
        return list_node_front(l);
    }

    if (index == l -> tam - 1) {
        return list_node_back(l);
    }

    list_node_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }

    return current;
}

void *list_data_at(list_t *l, int index) {
    if (list_is_empty(l)) {
        return NULL;
    }

    if (index < 0 || index > l -> tam - 1) {
        printf("Index inválido!\n");
        return NULL;
    }

    if (index == 0) {
        return list_front(l);
    }

    if (index == l -> tam - 1) {
        return list_back(l);
    }

    list_node_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }

    return current -> data;
}

list_node_t *list_node_next(list_node_t *n) {
    if (n == NULL || n -> next == NULL) {
        return NULL;
    }

    return n -> next;
}

list_node_t *list_node_prev(list_node_t *n) {
    if (n == NULL || n -> prev == NULL) {
        return NULL;
    }

    return n -> prev;
}

list_node_t *get_head_next(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> head -> next;
}

list_node_t *get_tail_prev(list_t *l) {
    if (list_is_empty(l)) {
        return NULL;
    }

    return l -> tail -> prev;
}

// =========== FUNÇÕES AUXILIARES ===========

void *list_remove_first(list_t *l, void *key, int (*compare)(void *a, void *b)) {
    if (l == NULL) {
        return NULL;
    }

    if (l -> tam == 0) {
        printf("LISTA: A lista está vazia! Nada a remover!\n");
        return NULL;
    }

    list_node_t *current = l -> head;
    int index = 0;
    while (current != NULL) {
        if (compare(key, current -> data) == 0) {
            return list_remove_at(l, index);
        }

        current = current -> next;
        index++;
    }

    printf("LISTA: Elemento não encontrado na lista\n");
    return NULL;
}

int list_index_of(list_t *l, list_node_t *n) {
    if (l == NULL || n == NULL) {
        return -1;
    }

    int i = 0;

    list_node_t *aux = l -> head;
    while (aux != NULL && aux != n) {
        aux = list_node_next(aux);
        i++;
    }

    if (aux == NULL) {
        return -1;
    }

    return i;
}

void *list_search(list_t *l, void *key, int (*compare)(void *a, void *b)) {
    list_node_t *current = l -> head;
    while (current != NULL) {
        if (compare(key, current -> data) == 0) {
            return current -> data;
        }

        current = current -> next;
    }

    return NULL;
}

void list_print(list_t *l, void (*print_data)(void *data)) {
    if (l == NULL) {
        printf("LISTA: Ponteiro nulo passado para a funcao 'list_print'\n");
        return;
    }

    list_node_t *current = l -> head;
    while (current != NULL) {
        if (current -> data != NULL && print_data != NULL) {
            print_data(current->data);
        }

        current = current -> next;
    }
}

void list_reverse(list_t *l) {
    if (l == NULL || l -> head == NULL) {
        return;
    }

    list_node_t *current = l -> head;
    list_node_t *temp = NULL;

    while (current != NULL) {
        temp = current -> prev;

        current -> prev = current -> next;
        current -> next = temp;

        current = current->prev;
    }

    temp = l->head;
    l->head = l->tail;
    l->tail = temp;
}

void list_clear(list_t *l, void (*free_data)(void *data)) {
    if (l == NULL) {
        return;
    }

    list_node_t *current = l->head;
    while (current != NULL) {
        list_node_t *next = current->next;

        if (free_data != NULL && current->data != NULL) {
            free_data(current->data);
        }

        free(current);
        current = next;
    }

    l->head = NULL;
    l->tail = NULL;
    l->tam = 0;
}

void list_foreach(list_t *l, void (*apply)(void *data)) {
    if (l == NULL) return;
    if (list_is_empty(l)) {
            return;
    }

    list_node_t *current = l -> head;
    while (current != NULL) {
        if (apply) {
            apply(current -> data);
        }
        current = current -> next;
    }
}

void *list_find_max(list_t *l, int (*compare)(void *a, void *b)) {
    if (list_is_empty(l)) {
        return NULL;
    }

    if (l -> tam == 1) {
        return l -> head -> data;
    }

    void *max = l -> head -> data;
    list_node_t *current = l -> head -> next;
    while (current != NULL) {
        if (compare(current -> data, max) > 0) {
            max = current -> data;
        }
        current = current  -> next;
    }

    return max;
}

void *list_find_min(list_t *l, int (*compare)(void *a, void *b)) {
     if (list_is_empty(l)) {
        return NULL;
    }

    if (l -> tam == 1) {
        return l -> head -> data;
    }

    void *min = l -> head -> data;
    list_node_t *current = l -> head -> next;
    while (current != NULL) {
        if (compare(current -> data, min) < 0) {
            min = current -> data;
        }
        current = current  -> next;
    }

    return min;
}

int list_contains(list_t *l, void *key, int (*compare)(void *a, void *b)) {
    if (list_search(l, key, compare) != NULL) {
        return true;
    }

    return false;
}

void list_node_swap(list_node_t *n1, list_node_t *n2) {
    if (n1 == NULL || n2 == NULL) return;

    void *temp = n1 -> data;
    n1 -> data = n2 -> data;
    n2 -> data = temp;
}

list_t *list_filter(list_t *l, int (*predicate)(void *data)) {
    if (l == NULL) return NULL;
    if (list_is_empty(l)) return list_init();

    list_t *filtered_lista = list_init();
    if (filtered_lista == NULL) {
        return NULL;
    }

    list_node_t *current = l -> head;
    while (current != NULL) {
        if (predicate && predicate(current -> data) == 1) {
            list_push_back(filtered_lista, current -> data);
        }
        current = current -> next;
    }

    return filtered_lista;
}

void list_map(list_t *l, void (*transform)(void *data)) {
    if (l == NULL || list_is_empty(l) || transform == NULL) return;

    list_node_t *current = l -> head;

    while (current != NULL) {
        transform(current -> data);
        current = current -> next;
    }
}

int list_remove_all_if(list_t *l, int (*predicate)(void *data), void (*free_data)(void *data)) {
    if (l == NULL || predicate == NULL) return 0;

    int removidos = 0;
    list_node_t *current = l->head;
    list_node_t *next_node = NULL;

    while (current != NULL) {
        next_node = current->next;

        if (predicate(current->data)) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                l->head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                l->tail = current->prev;
            }

            if (free_data != NULL) {
                free_data(current->data);
            }
            free(current);
            removidos++;
            l->tam--;
        }
        current = next_node;
    }

    return removidos;
}

// =========== FUNÇÕES DE ORDENAÇÃO PARA LISTA ==========

void list_sort_insertion(list_t *l, int (*cmp_data)(void *a, void *b)) {
    if (list_is_empty(l) || list_size(l) <= 1) {
        return;
    }

    list_node_t *node_i = l -> head -> next;


    while (node_i != NULL) {
        list_node_t *node_j = node_i;
        list_node_t *j_prev = node_j -> prev;

        list_node_t *next_i = node_i -> next;


        while (j_prev != NULL && cmp_data(j_prev -> data, node_j -> data) > 0) {
            list_node_swap(j_prev, node_j);
            node_j = j_prev;
            j_prev = node_j -> prev;
        }

        node_i = next_i;
    }
}

static list_node_t *get_middle_lista(list_node_t *head) {
    list_node_t *slow = head;
    list_node_t *fast = head -> next;

    while (fast && fast -> next) {
        slow = slow -> next;
        fast = fast -> next -> next;
    }

    return slow;
}

static list_node_t *merge_nodes(list_node_t *right, list_node_t *left, int (*cmp)(void *a, void *b)) {
    if (left == NULL) {
        return right;
    }

    if (right == NULL) {
        return left;
    }

    list_node_t *final_head = NULL;

    if (cmp(left -> data, right -> data) <= 0) {
        final_head = left;
        left = left -> next;
    } else {
        final_head = right;
        right = right -> next;
    }

    final_head -> prev = NULL;

    list_node_t *tail = final_head;


    while (left != NULL && right != NULL) {
        if (cmp(left -> data, right -> data) <= 0) {
            tail -> next = left;
            left -> prev = tail;
            tail = left;
            left = left -> next;
        } else {
            tail -> next = right;
            right -> prev = tail;
            tail = right;
            right = right -> next;
        }
    }

    if (left != NULL) {
        tail -> next = left;
        left -> prev = tail;
    } else if (right != NULL) {
        tail -> next = right;
        right -> prev = tail;
    }

    return final_head;
}

static list_node_t *recursive_sort(list_node_t *head, int (*cmp)(void *a, void *b)) {
    if (head == NULL || head -> next == NULL) return head;

    list_node_t *mid = get_middle_lista(head);
    list_node_t *right_head = mid -> next;

    mid -> next = NULL;

    if (right_head != NULL) {
        right_head -> prev = NULL;
    }

    list_node_t *sorted_left = recursive_sort(head, cmp);
    list_node_t *sorted_right = recursive_sort(right_head, cmp);

    return merge_nodes(sorted_left, sorted_right, cmp);
}

void list_sort_merge(list_t *l, int (*cmp)(void *a, void *b)) {
    if (l == NULL || l -> tam <= 1) return;
    l -> head = recursive_sort(l -> head, cmp);

    list_node_t *tail = l -> head;

    if (tail != NULL) {
        while (tail -> next != NULL) {
            tail = tail -> next;
        }
    }

    l -> tail = tail;

}
