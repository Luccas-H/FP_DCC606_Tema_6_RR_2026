#include "../include/graph.h"

/* Complexidade: O(n) */
Graph* graph_create(int n) {
    if (n <= 0 || n > MAX_VERTICES) {
        fprintf(stderr, "[ERRO] graph_create: n=%d invalido (max=%d)\n", n, MAX_VERTICES);
        return NULL;
    }
    Graph *g = (Graph*) malloc(sizeof(Graph));
    if (!g) { perror("malloc Graph"); return NULL; }

    g->n = n;
    g->m = 0;

    g->adjacency = (Bitset*) calloc(n, sizeof(Bitset));
    if (!g->adjacency) { perror("calloc adjacency"); free(g); return NULL; }

    g->adj_list = (AdjNode**) calloc(n, sizeof(AdjNode*));
    if (!g->adj_list) { perror("calloc adj_list"); free(g->adjacency); free(g); return NULL; }

    g->degree = (int*) calloc(n, sizeof(int));
    if (!g->degree) { perror("calloc degree"); free(g->adj_list); free(g->adjacency); free(g); return NULL; }

    g->vertex_labels = (char**) calloc(n, sizeof(char*));
    if (!g->vertex_labels) { perror("calloc labels"); free(g->degree); free(g->adj_list); free(g->adjacency); free(g); return NULL; }

    return g;
}

/* Complexidade: O(n + m) */
void graph_free(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        AdjNode *cur = g->adj_list[i];
        while (cur) { AdjNode *nx = cur->next; free(cur); cur = nx; }
        if (g->vertex_labels[i]) free(g->vertex_labels[i]);
    }
    free(g->adjacency);
    free(g->adj_list);
    free(g->degree);
    free(g->vertex_labels);
    free(g);
}

/* Complexidade: O(1) */
void graph_add_edge(Graph *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n || u == v) return;
    if (bitset_test(&g->adjacency[u], v)) return; /* ignora duplicata */

    bitset_set(&g->adjacency[u], v);
    bitset_set(&g->adjacency[v], u);

    AdjNode *nv = (AdjNode*) malloc(sizeof(AdjNode));
    AdjNode *nu = (AdjNode*) malloc(sizeof(AdjNode));
    if (!nv || !nu) { perror("malloc AdjNode"); return; }

    nv->vertex = v; nv->next = g->adj_list[u]; g->adj_list[u] = nv;
    nu->vertex = u; nu->next = g->adj_list[v]; g->adj_list[v] = nu;

    g->degree[u]++;
    g->degree[v]++;
    g->m++;
}

/* Complexidade: O(1) */
int graph_has_edge(const Graph *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) return 0;
    return bitset_test(&g->adjacency[u], v);
}

void graph_print_info(const Graph *g) {
    if (!g) return;
    int dmax = 0, dmin = g->degree[0];
    for (int i = 0; i < g->n; i++) {
        if (g->degree[i] > dmax) dmax = g->degree[i];
        if (g->degree[i] < dmin) dmin = g->degree[i];
    }
    printf("=== Grafo ===\n");
    printf("  Vertices : %d\n", g->n);
    printf("  Arestas  : %d\n", g->m);
    printf("  Densidade: %.4f\n",
           (g->n > 1) ? (2.0 * g->m) / ((double)g->n * (g->n-1)) : 0.0);
    printf("  Grau max : %d\n", dmax);
    printf("  Grau min : %d\n", dmin);
}
