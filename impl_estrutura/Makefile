CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude

# Fontes — adicionar novos .c aqui conforme forem criados
SRCS = src/graph.c      \
       src/dimacs.c     \
       src/clique.c     \
       src/twitter_parser.c \
       src/dot_export.c \
       src/main.c

OBJS = $(SRCS:.c=.o)
BIN  = proj_exec

# ── Build principal ─────────────────────────────────────────
all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo ">>> Binario '$(BIN)' gerado"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Atalhos de execução ─────────────────────────────────────
test:
	./$(BIN) test

greedy:
	./$(BIN) greedy benchmarks/brock200_2.clq

grasp:
	./$(BIN) grasp benchmarks/brock200_2.clq

twitter:
	./$(BIN) twitter benchmarks/sample.csv

# ── Limpeza ─────────────────────────────────────────────────
clean:
	rm -f src/*.o $(BIN)
	@echo ">>> Limpo"

.PHONY: all test greedy grasp twitter clean
