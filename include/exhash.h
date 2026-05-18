#ifndef EXHASH
#define EXHASH

#include <inttypes.h>
#include <stdbool.h>

/* Módulo de Hash Extensível (Extendible Hashing) em memória RAM.
 * Estrutura de dados genérica focada em inserção, busca e remoção ultrarrápidas (O(1) médio).
 * O diretório de baldes (buckets) cresce dinamicamente na memória conforme a necessidade.
 */

typedef struct stExhash exhash_t;
typedef struct stBucket bucket_t;

/// @brief Inicializa o Hash Map na memória RAM.
/// @param record_size Tamanho exato em bytes da struct ou ponteiro a ser guardado.
/// @param bucket_size Tamanho limite do balde em bytes (define a capacidade de itens por balde).
/// @return Ponteiro para o Hash Map criado ou NULL em caso de erro.
exhash_t *exhash_init(uint32_t record_size, uint32_t bucket_size);

/// @brief Insere um dado genérico no Hash Map.
/// @param map Ponteiro para o Hash Map.
/// @param data Dado a ser inserido (copiado para a estrutura interna).
/// @param key Chave alfanumérica única.
/// @return true se inserido com sucesso, false se a chave já existir.
bool exhash_insert(exhash_t *map, const void *data, const char *key);

/// @brief Busca um dado usando sua chave e faz uma cópia para o buffer de saída.
/// @param map Ponteiro para o Hash Map.
/// @param key Chave alfanumérica a ser buscada.
/// @param out_data Buffer pré-alocado pelo usuário onde o dado será copiado.
/// @return true se encontrado, false caso contrário.
bool exhash_search(const exhash_t *map, const char *key, void *out_data);

/// @brief Remove um dado do Hash Map e o devolve.
/// @param map Ponteiro para o Hash Map.
/// @param key Chave alfanumérica do dado a ser removido.
/// @return Ponteiro alocado com o dado removido (exige free() posterior) ou NULL se não achar.
void *exhash_remove(const exhash_t *map, const char *key);

/// @brief Libera toda a memória RAM utilizada pelo Hash Map (diretório, baldes e dados).
/// @param map Ponteiro para o Hash Map.
void exhash_destroy(exhash_t *map);

#endif