#define _POSIX_C_SOURCE 200809L
#include "../include/dot_export.h"
#include <stdio.h>
#include <string.h>

static void write_label(FILE *fp, const char *label) {
    for (const char *p = label; *p; p++) {
        unsigned char c = (unsigned char)*p;
        if (c > 127) continue;
        if (c == '"')  continue;
        if (c == '\\') { fputc('_', fp); continue; }
        fputc(c, fp);
    }
}

void dot_export(const Graph *g, const CliqueResult *clique,
                const char *filename) {
    if (!g || !filename) return;

    FILE *fp = fopen(filename, "w");
    if (!fp) { fprintf(stderr, "[ERRO] dot_export: nao abriu '%s'\n", filename); return; }

    Bitset in_clique;
    bitset_zero(&in_clique);
    if (clique)
        for (int i = 0; i < clique->size; i++)
            bitset_set(&in_clique, clique->vertices[i]);

    fprintf(fp, "graph G {\n");
    fprintf(fp, "  graph [bgcolor=white fontname=\"Helvetica\"];\n");
    fprintf(fp, "  node  [fontname=\"Helvetica\" fontsize=10 style=filled];\n");
    fprintf(fp, "  edge  [fontname=\"Helvetica\"];\n\n");

    for (int i = 0; i < g->n; i++) {
        const char *lbl = (g->vertex_labels && g->vertex_labels[i])
                          ? g->vertex_labels[i] : "";
        if (bitset_test(&in_clique, i)) {
            fprintf(fp, "  v%d [label=\"", i);
            write_label(fp, lbl[0] ? lbl : "v");
            fprintf(fp, "\" fillcolor=firebrick1 fontcolor=white penwidth=2];\n");
        } else {
            fprintf(fp, "  v%d [label=\"", i);
            write_label(fp, lbl[0] ? lbl : "v");
            fprintf(fp, "\" fillcolor=gray90 fontcolor=gray30];\n");
        }
    }

    fprintf(fp, "\n");

    for (int u = 0; u < g->n; u++) {
        for (int v = u + 1; v < g->n; v++) {
            if (!graph_has_edge(g, u, v)) continue;
            if (bitset_test(&in_clique, u) && bitset_test(&in_clique, v))
                fprintf(fp, "  v%d -- v%d [color=firebrick1 penwidth=2.5];\n", u, v);
            else
                fprintf(fp, "  v%d -- v%d [color=gray80];\n", u, v);
        }
    }

    fprintf(fp, "}\n");
    fclose(fp);

    printf("[DOT] '%s' gerado. Para visualizar:\n", filename);
    printf("      dot -Tpng %s -o saida.png\n", filename);
}
