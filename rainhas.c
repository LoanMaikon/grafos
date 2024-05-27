#include "rainhas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SQUARES (n * n) // Board size will always be n^2.
#define ADD 1
#define REM 0

// Só para eu lembrar depois:
//                              n - 1 da linha horizontal
//                              n - 1 da linha vertical
//                              n - 1 + 2*(min(i, j)) da diagonal
//                              total: 3(n -1) + 2*(min(i, j)) para cada casa
//                              maior valor da diagonal acontece no centro
//                              o centro ocorre em ceil(n/2)
//                              preciso alocar 3(n - 1) + n = 4n - 3

struct graph_t {
    unsigned int **adjacency;
    unsigned int size; // Guarda o tamanho do tabuleiro, e não do grafo.
};

struct set_t {
    unsigned int size; // Guarda a quantidade de elementos no conjunto.
    unsigned int *data;
};

// Aloca espaço para um grafo (matriz de adjacência).
struct graph_t* create_graph(unsigned int n) {
    int i;
    struct graph_t* g;

    if(! (g = malloc(sizeof(struct graph_t)))) {
        fprintf(stderr, "Erro ao alocar memória");
        return NULL;
    }
    g->size = n;

    if(! (g->adjacency = calloc(BOARD_SQUARES, sizeof(int*)))) {
        fprintf(stderr, "Erro ao alocar memória");
        return NULL;
    }
    for (i = 0; i < BOARD_SQUARES; i++)
        if(! (g->adjacency[i] = calloc(BOARD_SQUARES, sizeof(int)))) {
            fprintf(stderr, "Erro ao alocar memória");
            return NULL;
        }

    return g;
}

// Aloca espaço para um conjunto.
struct set_t* create_set(unsigned int n) {
    struct set_t *s;
    if(! (s = malloc(sizeof(struct set_t)))) {
        fprintf(stderr, "Erro ao alocar memória");
        return NULL;
    }
    if(! (s->data = calloc(BOARD_SQUARES, sizeof(int)))) {
        fprintf(stderr, "Erro ao alocar memória");
        return NULL;
    }
    s->size = 0;

    return s;
}

// Transforma uma coordenada do tabuleiro em índice do vetor.
int get_index_from_position(int board_size, casa c) {
    return board_size * c.linha + c.coluna;
}

// Transforma um índice do vetor em coordenada do tabuleiro.
void get_position_from_index(int index, int board_size, casa *c) {
    c->linha = index / board_size;
    c->coluna = index % board_size;
}

// Adiciona ou remove uma aresta entre duas casas.
void update_edge(struct graph_t *graph, casa c1, casa c2, int operation) {
    int v1 = get_index_from_position(graph->size, c1);
    int v2 = get_index_from_position(graph->size, c2);
    graph->adjacency[v1][v2] = operation;
    graph->adjacency[v2][v1] = operation;
}

// Imprime a matriz de adjacência.
void print_graph(struct graph_t *g) {
    int i, j;
    int n = g->size;

    for (i = 0; i < BOARD_SQUARES; i++) {
        for(j = 0; j < BOARD_SQUARES; j++)
            printf("%d ", g->adjacency[i][j]);
        printf("\n");
    }
}

// Ainda não comecei a implementar a parte dos conjuntos.
//struct set_t* append(struct set_t *s, casa v) {
//    return NULL;
//}
//struct set_t* remove_neighbours(struct graph_t *g, struct set_t *s, casa v) {
//    return NULL;
//}


int isValid(unsigned int *t, unsigned int r, unsigned int j, casa *c, unsigned int k);
unsigned int *posicionarRainhas(unsigned int *t, unsigned int n, unsigned int r, casa *c, unsigned int k);


int isValid(unsigned int *t, unsigned int r, unsigned int j, casa *c, unsigned int k) {
    for (unsigned int i = 0; i < k; i++) {
        if (c[i].linha == r + 1 && c[i].coluna == j + 1) {
            return 0;
        }
    }

    for (unsigned int i = 0; i < r; i++) {
        if (t[i] == j || t[i] == j + r - i || t[i] == j - r + i) {
            return 0;
        }
    }

    return 1;
}

unsigned int *posicionarRainhas(unsigned int *t, unsigned int n, unsigned int r, casa *c, unsigned int k) {
    if (r == n) {
        return t;
    }

    for (unsigned int j = 0; j < n; j++) {
        if (isValid(t, r, j, c, k)) {
            t[r] = j;
            if (posicionarRainhas(t, n, r + 1, c, k) != NULL) {
                return t;
            }
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n rainhas 
// com casas proibidas usando backtracking
//
//    n é o tamanho (número de linhas/colunas) do tabuleiro
//
//    c é um vetor de k 'struct casa' indicando as casas proibidas
//
//    r é um vetor de n posições (já alocado) a ser preenchido com a resposta:
//      r[i] = j > 0 indica que a rainha da linha i+1 fica na coluna j;
//      r[i] = 0     indica que não há rainha nenhuma na linha i+1
//
// devolve r

unsigned int *rainhas_bt(unsigned int n, unsigned int k, casa *c, unsigned int *r) {
    return posicionarRainhas(r, n, 0, c, k);
}
//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int *rainhas_ci(unsigned int n, unsigned int k, casa *c, unsigned int *r) {
    return r;
}
