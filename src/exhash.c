#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/exhash.h"

// ============== ESTRUTURAS ==============

typedef struct stBucket {
    uint16_t local_depth;
    uint16_t record_count;
    uint8_t *data;
} bucket_t;

typedef struct stExhash {
    bucket_t **directory;
    uint32_t record_size;
    uint32_t bucket_size;
    uint8_t global_depth;
} exhash_t;

// ============== PROTÓTIPOS ESTÁTICOS ==============

static bool exhash_contains(const exhash_t *map, const char *key);
static uint64_t murmurhash3_64(const void *key, size_t len, uint32_t seed);
static uint64_t exhash_capacity(const exhash_t *map);
static bucket_t *new_empty_bucket(uint32_t bucket_size, uint32_t record_size, uint16_t depth);
static void free_bucket(bucket_t *b);
static bool insert_raw_into_bucket(const exhash_t *map, bucket_t *b, uint64_t numeric_key, const void *data);
static bool insert_into_bucket(const exhash_t *map, bucket_t *b, const char *key, const void *data);
static uint64_t get_directory_index(const exhash_t *map, const char *key);
static void expand_directory(exhash_t *map);
static void redistribute_records(const exhash_t *map, bucket_t *old_b, bucket_t *new_b, uint32_t old_count);
static void update_directory_pointers(exhash_t *map, bucket_t *old_b, bucket_t *new_b, uint8_t depth);
static void split_bucket(exhash_t *map, bucket_t *old_b);

static inline uint64_t dir_size(const exhash_t *map) {
    return (uint64_t)1 << map -> global_depth;
}

static inline uint64_t dir_mask(const exhash_t *map) {
    return dir_size(map) - 1;
}

static inline uint64_t slot_size(const exhash_t *map) {
    return sizeof(uint64_t) + map -> record_size;
}

// ============== FUNÇÕES PRINCIPAIS ==============

exhash_t *exhash_init(uint32_t record_size, uint32_t bucket_size) {
    exhash_t *map = malloc(sizeof(exhash_t));
    assert(map != NULL);

    map -> record_size = record_size;
    map -> bucket_size = bucket_size;
    map -> global_depth = 0;

    map -> directory = malloc(sizeof(bucket_t *));
    assert(map -> directory != NULL);

    map -> directory[0] = new_empty_bucket(bucket_size, record_size, 0);
    assert(map -> directory[0] != NULL);

    return map;
}

bool exhash_insert(exhash_t *map, const void *data, const char *key) {
    assert(map != NULL && data != NULL && key != NULL);

    if (exhash_contains(map, key)) {
        return false;
    }

    while (true) {
        uint64_t idx = get_directory_index(map, key);
        bucket_t *b = map -> directory[idx];

        if (b -> record_count < exhash_capacity(map)) {
            return insert_into_bucket(map, b, key, data);
        }

        if (b -> local_depth == map -> global_depth) {
            expand_directory(map);
        }

        split_bucket(map, b);
    }
}

bool exhash_search(const exhash_t *map, const char *key, void *out_data) {
    assert(map != NULL && key != NULL);

    uint64_t hashed_key = murmurhash3_64(key, strlen(key), 0);
    uint64_t idx = hashed_key & dir_mask(map);
    bucket_t *b = map -> directory[idx];
    uint64_t sz = slot_size(map);

    for (uint16_t i = 0; i < b -> record_count; i++) {
        uint64_t stored_key;
        memcpy(&stored_key, b -> data + i * sz, sizeof(uint64_t));

        if (stored_key == hashed_key) {
            if (out_data != NULL) {
                memcpy(out_data, b -> data + i * sz + sizeof(uint64_t), map -> record_size);
            }
                return true;
        }
    }

    return false;
}

void *exhash_remove(const exhash_t *map, const char *key) {
    if (map == NULL || key == NULL) return NULL;

    uint64_t hashed_key = murmurhash3_64(key, strlen(key), 0);
    uint64_t idx = hashed_key & dir_mask(map);
    bucket_t *b = map -> directory[idx];
    uint64_t sz = slot_size(map);

    for (uint16_t i = 0; i < b -> record_count; i++) {
        uint64_t stored_key;
        memcpy(&stored_key, b -> data + i * sz, sizeof(uint64_t));

        if (stored_key != hashed_key) continue;

        void *removed = malloc(map -> record_size);
        assert(removed != NULL);

        memcpy(removed, b -> data + i * sz + sizeof(uint64_t), map -> record_size);

        uint16_t last = b -> record_count - 1;
        if (i < last) {
            memcpy(b -> data + i * sz, b -> data + last * sz, sz);
        }

        b -> record_count--;
        return removed;
    }

    return NULL;
}

void exhash_foreach(const exhash_t *map, void (*action)(void *data, void *context), void *context) {
    if (!map || !action) return;

    uint64_t size = dir_size(map);
    bucket_t **visited = calloc(size, sizeof(bucket_t *));
    uint64_t n_visited = 0;
    uint64_t sz = slot_size(map);

    for (uint64_t i = 0; i < size; i++) {
        bucket_t *b = map -> directory[i];
        bool already = false;

        for (uint64_t v = 0; v < n_visited; v++) {
            if (visited[v] == b) { already = true; break; }
        }

        if (!already) {
            visited[n_visited++] = b;

            // Itera pelos registros dentro do balde único
            for (uint16_t r = 0; r < b -> record_count; r++) {
                void *record_data = b -> data + r * sz + sizeof(uint64_t);
                action(record_data, context);
            }
        }
    }
    free(visited);
}

void exhash_destroy(exhash_t *map) {
    if (map == NULL) return;

    uint64_t size = dir_size(map);
    bucket_t **visited = calloc(size, sizeof(bucket_t *));
    uint64_t n_visited = 0;

    for (uint64_t i = 0; i < size; i++) {
        bucket_t *b = map -> directory[i];
        bool already = false;

        for (uint64_t v = 0; v < n_visited; v++) {
            if (visited[v] == b) {
                already = true;
                break;
            }
        }

        if (!already) {
            visited[n_visited++] = b;
            free_bucket(b);
        }
    }

    free(visited);
    free(map -> directory);
    free(map);
}

// ============== FUNÇÕES ESTÁTICAS ==============

static uint64_t murmurhash3_64(const void *key, size_t len, uint32_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);
    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *)data;
    switch (len & 7) {
        case 7: h ^= (uint64_t)data2[6] << 48;
        case 6: h ^= (uint64_t)data2[5] << 40;
        case 5: h ^= (uint64_t)data2[4] << 32;
        case 4: h ^= (uint64_t)data2[3] << 24;
        case 3: h ^= (uint64_t)data2[2] << 16;
        case 2: h ^= (uint64_t)data2[1] <<  8;
        case 1: h ^= (uint64_t)data2[0];
                h *= m;
                break;
        default: break;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

static uint64_t exhash_capacity(const exhash_t *map) {
    uint64_t usable = map -> bucket_size - sizeof(uint16_t) * 2;
    return usable / slot_size(map);
}

static bucket_t *new_empty_bucket(uint32_t bucket_size, uint32_t record_size, uint16_t depth) {
    bucket_t *b = malloc(sizeof(bucket_t));
    assert(b != NULL);

    b -> local_depth = depth;
    b -> record_count = 0;

    uint64_t sz = sizeof(uint64_t) + record_size;
    uint64_t usable = bucket_size - sizeof(uint16_t) * 2;
    uint64_t cap = usable / sz;

    b -> data = calloc(cap, sz);
    assert(b -> data != NULL);

    return b;
}

static void free_bucket(bucket_t *b) {
    if (!b) return;
    free(b -> data);
    free(b);
}

static bool insert_raw_into_bucket(const exhash_t *map, bucket_t *b, uint64_t numeric_key, const void *data) {
    uint64_t sz = slot_size(map);
    uint8_t *slot = b -> data + b -> record_count * sz;

    memcpy(slot, &numeric_key, sizeof(uint64_t));
    memcpy(slot + sizeof(uint64_t), data, map -> record_size);

    b -> record_count++;
    return true;
}

static bool insert_into_bucket(const exhash_t *map, bucket_t *b, const char *key, const void *data) {
    uint64_t numeric_key = murmurhash3_64(key, strlen(key), 0);
    return insert_raw_into_bucket(map, b, numeric_key, data);
}

static uint64_t get_directory_index(const exhash_t *map, const char *key) {
    return murmurhash3_64(key, strlen(key), 0) & dir_mask(map);
}

static void expand_directory(exhash_t *map) {
    uint64_t old_size = dir_size(map);
    uint64_t new_size = old_size * 2;

    bucket_t **nd = realloc(map -> directory, sizeof(bucket_t *) * new_size);
    assert(nd != NULL);

    map -> directory = nd;

    for (uint64_t i = 0; i < old_size; i++) {
        map -> directory[i + old_size] = map -> directory[i];
    }

    map -> global_depth++;
}

static void redistribute_records(const exhash_t *map, bucket_t *old_b, bucket_t *new_b, uint32_t old_count) {
    void *tmp = malloc(map -> record_size);
    assert(tmp != NULL);

    uint64_t sz = slot_size(map);

    for (uint32_t i = 0; i < old_count; i++) {
        uint64_t saved_key;
        uint8_t *slot = old_b -> data + i * sz;

        memcpy(&saved_key, slot, sizeof(uint64_t));
        memcpy(tmp, slot + sizeof(uint64_t), map -> record_size);

        uint32_t bit = (saved_key >> (old_b -> local_depth - 1)) & 1;

        if (bit == 0) {
            insert_raw_into_bucket(map, old_b, saved_key, tmp);
        } else {
            insert_raw_into_bucket(map, new_b, saved_key, tmp);
        }
    }

    free(tmp);
}

static void update_directory_pointers(exhash_t *map, bucket_t *old_b, bucket_t *new_b, uint8_t depth) {
    uint64_t size = dir_size(map);

    for (uint64_t i = 0; i < size; i++) {
        if (map -> directory[i] == old_b) {
            uint32_t bit = (i >> (depth - 1)) & 1;
            if (bit == 1) {
                map -> directory[i] = new_b;
            }
        }
    }
}

static void split_bucket(exhash_t *map, bucket_t *old_b) {
    old_b -> local_depth++;

    bucket_t *new_b = new_empty_bucket(map -> bucket_size, map -> record_size, old_b -> local_depth);
    assert(new_b != NULL);

    uint32_t old_count = old_b -> record_count;
    old_b -> record_count = 0;

    redistribute_records(map, old_b, new_b, old_count);
    update_directory_pointers(map, old_b, new_b, old_b -> local_depth);
}

static bool exhash_contains(const exhash_t *map, const char *key) {
    uint64_t hashed_key = murmurhash3_64(key, strlen(key), 0);
    uint64_t idx = hashed_key & dir_mask(map);
    bucket_t *b = map -> directory[idx];
    uint64_t sz = slot_size(map);

    for (uint16_t i = 0; i < b -> record_count; i++) {
        uint64_t stored_key;
        memcpy(&stored_key, b -> data + i * sz, sizeof(uint64_t));

        if (stored_key == hashed_key) {
            return true;
        }
    }

    return false;
}