#ifndef DIMACS_H
#define DIMACS_H

#include "graph.h"

/*
 * Lê arquivo .clq no formato DIMACS ASCII e retorna um Graph.
 * Formato: linhas 'c' (comentário), 'p edge n m', 'e u v'
 * Vértices no arquivo são 1-indexados; internamente 0-indexados.
 * Retorna NULL em caso de erro.
 * Complexidade: O(n + m)
 */
Graph* dimacs_read(const char *filename);

#endif /* DIMACS_H */
