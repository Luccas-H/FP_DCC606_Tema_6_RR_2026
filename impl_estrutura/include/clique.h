#ifndef CLIQUE_H
#define CLIQUE_H

#include "graph.h"

/* =========================================================
 * RESULTADO
 * ========================================================= */
typedef struct {
    int    *vertices;   /* índices dos vértices do clique */
    int     size;       /* tamanho do clique              */
    long    iterations; /* iterações realizadas           */
    double  time_ms;    /* tempo em milissegundos         */
} CliqueResult;

CliqueResult* clique_result_create(int max_size);
void          clique_result_free(CliqueResult *r);
void          clique_result_print(const CliqueResult *r, const Graph *g);

/* =========================================================
 * MÓDULO 1 — GULOSO
 * Ordena por grau, expande clique via AND de Bitsets.
 * Complexidade: O(n² / 64)
 * ========================================================= */
CliqueResult* clique_greedy(const Graph *g);

/* =========================================================
 * MÓDULO 2 — GRASP
 * Fase construtiva aleatorizada (RCL) + busca local 1-opt.
 * Complexidade: O(max_iter × n² / 64)
 * ========================================================= */
CliqueResult* clique_grasp(const Graph *g, int max_iter,
                           int rcl_size, unsigned int seed);

#endif /* CLIQUE_H */
