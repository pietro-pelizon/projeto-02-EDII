#ifndef PROJETO_02_EDII_UTILS_H
#define PROJETO_02_EDII_UTILS_H
#include "exhash.h"
#include "grafo.h"

/*
 * @file utils.h
 * @brief Coleção de funções utilitárias genéricas que não caberiam em nenhum outro módulo.
 */

/// @brief Duplica uma string "src"
/// @param src String a ser duplicada
/// @return Retorna a duplicata da string
char *my_strdup(const char *src);

/// @brief Gera uma cor aleatória e salva em "buffer"
/// @param buffer Variável aonde a cor aleatória será gravada
void gera_cor_aleatoria(char *buffer);

/* ___________________________________________________________________ */

// Operações para SIMULAR um Union Find. Não há necessidade de criarmos
// um módulo novo, podemos simular o Union Find com o hashmap

/// @brief Busca a raiz (representante principal) do conjunto ao qual o vértice pertence.
/// @param pais Ponteiro para o hashmap que mapeia os pais de cada vértice.
/// @param id Identificador do vértice procurado.
/// @return O Identificador do vértice raiz do conjunto.
const char *uf_find(exhash_t *pais, const char *id);

/// @brief Une dois conjuntos distintos, conectando a raiz de um à raiz do outro.
/// @param pais Ponteiro para o hashmap de controle dos conjuntos.
/// @param id1 Identificador do primeiro vértice.
/// @param id2 Identificador do segundo vértice.
void uf_union(exhash_t *pais, const char *id1, const char *id2);

#endif //PROJETO_02_EDII_UTILS_H
