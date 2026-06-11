#include "../include/clique.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* ── Utilitários do resultado ──────────────────────────── */

CliqueResult* clique_result_create(int max_size) {
    CliqueResult *r = (CliqueResult*) malloc(sizeof(CliqueResult));
    if (!r) return NULL;
    r->vertices   = (int*) malloc(max_size * sizeof(int));
    r->size       = 0;
    r->iterations = 0;
    r->time_ms    = 0.0;
    return r;
}

void clique_result_free(CliqueResult *r) {
    if (!r) return;
    free(r->vertices);
    free(r);
}

void clique_result_print(const CliqueResult *r, const Graph *g) {
    if (!r) return;
    printf("\n=== Resultado ===\n");
    printf("  Tamanho do clique : %d\n", r->size);
    printf("  Iteracoes         : %ld\n", r->iterations);
    printf("  Tempo             : %.2f ms\n", r->time_ms);
    printf("  Vertices          : { ");
    for (int i = 0; i < r->size; i++) {
        if (g && g->vertex_labels && g->vertex_labels[r->vertices[i]])
            printf("%s ", g->vertex_labels[r->vertices[i]]);
        else
            printf("%d ", r->vertices[i] + 1); /* imprime 1-indexado */
    }
    printf("}\n");
}

/* ── GULOSO ─────────────────────────────────────────────────
 *
 * Estrategia:
 *   1. Comeca com o vertice de maior grau como semente.
 *   2. Mantem Bitset P com candidatos: vizinhos de todos no clique.
 *   3. A cada passo escolhe o candidato de maior grau em P.
 *   4. Adiciona ao clique e atualiza P via AND.
 *   5. Para quando P fica vazio.
 *
 * Complexidade: O(n^2/64)
 * ─────────────────────────────────────────────────────────── */
CliqueResult* clique_greedy(const Graph *g) {
    if (!g) return NULL;

    clock_t t_start = clock();

    CliqueResult *r = clique_result_create(g->n);
    if (!r) return NULL;

    Bitset P;
    bitset_zero(&P);
    for (int i = 0; i < g->n; i++) bitset_set(&P, i);

    /* Semente: vertice de maior grau */
    int seed = 0;
    for (int i = 1; i < g->n; i++)
        if (g->degree[i] > g->degree[seed]) seed = i;

    r->vertices[r->size++] = seed;
    bitset_and(&P, &P, &g->adjacency[seed]);
    r->iterations++;

    while (!bitset_empty(&P)) {
        int best = -1, best_deg = -1;

        for (int v = bitset_next(&P,0); v != -1; v = bitset_next(&P,v+1)) {
            Bitset tmp;
            bitset_and(&tmp, &P, &g->adjacency[v]);
            int deg_in_P = bitset_popcount(&tmp);
            if (deg_in_P > best_deg) { best_deg = deg_in_P; best = v; }
            r->iterations++;
        }

        if (best == -1) break;
        r->vertices[r->size++] = best;
        bitset_and(&P, &P, &g->adjacency[best]);
    }

    r->time_ms = 1000.0 * (clock() - t_start) / CLOCKS_PER_SEC;
    return r;
}

/* ── GRASP ──────────────────────────────────────────────────
 *
 * FASE 1 - Construtiva aleatorizada (RCL):
 *   Igual ao guloso, mas em vez de sempre pegar o melhor
 *   candidato, sorteia entre os 'rcl_size' melhores.
 *   Isso diversifica a busca entre iteracoes.
 *
 * FASE 2 - Busca local 1-opt:
 *   Tenta trocar cada vertice do clique por dois outros fora
 *   dele. Se a troca aumentar o tamanho, aceita e continua.
 *
 * Guarda o melhor clique encontrado em todas as iteracoes.
 *
 * Complexidade por iteracao: O(n^2/64)
 * Complexidade total:        O(max_iter x n^2/64)
 * ─────────────────────────────────────────────────────────── */

typedef struct { int vertex; int deg_in_P; } Candidate;

static int cmp_cand_desc(const void *a, const void *b) {
    return ((Candidate*)b)->deg_in_P - ((Candidate*)a)->deg_in_P;
}

/* Fase construtiva: monta um clique via RCL */
static CliqueResult* grasp_construct(const Graph *g, int rcl_size,
                                     unsigned int *rng) {
    CliqueResult *r = clique_result_create(g->n);
    if (!r) return NULL;

    Candidate *buf = (Candidate*) malloc(g->n * sizeof(Candidate));
    if (!buf) { clique_result_free(r); return NULL; }

    Bitset P;
    bitset_zero(&P);
    for (int i = 0; i < g->n; i++) bitset_set(&P, i);

    /* Semente: sorteia entre os top rcl_size por grau global */
    int nc = 0;
    for (int v = bitset_next(&P,0); v != -1; v = bitset_next(&P,v+1))
        buf[nc++] = (Candidate){ v, g->degree[v] };
    qsort(buf, nc, sizeof(Candidate), cmp_cand_desc);
    int rcl = (rcl_size < nc) ? rcl_size : nc;
    int seed_v = buf[*rng % rcl].vertex;
    *rng = *rng * 1664525u + 1013904223u; /* LCG */

    r->vertices[r->size++] = seed_v;
    bitset_and(&P, &P, &g->adjacency[seed_v]);

    /* Expande via RCL */
    while (!bitset_empty(&P)) {
        int nb = 0;
        for (int v = bitset_next(&P,0); v != -1; v = bitset_next(&P,v+1)) {
            Bitset tmp;
            bitset_and(&tmp, &P, &g->adjacency[v]);
            buf[nb++] = (Candidate){ v, bitset_popcount(&tmp) };
            r->iterations++;
        }
        qsort(buf, nb, sizeof(Candidate), cmp_cand_desc);
        int r_sz = (rcl_size < nb) ? rcl_size : nb;
        int chosen = buf[*rng % r_sz].vertex;
        *rng = *rng * 1664525u + 1013904223u;
        r->vertices[r->size++] = chosen;
        bitset_and(&P, &P, &g->adjacency[chosen]);
    }

    free(buf);
    return r;
}

/* Fase de busca local 1-opt */
static void grasp_local_search(const Graph *g, CliqueResult *r) {
    int improved = 1;
    while (improved) {
        improved = 0;
        for (int i = 0; i < r->size; i++) {
            /* P = vizinhos comuns de todos exceto vertices[i] */
            Bitset P;
            bitset_zero(&P);
            for (int j = 0; j < g->n; j++) bitset_set(&P, j);
            for (int j = 0; j < r->size; j++) {
                if (j == i) continue;
                bitset_and(&P, &P, &g->adjacency[r->vertices[j]]);
            }
            for (int j = 0; j < r->size; j++)
                bitset_clear(&P, r->vertices[j]);

            /* Tenta adicionar dois vertices de P */
            int extra[2], ne = 0;
            for (int v = bitset_next(&P,0); v != -1 && ne < 2;
                 v = bitset_next(&P,v+1)) {
                int ok = 1;
                for (int k = 0; k < ne; k++)
                    if (!graph_has_edge(g, v, extra[k])) { ok = 0; break; }
                if (ok) extra[ne++] = v;
            }

            /* Aceita troca se ganhar pelo menos 1 vertice (ne=2 troca 1 por 2) */
            if (ne >= 2) {
                r->vertices[i] = extra[0];
                r->vertices[r->size++] = extra[1];
                improved = 1;
                break;
            }
        }
    }
}

CliqueResult* clique_grasp(const Graph *g, int max_iter,
                           int rcl_size, unsigned int seed) {
    if (!g) return NULL;

    clock_t t_start = clock();

    CliqueResult *best = clique_result_create(g->n);
    if (!best) return NULL;

    unsigned int rng = seed;

    for (int it = 0; it < max_iter; it++) {
        CliqueResult *cur = grasp_construct(g, rcl_size, &rng);
        if (!cur) continue;

        grasp_local_search(g, cur);

        best->iterations += cur->iterations;

        if (cur->size > best->size) {
            best->size = cur->size;
            for (int i = 0; i < cur->size; i++)
                best->vertices[i] = cur->vertices[i];
        }

        clique_result_free(cur);
    }

    best->time_ms = 1000.0 * (clock() - t_start) / CLOCKS_PER_SEC;
    return best;
}
