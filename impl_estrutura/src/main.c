#include <stdio.h>
#include <string.h>
#include "../include/graph.h"
#include "../include/dimacs.h"
#include "../include/clique.h"
#include "../include/twitter_parser.h"
#include "../include/dot_export.h"

static void usage(const char *prog) {
    printf("Uso:\n");
    printf("  %s greedy   <arquivo.clq>          - Algoritmo Guloso\n", prog);
    printf("  %s grasp    <arquivo.clq>           - GRASP\n", prog);
    printf("  %s twitter  <arquivo.csv>           - Parser Twitter\n", prog);
    printf("  %s test                             - Teste da estrutura\n", prog);
}

/* ── Modo teste rápido da estrutura ─────────────────────── */
static int run_test(void) {
    printf("=== Teste da Estrutura do Grafo ===\n\n");
    Graph *g = graph_create(6);
    graph_add_edge(g, 0, 1); graph_add_edge(g, 0, 2);
    graph_add_edge(g, 0, 3); graph_add_edge(g, 1, 2);
    graph_add_edge(g, 1, 4); graph_add_edge(g, 2, 4);
    graph_add_edge(g, 2, 3);

    graph_print_info(g);
    printf("\n(0,1) existe? %s\n", graph_has_edge(g,0,1) ? "SIM" : "NAO");
    printf("(0,4) existe? %s\n", graph_has_edge(g,0,4) ? "SIM" : "NAO");

    Bitset inter;
    bitset_and(&inter, &g->adjacency[1], &g->adjacency[2]);
    printf("Vizinhos comuns de 1 e 2: { ");
    for (int j = bitset_next(&inter,0); j!=-1; j=bitset_next(&inter,j+1))
        printf("%d ", j);
    printf("}\n");

    graph_free(g);
    printf("\n[OK] Teste concluido\n");
    return 0;
}

/* ── Modo greedy ────────────────────────────────────────── */
static int run_greedy(const char *file) {
    Graph *g = dimacs_read(file);
    if (!g) return 1;
    graph_print_info(g);

    CliqueResult *r = clique_greedy(g);
    clique_result_print(r, g);
    dot_export(g, r, "output_greedy.dot");
    printf("[DOT] Grafo exportado para 'output_greedy.dot'\n");

    clique_result_free(r);
    graph_free(g);
    return 0;
}

/* ── Modo GRASP ─────────────────────────────────────────── */
static int run_grasp(const char *file) {
    Graph *g = dimacs_read(file);
    if (!g) return 1;
    graph_print_info(g);

    CliqueResult *r = clique_grasp(g, 100, 3, 42);
    clique_result_print(r, g);
    dot_export(g, r, "output_grasp.dot");
    printf("[DOT] Grafo exportado para 'output_grasp.dot'\n");

    clique_result_free(r);
    graph_free(g);
    return 0;
}

/* ── Modo Twitter ───────────────────────────────────────── */
static int run_twitter(const char *file) {
    Graph *g = twitter_parse(file, MAX_VERTICES);
    if (!g) return 1;
    graph_print_info(g);

    CliqueResult *r = clique_greedy(g);
    clique_result_print(r, g);
    dot_export(g, r, "output_twitter.dot");
    printf("[DOT] Grafo exportado para 'output_twitter.dot'\n");

    clique_result_free(r);
    graph_free(g);
    return 0;
}

/* ── Main ───────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    if (argc < 2) { usage(argv[0]); return 1; }

    if (strcmp(argv[1], "test") == 0)
        return run_test();

    if (argc < 3) { usage(argv[0]); return 1; }

    if (strcmp(argv[1], "greedy")  == 0) return run_greedy(argv[2]);
    if (strcmp(argv[1], "grasp")   == 0) return run_grasp(argv[2]);
    if (strcmp(argv[1], "twitter") == 0) return run_twitter(argv[2]);

    usage(argv[0]);
    return 1;
}
