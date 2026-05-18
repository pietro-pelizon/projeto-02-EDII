#include "fila.h"


#include <stdlib.h>
#include <stdio.h>


typedef struct stNode {
	void *item;
	nodef_t *prox;
}nodef_t;

typedef struct stFila {
	nodef_t *inicio;
	nodef_t *fim;
	int tam;
}fila_t;


fila_t *fila_init(void) {
	fila_t *f = malloc (sizeof(fila_t));
	if (!f) {
		printf("Erro ao criar a fila!\n");
		return NULL;
	}

	f -> inicio = NULL;
	f -> fim = NULL;
	f -> tam = 0;

	return f;

}

int fila_get_size(fila_t *f) {
	return f -> tam;
}

bool fila_is_empty(fila_t *f) {
	if (f -> tam == 0) {
		return true;
	}

	return false;
}

void enqueue(fila_t *f, void *item) {
	nodef_t *novo = malloc(sizeof(nodef_t));
	if (!novo) {
		perror("malloc node_t");
		exit(1);
	}


	novo -> item = item;
	novo -> prox = NULL;

	if (fila_is_empty(f)) {
		f -> inicio = novo;
		f -> fim = novo;
	}

	else {
		f -> fim -> prox = novo;
		f -> fim = novo;

	}

	f -> tam++;
}

nodef_t *fila_get_inicio(fila_t *f) {
	if (fila_is_empty(f)) {
		return NULL;
	}

	return f -> inicio;
}

nodef_t *fila_get_fim(fila_t *f) {
	if (fila_is_empty(f)) {
		return NULL;
	}

	return f -> fim;
}

void *dequeue(fila_t *f) {
	if (fila_is_empty(f)) {
		return NULL;
	}

	nodef_t *remover = f -> inicio;
	void *retorno = f -> inicio -> item;
	f -> inicio = f -> inicio -> prox;

	if (f -> inicio == NULL) {
		f -> fim = NULL;
	}

	f -> tam--;

	free(remover);
	return retorno;

}

void fila_destroy(fila_t *f, void (*destrutor)(void *item)) {
	if (f == NULL) return;

	nodef_t *atual = f -> inicio;
	int contador = 0;
	while (atual != NULL) {
		nodef_t *proximo = atual->prox;

		if (destrutor != NULL && atual -> item != NULL) {
			destrutor(atual->item);
			contador++;
		}

		free(atual);

		atual = proximo;
	}

	free(f);
}

void fila_copy(fila_t *principal, fila_t *copia) {
	if (fila_is_empty(principal)) {
		return;
	}

	nodef_t *aux = principal -> inicio;
	while (aux != NULL) {
		enqueue(copia, aux -> item);
		aux = aux -> prox;
	}
}

void fila_for_each(fila_t *f, void (*acao)(void *item, void *aux_data), void *aux_data) {
	if (f == NULL || acao == NULL || fila_is_empty(f)) {
		return;
	}

	nodef_t *atual = f->inicio;

	while (atual != NULL) {
		acao(atual->item, aux_data);

		atual = atual->prox;
	}
}

nodef_t* fila_get_prox_node(nodef_t *n) {
	if (n == NULL) {
		return NULL;
	}
	return n -> prox;
}

void *fila_get_node_data(nodef_t *n) {
	if (n == NULL) {
		return NULL;
	}
	return n->item;
}
