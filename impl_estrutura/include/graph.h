#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* =========================================================
 * CONFIGURAÇÕES
 * ========================================================= */
#define MAX_VERTICES  10000
#define BITS_PER_WORD 64
#define WORDS_PER_ROW ((MAX_VERTICES + BITS_PER_WORD - 1) / BITS_PER_WORD)

/* =========================================================
 * BITSET
 * Representação principal do grafo.
 * Bit j setado = existe aresta com o vértice j.
 * Interseção de vizinhanças = AND bit a bit (O(n/64)).
 * ========================================================= */
typedef struct {
    uint64_t words[WORDS_PER_ROW];
} Bitset;

static inline void bitset_set(Bitset *bs, int j) {
    bs->words[j / BITS_PER_WORD] |= (1ULL << (j % BITS_PER_WORD));
}
static inline void bitset_clear(Bitset *bs, int j) {
    bs->words[j / BITS_PER_WORD] &= ~(1ULL << (j % BITS_PER_WORD));
}
static inline int bitset_test(const Bitset *bs, int j) {
    return (bs->words[j / BITS_PER_WORD] >> (j % BITS_PER_WORD)) & 1;
}
static inline void bitset_zero(Bitset *bs) {
    memset(bs->words, 0, sizeof(bs->words));
}
static inline void bitset_and(Bitset *out, const Bitset *a, const Bitset *b) {
    for (int w = 0; w < WORDS_PER_ROW; w++)
        out->words[w] = a->words[w] & b->words[w];
}
static inline int bitset_popcount(const Bitset *bs) {
    int count = 0;
    for (int w = 0; w < WORDS_PER_ROW; w++)
        count += __builtin_popcountll(bs->words[w]);
    return count;
}
static inline int bitset_empty(const Bitset *bs) {
    for (int w = 0; w < WORDS_PER_ROW; w++)
        if (bs->words[w]) return 0;
    return 1;
}
static inline int bitset_next(const Bitset *bs, int start) {
    for (int j = start; j < MAX_VERTICES; j++)
        if (bitset_test(bs, j)) return j;
    return -1;
}

/* =========================================================
 * LISTA DE ADJACÊNCIA - navegação geral
 * ========================================================= */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/* =========================================================
 * GRAFO
 * ========================================================= */
typedef struct {
    int      n;              /* número de vértices        */
    int      m;              /* número de arestas         */
    Bitset  *adjacency;      /* adjacency[i] = Bitset     */
    AdjNode **adj_list;      /* lista de adjacência       */
    int     *degree;         /* grau de cada vértice      */
    char   **vertex_labels;  /* label string (Twitter)    */
} Graph;

/* =========================================================
 * FUNÇÕES
 * ========================================================= */
Graph* graph_create(int n);
void   graph_free(Graph *g);
void   graph_add_edge(Graph *g, int u, int v);
int    graph_has_edge(const Graph *g, int u, int v);
void   graph_print_info(const Graph *g);

#endif /* GRAPH_H */
