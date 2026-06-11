#include "../include/dimacs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Complexidade: O(n + m) */
Graph* dimacs_read(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { fprintf(stderr, "[ERRO] dimacs_read: nao abriu '%s'\n", filename); return NULL; }

    Graph *g = NULL;
    char   line[512];
    int    n = 0, m_expected = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'c' || line[0] == '\n' || line[0] == '\r') continue;

        if (line[0] == 'p') {
            char fmt[16];
            if (sscanf(line, "p %s %d %d", fmt, &n, &m_expected) != 3) {
                fprintf(stderr, "[ERRO] dimacs_read: linha 'p' invalida\n");
                fclose(fp); return NULL;
            }
            g = graph_create(n);
            if (!g) { fclose(fp); return NULL; }
            continue;
        }

        if (line[0] == 'e' && g) {
            int u, v;
            if (sscanf(line, "e %d %d", &u, &v) == 2)
                graph_add_edge(g, u - 1, v - 1); /* 1-idx -> 0-idx */
        }
    }
    fclose(fp);

    if (!g) { fprintf(stderr, "[ERRO] dimacs_read: sem linha 'p' em '%s'\n", filename); return NULL; }

    printf("[DIMACS] '%s': %d vertices, %d arestas (esperado: %d)\n",
           filename, g->n, g->m, m_expected);
    return g;
}
