#ifndef PROJETO_02_EDII_RUA_H
#define PROJETO_02_EDII_RUA_H


/*
 * @file rua.h
 * @brief Entidade de Negócio: Rua (Aresta do Grafo).
 *
 * @details
 * A estrutura "rua_t" encapsula as propriedades físicas e lógicas das vias da cidade,
 * como comprimento, velocidade média, nome e identificadores de cruzamentos.
 * Ela atua como o payload (dado real) armazenado dentro das arestas opacas do Grafo.
 *
 * Funcionalidades principais:
 * - Criação segura na Heap via construtores específicos.
 * - Encapsulamento de cálculo: métodos dedicados para alterar velocidades
 * (para suporte ao comando "exp") com proteção das propriedades constantes.
 * - Interface limpa via Getters estritos.
 */


/// @brief Tipo opaco que representa uma rua (aresta) no sistema viário
typedef struct stRua rua_t;

/// @brief Inicializa e aloca dinamicamente uma nova estrutura de rua
/// @param cep_esquerda CEP da quadra localizada à esquerda do segmento da rua
/// @param cep_direita CEP da quadra localizada à direita do segmento da rua
/// @param nome Nome completo da rua
/// @param vm Velocidade média permitida no trecho (m/s)
/// @param cmp Comprimento do trecho da rua (metros)
/// @return Ponteiro para a estrutura rua_t recém-criada, ou NULL em caso de falha
rua_t *rua_init(const char *cep_esquerda, const char *cep_direita, const char *nome, double vm, double cmp);

/// @brief Obtém o CEP da quadra à esquerda da rua
/// @param r Ponteiro para a rua
/// @return String contendo o CEP da quadra à esquerda
char *get_cep_esquerda(rua_t *r);

/// @brief Atualiza o CEP da quadra à esquerda da rua
/// @param r Ponteiro para a rua
/// @param novo_cep_esquerda Nova string de CEP (a função deve lidar com a cópia/alocação)
void set_cep_esquerda(rua_t *r, char *novo_cep_esquerda);

/// @brief Obtém o CEP da quadra à direita da rua
/// @param r Ponteiro para a rua
/// @return String contendo o CEP da quadra à direita
char *get_cep_direita(rua_t *r);

/// @brief Atualiza o CEP da quadra à direita da rua
/// @param r Ponteiro para a rua
/// @param novo_cep_direita Nova string de CEP (a função deve lidar com a cópia/alocação)
void set_cep_direita(rua_t *r, char *novo_cep_direita);

/// @brief Obtém o nome da rua
/// @param r Ponteiro para a rua
/// @return String contendo o nome da rua
char *get_nome(rua_t *r);

/// @brief Atualiza o nome da rua
/// @param r Ponteiro para a rua
/// @param novo_nome Nova string do nome (a função deve lidar com a cópia/alocação)
void set_nome(rua_t *r, char *novo_nome);

/// @brief Obtém a velocidade média permitida na rua
/// @param r Ponteiro para a rua
/// @return Velocidade média em m/s
double get_vm(rua_t *r);

/// @brief Atualiza a velocidade média permitida na rua
/// @param r Ponteiro para a rua
/// @param nova_vm Nova velocidade média em m/s
void set_vm(rua_t *r, double nova_vm);

/// @brief Obtém o comprimento do trecho da rua
/// @param r Ponteiro para a rua
/// @return Comprimento em metros
double get_cmp(rua_t *r);

/// @brief Atualiza o comprimento do trecho da rua
/// @param r Ponteiro para a rua
/// @param novo_cmp Novo comprimento em metros
void set_cmp(rua_t *r, double novo_cmp);

/// @brief Libera toda a memória alocada para a estrutura da rua, incluindo suas strings internas
/// @param r Ponteiro para a rua a ser destruída
void rua_destroy(rua_t *r);

#endif //PROJETO_02_EDII_RUA_H