#include "lista.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct stNode {
    void *data;
    struct stNode *next;
    struct stNode *prev;
} nodel_t;

typedef struct stLista {
    int tam;
    nodel_t *head;
    nodel_t *tail;
} lista_t;

// ========== FUNÇÕES BASE ==========

lista_t *init_lista() {
    lista_t *l = malloc (sizeof(lista_t));
    if (l == NULL) {
        printf("LISTA: Erro ao alocar memoria para a lista!\n");
        return NULL;
    }

    l -> tam = 0;
    l -> head = NULL;
    l -> tail = NULL;

    return l;
}

int get_tam_lista(lista_t *l) {
    return l -> tam;
}

bool is_empty_lista(lista_t *l) {
    if (l -> tam == 0 || l -> head == NULL) {
        return true;
    }

    return false;
}

void insert_head(lista_t *l, void *new_data) {
    nodel_t *new = malloc (sizeof(nodel_t));
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

void insert_tail(lista_t *l, void *new_data) {
    nodel_t *new = malloc (sizeof(nodel_t));
    if (new == NULL) {
        printf("LISTA: Erro ao alocar memoria para o novo node\n");
        return;
    }

    new -> data = new_data;
    new -> next = NULL;

   if (is_empty_lista(l)) {
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

void index_insert(lista_t *l, void *new_data, int index) {
    if (index > l -> tam || index < 0) {
        return;
    }

    if (index == 0) {
        insert_head(l, new_data);
        return;
    }

    if (index == l -> tam) {
        insert_tail(l, new_data);
        return;
    }

    nodel_t *new = malloc (sizeof(nodel_t));
    if (new == NULL) {
        printf("LISTA: Erro ao alocar memoria o novo no da lista!\n");
        return;
    }
    new -> data = new_data;

    nodel_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }


    new -> next = current;
    new -> prev = current -> prev;

    current -> prev -> next = new;
    current -> prev = new;

    l -> tam++;

}

void *remove_head(lista_t *l) {
    if (l == NULL) {
        return NULL;
    }

    if (l -> head == NULL) {
        printf("LISTA: Nada a remover, a lista ja esta vazia!\n");
        return NULL;
    }

    nodel_t *removed = l -> head;

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

void *remove_tail(lista_t *l) {
    if (l == NULL) {
        return NULL;
    }

    if (is_empty_lista(l)) {
        printf("LISTA: Nada a remover, a lista ja esta vazia!\n");
        return NULL;
    }

    nodel_t *removed = l -> tail;
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

void *remove_index(lista_t *l, int index) {
    if (l == NULL || index < 0 || index >= l -> tam ) {
        return NULL;
    }

    if (index == 0) {
        return remove_head(l);

    }

    if (index == l -> tam - 1) {
        return remove_tail(l);
    }

    nodel_t *current = l -> head;
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

void free_lista(lista_t *l, void (*free_data)(void *data)) {
    if (l == NULL) {
        return;
    }

    nodel_t *current = l -> head;
    while (current != NULL) {
        nodel_t *next = current -> next;

        if (free_data != NULL && current -> data != NULL) {
            free_data(current -> data);
        }

        free(current);
        current = next;
    }

    free(l);
}

// ========== FUNÇÕES GET ==========

void *get_head_data(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> head -> data;
}

void *get_node_data(nodel_t *n) {
    if (n == NULL || n -> data == NULL) {
        return NULL;
    }

    return n -> data;
}

nodel_t *get_head_node(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> head;
}

void *get_tail_data(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> tail -> data;
}

nodel_t *get_tail_node(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> tail;
}

nodel_t *get_node_index(lista_t *l, int index) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    if (index < 0 || index > l -> tam - 1) {
        printf("LISTA: Index inválido!\n");
        return NULL;
    }

    if (index == 0) {
        return get_head_node(l);
    }

    if (index == l -> tam - 1) {
        return get_tail_node(l);
    }

    nodel_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }

    return current;
}

void *get_data_index(lista_t *l, int index) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    if (index < 0 || index > l -> tam - 1) {
        printf("Index inválido!\n");
        return NULL;
    }

    if (index == 0) {
        return get_head_data(l);
    }

    if (index == l -> tam - 1) {
        return get_tail_data(l);
    }

    nodel_t *current = l -> head;
    for (int i = 0; i < index; i++) {
        current = current -> next;
    }

    return current -> data;
}

nodel_t *go_next_node(nodel_t *n) {
    if (n == NULL || n -> next == NULL) {
        return NULL;
    }

    return n -> next;
}

nodel_t *go_prev_node(nodel_t *n) {
    if (n == NULL || n -> prev == NULL) {
        return NULL;
    }

    return n -> prev;
}

nodel_t *get_head_next(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> head -> next;
}

nodel_t *get_tail_prev(lista_t *l) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    return l -> tail -> prev;
}

// =========== FUNÇÕES AUXILIARES ===========

void *remove_first_data(lista_t *l, void *key, int (*compare)(void *a, void *b)) {
    if (l == NULL) {
        return NULL;
    }

    if (l -> tam == 0) {
        printf("LISTA: A lista está vazia! Nada a remover!\n");
        return NULL;
    }

    nodel_t *current = l -> head;
    int index = 0;
    while (current != NULL) {
        if (compare(key, current -> data) == 0) {
            return remove_index(l, index);
        }

        current = current -> next;
        index++;
    }

    printf("LISTA: Elemento não encontrado na lista\n");
    return NULL;
}

int get_index(lista_t *l, nodel_t *n) {
    if (l == NULL || n == NULL) {
        return -1;
    }

    int i = 0;

    nodel_t *aux = l -> head;
    while (aux != NULL && aux != n) {
        aux = go_next_node(aux);
        i++;
    }

    if (aux == NULL) {
        return -1;
    }

    return i;
}

void *search_lista(lista_t *l, void *key, int (*compare)(void *a, void *b)) {
    nodel_t *current = l -> head;
    while (current != NULL) {
        if (compare(key, current -> data) == 0) {
            return current -> data;
        }

        current = current -> next;
    }

    return NULL;
}

void print_lista(lista_t *l, void (*print_data)(void *data)) {
    if (l == NULL) {
        printf("LISTA: Ponteiro nulo passado para a funcao 'print_lista'\n");
        return;
    }

    nodel_t *current = l -> head;
    while (current != NULL) {
        if (current -> data != NULL && print_data != NULL) {
            print_data(current->data);
        }

        current = current -> next;
    }
}

void reverse_lista(lista_t *l) {
    if (l == NULL || l -> head == NULL) {
        return;
    }

    nodel_t *current = l -> head;
    nodel_t *temp = NULL;

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

void clear_lista(lista_t *l, void (*free_data)(void *data)) {
    if (l == NULL) {
        return;
    }

    nodel_t *current = l->head;
    while (current != NULL) {
        nodel_t *next = current->next;

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

void foreach_lista(lista_t *l, void (*apply)(void *data)) {
    if (l == NULL) return;
    if (is_empty_lista(l)) {
            return;
    }

    nodel_t *current = l -> head;
    while (current != NULL) {
        if (apply) {
            apply(current -> data);
        }
        current = current -> next;
    }
}

void *find_max(lista_t *l, int (*compare)(void *a, void *b)) {
    if (is_empty_lista(l)) {
        return NULL;
    }

    if (l -> tam == 1) {
        return l -> head -> data;
    }

    void *max = l -> head -> data;
    nodel_t *current = l -> head -> next;
    while (current != NULL) {
        if (compare(current -> data, max) > 0) {
            max = current -> data;
        }
        current = current  -> next;
    }

    return max;
}

void *find_min(lista_t *l, int (*compare)(void *a, void *b)) {
     if (is_empty_lista(l)) {
        return NULL;
    }

    if (l -> tam == 1) {
        return l -> head -> data;
    }

    void *min = l -> head -> data;
    nodel_t *current = l -> head -> next;
    while (current != NULL) {
        if (compare(current -> data, min) < 0) {
            min = current -> data;
        }
        current = current  -> next;
    }

    return min;
}

int contains(lista_t *l, void *key, int (*compare)(void *a, void *b)) {
    if (search_lista(l, key, compare) != NULL) {
        return true;
    }

    return false;
}

void swap_node_data(nodel_t *n1, nodel_t *n2) {
    if (n1 == NULL || n2 == NULL) return;

    void *temp = n1 -> data;
    n1 -> data = n2 -> data;
    n2 -> data = temp;
}

lista_t *filter_lista(lista_t *l, int (*predicate)(void *data)) {
    if (l == NULL) return NULL;
    if (is_empty_lista(l)) return init_lista();

    lista_t *filtered_lista = init_lista();
    if (filtered_lista == NULL) {
        return NULL;
    }

    nodel_t *current = l -> head;
    while (current != NULL) {
        if (predicate && predicate(current -> data) == 1) {
            insert_tail(filtered_lista, current -> data);
        }
        current = current -> next;
    }

    return filtered_lista;
}

void map_lista(lista_t *l, void (*transform)(void *data)) {
    if (l == NULL || is_empty_lista(l) || transform == NULL) return;

    nodel_t *current = l -> head;

    while (current != NULL) {
        transform(current -> data);
        current = current -> next;
    }
}

int remove_all_if(lista_t *l, int (*predicate)(void *data), void (*free_data)(void *data)) {
    if (l == NULL || predicate == NULL) return 0;

    int removidos = 0;
    nodel_t *current = l->head;
    nodel_t *next_node = NULL;

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

void insertion_sort_lista(lista_t *l, int (*cmp_data)(void *a, void *b)) {
    if (is_empty_lista(l) || get_tam_lista(l) <= 1) {
        return;
    }

    nodel_t *node_i = l -> head -> next;


    while (node_i != NULL) {
        nodel_t *node_j = node_i;
        nodel_t *j_prev = node_j -> prev;

        nodel_t *next_i = node_i -> next;


        while (j_prev != NULL && cmp_data(j_prev -> data, node_j -> data) > 0) {
            swap_node_data(j_prev, node_j);
            node_j = j_prev;
            j_prev = node_j -> prev;
        }

        node_i = next_i;
    }
}

static nodel_t *get_middle_lista(nodel_t *head) {
    nodel_t *slow = head;
    nodel_t *fast = head -> next;

    while (fast && fast -> next) {
        slow = slow -> next;
        fast = fast -> next -> next;
    }

    return slow;
}

static nodel_t *merge_nodes(nodel_t *right, nodel_t *left, int (*cmp)(void *a, void *b)) {
    if (left == NULL) {
        return right;
    }

    if (right == NULL) {
        return left;
    }

    nodel_t *final_head = NULL;

    if (cmp(left -> data, right -> data) <= 0) {
        final_head = left;
        left = left -> next;
    } else {
        final_head = right;
        right = right -> next;
    }

    final_head -> prev = NULL;

    nodel_t *tail = final_head;


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

static nodel_t *recursive_sort(nodel_t *head, int (*cmp)(void *a, void *b)) {
    if (head == NULL || head -> next == NULL) return head;

    nodel_t *mid = get_middle_lista(head);
    nodel_t *right_head = mid -> next;

    mid -> next = NULL;

    if (right_head != NULL) {
        right_head -> prev = NULL;
    }

    nodel_t *sorted_left = recursive_sort(head, cmp);
    nodel_t *sorted_right = recursive_sort(right_head, cmp);

    return merge_nodes(sorted_left, sorted_right, cmp);
}

void merge_sort_lista(lista_t *l, int (*cmp)(void *a, void *b)) {
    if (l == NULL || l -> tam <= 1) return;
    l -> head = recursive_sort(l -> head, cmp);

    nodel_t *tail = l -> head;

    if (tail != NULL) {
        while (tail -> next != NULL) {
            tail = tail -> next;
        }
    }

    l -> tail = tail;

}
