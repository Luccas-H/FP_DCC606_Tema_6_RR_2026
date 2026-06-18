#define _POSIX_C_SOURCE 200809L
#include "../include/twitter_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =========================================================
 * TABELA HASH string -> int
 * ========================================================= */
#define HASH_SIZE 20011

typedef struct HashEntry {
    char            *key;
    int              idx;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry *buckets[HASH_SIZE];
    int        count;
} HashMap;

static unsigned int hash_str(const char *s) {
    unsigned int h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h % HASH_SIZE;
}

static HashMap* hashmap_create(void) {
    return (HashMap*) calloc(1, sizeof(HashMap));
}

static void hashmap_free(HashMap *m) {
    if (!m) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *e = m->buckets[i];
        while (e) { HashEntry *nx = e->next; free(e->key); free(e); e = nx; }
    }
    free(m);
}

static int hashmap_get_or_insert(HashMap *m, const char *key, int *idx_out) {
    unsigned int h = hash_str(key);
    for (HashEntry *e = m->buckets[h]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) { *idx_out = e->idx; return 0; }
    }
    HashEntry *ne = (HashEntry*) malloc(sizeof(HashEntry));
    if (!ne) return -1;
    ne->key  = strdup(key);
    ne->idx  = m->count++;
    ne->next = m->buckets[h];
    m->buckets[h] = ne;
    *idx_out = ne->idx;
    return 1;
}

static int hashmap_get(HashMap *m, const char *key) {
    unsigned int h = hash_str(key);
    for (HashEntry *e = m->buckets[h]; e; e = e->next)
        if (strcmp(e->key, key) == 0) return e->idx;
    return -1;
}

static void hashmap_insert_raw(HashMap *m, const char *key, int idx) {
    unsigned int h = hash_str(key);
    HashEntry *ne = (HashEntry*) malloc(sizeof(HashEntry));
    if (!ne) return;
    ne->key  = strdup(key);
    ne->idx  = idx;
    ne->next = m->buckets[h];
    m->buckets[h] = ne;
    m->count++;
}

/* =========================================================
 * PARSER CSV — respeita campos entre aspas
 * ========================================================= */
static const char* csv_next_field(const char *p, char *out, int out_sz) {
    if (!p || *p == '\0') return NULL;
    int i = 0;
    if (*p == '"') {
        p++;
        while (*p && !(*p == '"' && *(p+1) != '"')) {
            if (i < out_sz - 1) out[i++] = *p;
            if (*p == '"') p++;
            p++;
        }
        if (*p == '"') p++;
    } else {
        while (*p && *p != ',' && *p != '\n' && *p != '\r')
            if (i < out_sz - 1) out[i++] = *p++;
            else p++;
    }
    out[i] = '\0';
    if (*p == ',') p++;
    return p;
}

/* =========================================================
 * ESTRUTURA TEMPORÁRIA POR LINHA
 * Guarda só o que precisa para montar as arestas.
 * ========================================================= */
typedef struct {
    int author_idx;
    int reply_to_tweet_idx; /* índice no tweet_map, ou -1 */
} Row;

/* =========================================================
 * twitter_parse — passagem única
 *
 * Estratégia:
 *   - Lê linha por linha uma única vez
 *   - Registra author_id -> indice no grafo (id_map)
 *   - Registra tweet_id -> author_idx (tweet_map)
 *   - Guarda (author_idx, in_response_to_tweet_id) em Row[]
 *   - Após a leitura, resolve arestas via tweet_map
 *
 * Complexidade: O(L) onde L = linhas do CSV
 * ========================================================= */
Graph* twitter_parse(const char *filename, int max_vertices) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { fprintf(stderr, "[ERRO] nao abriu '%s'\n", filename); return NULL; }

    printf("[Twitter] Carregando '%s'...\n", filename);

    HashMap *id_map    = hashmap_create(); /* author_id -> vertice idx */
    HashMap *tweet_map = hashmap_create(); /* tweet_id  -> author idx  */

    int    cap   = 100000;
    Row   *rows  = (Row*) malloc(cap * sizeof(Row));
    char **reply_ids = (char**) malloc(cap * sizeof(char*));
    int    n_rows = 0;

    char line[2048];
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return NULL; } /* header */

    long line_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        line_count++;
        if (line_count % 500000 == 0)
            printf("[Twitter] %ld linhas lidas, %d autores...\n",
                   line_count, id_map->count);

        /* Para quando atingir limite de vértices */
        if (id_map->count >= max_vertices && n_rows >= cap - 1) break;

        const char *p = line;
        char tweet_id[64], author_id[64], f3[64], f4[256], text[256];
        char resp_id[64], in_resp[64];

        p = csv_next_field(p, tweet_id,  sizeof(tweet_id));
        p = csv_next_field(p, author_id, sizeof(author_id));
        p = csv_next_field(p, f3,        sizeof(f3));
        p = csv_next_field(p, f4,        sizeof(f4));
        p = csv_next_field(p, text,      sizeof(text));
        p = csv_next_field(p, resp_id,   sizeof(resp_id));
        if (p) csv_next_field(p, in_resp, sizeof(in_resp));
        else   in_resp[0] = '\0';

        if (author_id[0] == '\0') continue;

        /* Registra autor */
        int a_idx = -1;
        if (id_map->count >= max_vertices) {
            a_idx = hashmap_get(id_map, author_id);
            if (a_idx < 0) continue;
        } else {
            hashmap_get_or_insert(id_map, author_id, &a_idx);
        }

        /* Registra tweet_id -> author */
        if (tweet_id[0] != '\0')
            hashmap_insert_raw(tweet_map, tweet_id, a_idx);

        /* Guarda linha para resolver arestas depois */
        if (n_rows >= cap) {
            cap *= 2;
            rows     = (Row*)   realloc(rows,     cap * sizeof(Row));
            reply_ids = (char**) realloc(reply_ids, cap * sizeof(char*));
        }
        rows[n_rows].author_idx        = a_idx;
        rows[n_rows].reply_to_tweet_idx = -1;
        reply_ids[n_rows] = (in_resp[0] != '\0') ? strdup(in_resp) : NULL;
        n_rows++;

        (void)text; (void)resp_id;
    }
    fclose(fp);

    int n = id_map->count;
    if (n > max_vertices) n = max_vertices;
    if (n == 0) {
        fprintf(stderr, "[ERRO] nenhum autor encontrado\n");
        goto cleanup;
    }

    Graph *g = graph_create(n);
    if (!g) goto cleanup;

    /* Copia labels */
    for (int b = 0; b < HASH_SIZE; b++) {
        for (HashEntry *e = id_map->buckets[b]; e; e = e->next)
            if (e->idx < n)
                g->vertex_labels[e->idx] = strdup(e->key);
    }

    /* Resolve arestas */
    int edges_reply = 0;
    for (int i = 0; i < n_rows; i++) {
        if (!reply_ids[i]) continue;
        int ref = hashmap_get(tweet_map, reply_ids[i]);
        if (ref >= 0 && ref < n && ref != rows[i].author_idx) {
            graph_add_edge(g, rows[i].author_idx, ref);
            edges_reply++;
        }
        free(reply_ids[i]);
    }

    printf("[Twitter] %d autores, %d arestas (%d respostas)\n",
           n, g->m, edges_reply);

    hashmap_free(id_map);
    hashmap_free(tweet_map);
    free(rows);
    free(reply_ids);
    return g;

cleanup:
    hashmap_free(id_map);
    hashmap_free(tweet_map);
    for (int i = 0; i < n_rows; i++) if (reply_ids[i]) free(reply_ids[i]);
    free(rows);
    free(reply_ids);
    return NULL;
}
