#include "rainhas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SQUARES (n * n) // Board size will always be n^2.
#define ADD 1
#define REM 0
#define TRUE 1
#define FALSE 0

int
isValid(unsigned int *t, unsigned int r, unsigned int j, casa *c,
        unsigned int k);

unsigned int*
posicionarRainhas(unsigned int *t, unsigned int n, unsigned int r,
                   casa *c, unsigned int k);

//-------------------------- Parte backtracking -------------------------------
int
isValid(unsigned int *t, unsigned int r, unsigned int j, casa *c,
        unsigned int k) {
    for (unsigned int i = 0; i < k; i++) {
        if (c[i].linha == r + 1 && c[i].coluna == j + 1) {
            return FALSE;
        }
    }

    for (unsigned int i = 0; i < r; i++) {
        if (t[i] == j || t[i] == j + r - i || t[i] == j - r + i) {
            return FALSE;
        }
    }

    return TRUE;
}

unsigned int*
posicionarRainhas(unsigned int *t, unsigned int n, unsigned int r,
                   casa *c, unsigned int k) {
    if (r == n) {
        return t;
    }

    for (unsigned int j = 0; j < n; j++) {
        if (isValid(t, r, j, c, k)) {
            t[r] = j + 1;
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

unsigned int*
rainhas_bt(unsigned int n, unsigned int k, casa *c,
            unsigned int *r) {
    return posicionarRainhas(r, n, 0, c, k);
}

//----------------------------- Parte grafos ----------------------------------
// Precisamos ver como usar a tabela hash de grafos. Talvez conversar com o
// Marcelo sobre a implementação.
struct graph_t {
    unsigned int **adjacency;
    unsigned int size; // Guarda o tamanho do tabuleiro, e não do grafo.
};

// Pretendo depois mudar para uma tabela hash também. Vai acelerar as coisas!
struct set_t {
    unsigned int elem_count; // Guarda a quantidade de elementos no conjunto.
    unsigned int capacity; // Guarda o tamanho do tabuleiro
    unsigned int *data;
};

// Aloca espaço para um grafo (matriz de adjacência).
struct graph_t*
create_graph(unsigned int n) {
    int i;
    struct graph_t *g;

    if (!(g = malloc(sizeof(struct graph_t)))) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    g->size = n;

    if (!(g->adjacency = calloc(BOARD_SQUARES, sizeof(int *)))) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    for (i = 0; i < BOARD_SQUARES; i++)
        if (!(g->adjacency[i] = calloc(BOARD_SQUARES, sizeof(int)))) {
            fprintf(stderr, "Erro ao alocar memória.\n");
            return NULL;
        }

    return g;
}

// Aloca espaço para um conjunto.
struct set_t*
create_set(unsigned int n) {
    struct set_t *s;
    if (!(s = malloc(sizeof(struct set_t)))) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    if (!(s->data = calloc(BOARD_SQUARES, sizeof(int)))) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    s->elem_count = 0;

    return s;
}

// Transforma uma coordenada do tabuleiro em índice do vetor.
int
get_index_from_position(int board_size, casa c) {
    return board_size * c.linha + c.coluna;
}

// Transforma um índice do vetor em coordenada do tabuleiro.
void
get_position_from_index(int index, int board_size, casa *c) {
    c->linha = index / board_size;
    c->coluna = index % board_size;
}

// Adiciona ou remove uma aresta entre duas casas.
void
update_edge(struct graph_t *graph, casa c1, casa c2, int operation) {
    int v1 = get_index_from_position(graph->size, c1);
    int v2 = get_index_from_position(graph->size, c2);
    graph->adjacency[v1][v2] = operation;
    graph->adjacency[v2][v1] = operation;
}

// Imprime a matriz de adjacência.
void
print_graph(struct graph_t *g) {
    int i, j;
    int n = g->size;

    for (i = 0; i < BOARD_SQUARES; i++) {
        for (j = 0; j < BOARD_SQUARES; j++)
            printf("%d ", g->adjacency[i][j]);
        printf("\n");
    }
}

int
set_contains(struct set_t *s, unsigned int square_index) {
    for (int i = 0; i < s->elem_count; i++)
        if (s->data[i] == square_index)
            return TRUE;

    return FALSE;
}

struct set_t*
append_element(struct set_t *s, casa v) {
    if (s == NULL) {
        fprintf(stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->capacity == s->elem_count) {
        fprintf(stderr, "Erro: capacidade máxima do conjunto já alcançada.\n");
        return s;
    }

    unsigned int square_index = get_index_from_position(s->capacity, v);

    if (set_contains(s, square_index))
        return s;

    s->data[s->elem_count] = square_index;
    s->elem_count += 1;

    return s;
}

struct set_t*
remove_element(struct set_t *s, casa v) {
    if (s == NULL) {
        fprintf(stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->capacity == s->elem_count) {
        fprintf(stderr, "Erro: conjunto não possui elementos.\n");
        return s;
    }

    unsigned int square_index = get_index_from_position(s->capacity, v);

    if (! set_contains(s, square_index))
        return s;
    
    for (int i = 0; i < s->elem_count; i++) {
        if (s->data[i] == square_index) {
            s->data[i] = s->data[s->elem_count - 1];
            s->elem_count--;
            break;
        }
    }

    return s;
}

// Essa função vai adicionar de novo os vizinhos de v depois da chamada
// recursiva.
struct set_t*
add_neighbours(struct graph_t *g, struct set_t *s, casa v) {
    return NULL;
}

// Essa função vai remover de s todos os vizinhos de v. E a casa também.
struct set_t*
remove_neighbours(struct graph_t *g, struct set_t *s, casa v) {
    return NULL;
}

// Cria as arestas entre a casa que possui uma rainha e todas as casas
// proibidas.
void
add_queen_neighbours(struct graph_t *g, casa c) {
    // Adiciona aresta entre todas as casas nos sentidos horizontal e vertical.
    for (int i = 0; i < g->size; i++) {
        if (i != c.coluna) {
            casa c_horizontal;
            c_horizontal.linha = i;
            c_horizontal.coluna = c.coluna;
            update_edge(g, c, c_horizontal, ADD);
        }
        if (i != c.linha) {
            casa c_vertical;
            c_vertical.linha = c.linha;
            c_vertical.coluna = i;
            update_edge(g, c, c_vertical, ADD);
        }
    }

    // Adiciona aresta entre todas as casas nas quatro diagonais.
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (int d = 0; d < 4; d++) {
        int horizontal_dir = directions[d][0];
        int vertical_dir = directions[d][1];
        int i = c.linha + horizontal_dir;
        int j = c.coluna + vertical_dir;

        while (i >= 0 && i < g->size && j >= 0 && j < g->size) {
            casa c_diag;
            c_diag.linha = i;
            c_diag.coluna = j;
            update_edge(g, c, c_diag, ADD);
            i += horizontal_dir;
            j += vertical_dir;
        }
    }
}

// Função que vai realizar todo o trabalho da parte de grafos.
unsigned int*
find_independent_set(struct graph_t *g, struct set_t *i,
                                   struct set_t *c) {
    if (i->elem_count == g->size)
        return i->data;
    else if (i->elem_count + c->elem_count < g->size)
        return NULL;

    casa v;
    // remover um vértice de C
    //  preciso ver como vou fazer tal decisão.

    unsigned int *r =
        find_independent_set(g, append_element(i, v), remove_neighbours(g, c, v));

    i = remove_element (i, v);
    c = add_neighbours(g, c, v);

    return r? r : find_independent_set(g, i, c);
}

//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int*
rainhas_ci(unsigned int n, unsigned int k, casa *c,
                         unsigned int *r) {
    return r;
}
