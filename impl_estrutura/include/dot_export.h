#ifndef DOT_EXPORT_H
#define DOT_EXPORT_H

#include "graph.h"
#include "clique.h"

/*
 * Gera arquivo .dot para visualização com GraphViz.
 * Vértices do clique são destacados em vermelho.
 * Uso: dot -Tpng output.dot -o output.png
 */
void dot_export(const Graph *g, const CliqueResult *clique,
                const char *filename);

#endif /* DOT_EXPORT_H */
