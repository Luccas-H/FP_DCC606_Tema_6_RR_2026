#ifndef TWITTER_PARSER_H
#define TWITTER_PARSER_H

#include "graph.h"

/*
 * Lê um CSV do dataset Twitter Customer Support (twcs/sample.csv)
 * e constrói um grafo de interações.
 *
 * Arestas criadas por:
 *   - in_response_to_tweet_id  (resposta direta)
 *   - menções @usuario no campo text (conexão implícita)
 *
 * Retorna NULL em caso de erro.
 */
Graph* twitter_parse(const char *filename, int max_vertices);

#endif /* TWITTER_PARSER_H */
