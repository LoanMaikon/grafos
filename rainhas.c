#include "rainhas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_SQUARES ((n) * (n)) // Squares amount will always be n^2
#define MAX_NEIGHBOURS (4 * (n) - 2)
#define ADD 1
#define REM 0
#define TRUE 1
#define FALSE 0

#define MAX(a, b) ((a) > (b) ? (a) : (b))

//-------------------------- Prototypes ---------------------------------------

int
isValid (unsigned int n, unsigned int *t, unsigned int r, unsigned int j,
         unsigned int *prohibitedSpotsVector);
void
placeQueens (unsigned int *t, unsigned int n, unsigned int r,
             unsigned int *bestSolution, unsigned int *maxRainhas,
             unsigned int *foundSolution, unsigned int maxQueensPossible,
             unsigned int *prohibitedSpotsVector, unsigned int actualNQueens,
             unsigned int lastColumn);
unsigned int
isProhibited (unsigned int n, unsigned int line, unsigned int column,
              unsigned int *prohibitedSpotsVector);
unsigned int
isLineProhibited (unsigned int line, unsigned int n,
                  unsigned int *prohibitedSpotsVector);
unsigned int
isColumnProhibited (unsigned int column, unsigned int n,
                    unsigned int *prohibitedSpotsVector);
unsigned int
calculateMaxQueensPossible (unsigned int n,
                            unsigned int *prohibitedSpotsVector);
unsigned int *
makeProhibitedSpotsVector (unsigned int n, unsigned int k,
                           casa *prohibitedSpots);
struct graph_t *
create_graph (unsigned int n);
void
delete_graph (struct graph_t *g);
struct set_t *
create_set (unsigned int n);
void
copy_set_data (struct set_t *dest, struct set_t *source);
struct set_t *
copy_set (struct set_t *original);
void
delete_set (struct set_t *s);
unsigned int
get_index_from_square (casa v, unsigned int board_size);
struct vertex_t
get_smaller_element (struct set_t *s);
void
add_edge (struct graph_t *graph, struct vertex_t v1, struct vertex_t v2);
void
print_graph (struct graph_t *g);
int
set_contains (struct set_t *s, struct vertex_t v);
struct set_t *
append_element (struct set_t *s, struct vertex_t v);
struct set_t *
remove_element (struct set_t *s, struct vertex_t v);
struct set_t *
remove_neighbours_set (struct graph_t *g, struct set_t *s, struct vertex_t c);
void
add_neighbours_board (struct graph_t *g, struct vertex_t v);
void
find_independent_set (struct graph_t *g, struct set_t *i, struct set_t *c,
                      struct set_t *m);

//-------------------------- Parte backtracking -------------------------------

unsigned int
isProhibited (unsigned int n, unsigned int line, unsigned int column,
              unsigned int *prohibitedSpotsVector) {
    if (prohibitedSpotsVector[n * line + column] == 1) {
        return TRUE;
    }

    return FALSE;
}

int
isValid (unsigned int n, unsigned int *t, unsigned int r, unsigned int j,
         unsigned int *prohibitedSpotsVector) {
    if (isProhibited (n, r, j, prohibitedSpotsVector)) {
        return FALSE;
    }

    for (unsigned int i = 0; i < r; i++) {
        if (t[i] == 0) {
            continue;
        }

        if (t[i] == j + 1 || t[i] == j + 1 + r - i || t[i] == j + 1 - r + i) {
            return FALSE;
        }
    }

    return TRUE;
}

unsigned int
isLineProhibited (unsigned int line, unsigned int n,
                  unsigned int *prohibitedSpotsVector) {
    for (unsigned int column = 0; column < n; column++) {
        if (!isProhibited (n, line, column, prohibitedSpotsVector)) {
            return FALSE;
        }
    }

    return TRUE;
}

unsigned int
isColumnProhibited (unsigned int column, unsigned int n,
                    unsigned int *prohibitedSpotsVector) {
    for (unsigned int line = 0; line < n; line++) {
        if (!isProhibited (n, line, column, prohibitedSpotsVector)) {
            return FALSE;
        }
    }

    return TRUE;
}

unsigned int
calculateMaxQueensPossible (unsigned int n,
                            unsigned int *prohibitedSpotsVector) {
    unsigned int prohibitedLines = 0;
    for (unsigned int line = 0; line < n; line++) {
        if (isLineProhibited (line, n, prohibitedSpotsVector)) {
            prohibitedLines++;
        }
    }

    unsigned int prohibitedColumns = 0;
    for (unsigned int column = 0; column < n; column++) {
        if (isColumnProhibited (column, n, prohibitedSpotsVector)) {
            prohibitedColumns++;
        }
    }

    return n - MAX (prohibitedLines, prohibitedColumns);
}

unsigned int *
makeProhibitedSpotsVector (unsigned int n, unsigned int k,
                           casa *prohibitedSpots) {
    unsigned int *prohibitedSpotsVector =
        (unsigned int *)calloc (TOTAL_SQUARES, sizeof (unsigned int));

    for (unsigned int i = 0; i < k; i++) {
        unsigned int index = get_index_from_square (prohibitedSpots[i], n);
        prohibitedSpotsVector[index] = 1;
    }

    return prohibitedSpotsVector;
}

void
placeQueens (unsigned int *t, unsigned int n, unsigned int r,
             unsigned int *bestSolution, unsigned int *maxQueens,
             unsigned int *foundSolution, unsigned int maxQueensPossible,
             unsigned int *prohibitedSpotsVector, unsigned int actualNQueens,
             unsigned int lastColumn) {
    if (*foundSolution) {
        return;
    }

    if (actualNQueens + n - r < *maxQueens) {
        return;
    }

    if (r == n) {
        if (actualNQueens > *maxQueens) {
            *maxQueens = actualNQueens;
            for (unsigned int i = 0; i < n; i++) {
                bestSolution[i] = t[i];
            }
        }

        if (actualNQueens == maxQueensPossible) {
            *foundSolution = TRUE;
        }

        return;
    }

    unsigned int invalidCount = 0;
    if (lastColumn == 0) {
        for (unsigned int j = 0; j < n; j++) {
            if (isValid (n, t, r, j, prohibitedSpotsVector)) {
                t[r] = j + 1;
                placeQueens (t, n, r + 1, bestSolution, maxQueens,
                             foundSolution, maxQueensPossible,
                             prohibitedSpotsVector, actualNQueens + 1, j);
            } else {
                invalidCount++;
            }
        }
    } else {
        invalidCount++;
        if (lastColumn >= 1) {
            invalidCount++;
        }
        if (lastColumn + 1 < n) {
            invalidCount++;
        }

        for (unsigned int j = lastColumn + 2; j < n; j++) {
            if (isValid (n, t, r, j, prohibitedSpotsVector)) {
                t[r] = j + 1;
                placeQueens (t, n, r + 1, bestSolution, maxQueens,
                             foundSolution, maxQueensPossible,
                             prohibitedSpotsVector, actualNQueens + 1, j);
            } else {
                invalidCount++;
            }
        }

        for (unsigned int j = 0; j < lastColumn - 1; j++) {
            if (isValid (n, t, r, j, prohibitedSpotsVector)) {
                t[r] = j + 1;
                placeQueens (t, n, r + 1, bestSolution, maxQueens,
                             foundSolution, maxQueensPossible,
                             prohibitedSpotsVector, actualNQueens + 1, j);
            } else {
                invalidCount++;
            }
        }
    }

    if (invalidCount == n) {
        t[r] = 0;
        placeQueens (t, n, r + 1, bestSolution, maxQueens, foundSolution,
                     maxQueensPossible, prohibitedSpotsVector, actualNQueens,
                     lastColumn);
    }
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
unsigned int *
rainhas_bt (unsigned int n, unsigned int k, casa *c, unsigned int *r) {
    unsigned int maxQueens = 0;
    unsigned int *t = (unsigned int *)calloc (n, sizeof (unsigned int));
    unsigned int *bestSolution =
        (unsigned int *)calloc (n, sizeof (unsigned int));
    unsigned int foundSolution = FALSE;

    unsigned int *prohibitedSpotsVector = makeProhibitedSpotsVector (n, k, c);
    unsigned int maxQueensPossible =
        calculateMaxQueensPossible (n, prohibitedSpotsVector);

    placeQueens (t, n, 0, bestSolution, &maxQueens, &foundSolution,
                 maxQueensPossible, prohibitedSpotsVector, 0, 0);

    for (unsigned int i = 0; i < n; i++) {
        r[i] = bestSolution[i];
    }

    free (t);
    free (bestSolution);
    free (prohibitedSpotsVector);

    return r;
}

//----------------------------- Parte grafos ----------------------------------
struct vertex_t {
    casa square;        // Guarda a posição da casa no tabuleiro
    unsigned int index; // Guarda a posição da casa no vetor principal do grafo
};

struct graph_t {
    struct vertex_t **adjacency;
    unsigned int size; // Guarda o tamanho do tabuleiro, e não do grafo.
};

struct set_t {
    unsigned int elem_count; // Guarda a quantidade de elementos no conjunto.
    unsigned int capacity;   // Guarda o tamanho do conjunto
    struct vertex_t *data;
};

struct graph_t *
create_graph (unsigned int n) {
    unsigned int i;
    struct graph_t *g;

    if (!(g = malloc (sizeof (struct graph_t)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    g->size = n;

    if (!(g->adjacency = calloc (TOTAL_SQUARES, sizeof (struct vertex_t *)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    for (i = 0; i < TOTAL_SQUARES; i++)
        if (!(g->adjacency[i] =
                  calloc (MAX_NEIGHBOURS, sizeof (struct vertex_t)))) {
            fprintf (stderr, "Erro ao alocar memória.\n");
            return NULL;
        }

    return g;
}

void
delete_graph (struct graph_t *g) {
    if (g) {
        unsigned int n = g->size;
        for (unsigned int i = 0; i < TOTAL_SQUARES; i++)
            free (g->adjacency[i]);
        free (g->adjacency);
        free (g);
    }
}

struct set_t *
create_set (unsigned int n) {
    struct set_t *s;
    if (!(s = malloc (sizeof (struct set_t)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    if (!(s->data = calloc (n, sizeof (struct vertex_t)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    s->capacity = n;
    s->elem_count = 0;

    return s;
}

void
copy_set_data (struct set_t *dest, struct set_t *source) {
    dest->elem_count = source->elem_count;
    memcpy (dest->data, source->data,
            source->capacity * sizeof (struct vertex_t));
}

struct set_t *
copy_set (struct set_t *original) {
    if (original == NULL)
        return NULL;

    struct set_t *copy = create_set (original->capacity);
    copy_set_data (copy, original);

    return copy;
}

void
delete_set (struct set_t *s) {
    if (s) {
        free (s->data);
        free (s);
    }
}

// Transforma uma coordenada do tabuleiro em índice do vetor.
unsigned int
get_index_from_square (casa v, unsigned int board_size) {
    return board_size * (v.linha - 1) + (v.coluna - 1);
}

struct vertex_t
get_smaller_element (struct set_t *s) {
    struct vertex_t min_element;
    min_element.index = s->capacity + 1;
    for (unsigned int i = 0; i < s->elem_count; i++) {
        if (s->data[i].index < min_element.index)
            min_element = s->data[i];
    }

    return min_element;
}

// Adiciona ou remove uma aresta entre duas casas.
void
add_edge (struct graph_t *graph, struct vertex_t v1, struct vertex_t v2) {
    unsigned int last_position = graph->adjacency[v1.index][0].index + 1;
    graph->adjacency[v1.index][last_position] = v2;
    graph->adjacency[v1.index][0].index++;
}

int
set_contains (struct set_t *s, struct vertex_t v) {
    for (unsigned int i = 0; i < s->elem_count; i++)
        if (s->data[i].index == v.index)
            return TRUE;

    return FALSE;
}

struct set_t *
append_element (struct set_t *s, struct vertex_t v) {
    if (s == NULL) {
        fprintf (stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->capacity == s->elem_count) {
        return s;
    }

    if (set_contains (s, v))
        return s;

    s->data[s->elem_count] = v;
    s->elem_count += 1;

    return s;
}

struct set_t *
remove_element (struct set_t *s, struct vertex_t v) {
    if (s == NULL) {
        fprintf (stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->elem_count == 0) {
        return s;
    }

    for (unsigned int i = 0; i < s->elem_count; i++) {
        if (s->data[i].index == v.index) {
            s->data[i] = s->data[s->elem_count - 1];
            s->elem_count--;
            break;
        }
    }

    return s;
}

struct set_t *
remove_neighbours_set (struct graph_t *g, struct set_t *s, struct vertex_t c) {
    unsigned int neighbour_amout = g->adjacency[c.index][0].index;
    for (unsigned int i = 1; i <= neighbour_amout; i++) {
        remove_element (s, g->adjacency[c.index][i]);
    }

    return s;
}

// Cria as arestas entre a casa que possui uma rainha e todas as casas
// proibidas.
void
add_neighbours_board (struct graph_t *g, struct vertex_t v) {
    // Adiciona aresta entre todas as casas nos sentidos horizontal e vertical.
    for (unsigned int i = 1; i <= g->size; i++) {
        if (i != v.square.linha) {
            struct vertex_t c_vertical;
            c_vertical.square.linha = i;
            c_vertical.square.coluna = v.square.coluna;
            c_vertical.index =
                get_index_from_square (c_vertical.square, g->size);
            add_edge (g, v, c_vertical);
        }
        if (i != v.square.coluna) {
            struct vertex_t c_horizontal;
            c_horizontal.square.linha = v.square.linha;
            c_horizontal.square.coluna = i;
            c_horizontal.index =
                get_index_from_square (c_horizontal.square, g->size);
            add_edge (g, v, c_horizontal);
        }
    }

    // Adiciona aresta entre todas as casas nas quatro diagonais.
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (int d = 0; d < 4; d++) {
        int horizontal_dir = directions[d][0];
        int vertical_dir = directions[d][1];
        int i = (int)v.square.linha + horizontal_dir;
        int j = (int)v.square.coluna + vertical_dir;
        int board_size = (int)g->size;

        while (i > 0 && i <= board_size && j > 0 && j <= board_size) {
            struct vertex_t c_diag;
            c_diag.square.linha = (unsigned int)i;
            c_diag.square.coluna = (unsigned int)j;
            c_diag.index = get_index_from_square (c_diag.square, g->size);
            add_edge (g, v, c_diag);
            i += horizontal_dir;
            j += vertical_dir;
        }
    }
}

// Função que realiza todo o trabalho da parte de grafos.
void
find_independent_set (struct graph_t *board, struct set_t *independent_set,
                      struct set_t *candidates_set, struct set_t *maximal_set) {
    if (maximal_set->elem_count == maximal_set->capacity)
        return;
    if (independent_set->elem_count > maximal_set->elem_count)
        copy_set_data (maximal_set, independent_set);
    if (candidates_set->elem_count == 0)
        return;

    struct vertex_t candidate;
    struct set_t *candidates_copy_set = copy_set (candidates_set);
    candidate = get_smaller_element (candidates_set);
    candidates_copy_set = remove_element (candidates_copy_set, candidate);

    find_independent_set (
        board, append_element (independent_set, candidate),
        remove_neighbours_set (board, candidates_copy_set, candidate),
        maximal_set);

    delete_set (candidates_copy_set);
    candidates_copy_set = NULL;
    independent_set = remove_element (independent_set, candidate);
    candidates_set = remove_element (candidates_set, candidate);

    find_independent_set (board, independent_set, candidates_set, maximal_set);
}

//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int *
rainhas_ci (unsigned int n, unsigned int k, casa *c, unsigned int *r) {
    struct graph_t *board = create_graph (n);
    struct set_t *independent_set = create_set (n);
    struct set_t *maximal_set = create_set (n);
    struct set_t *candidates_set = create_set (TOTAL_SQUARES);
    struct set_t *prohibited_set = create_set (k);
    memset (r, 0, n * sizeof (unsigned int));

    for (unsigned int i = 0; i < k; i++) {
        prohibited_set->data[i].square = c[i];
        unsigned int index = get_index_from_square (c[i], n);
        prohibited_set->data[i].index = index;
    }

    for (unsigned int i = 0; i < candidates_set->capacity; i++) {
        struct vertex_t vertice;
        vertice.square.linha = (i / board->size) + 1;
        vertice.square.coluna = (i % board->size) + 1;
        vertice.index = i;
        candidates_set->data[i] = vertice;
        candidates_set->elem_count++;
        add_neighbours_board (board, vertice);
    }

    for (unsigned int i = 0; i < k; i++) {
        remove_element (candidates_set, prohibited_set->data[i]);
    }

    find_independent_set (board, independent_set, candidates_set, maximal_set);
    for (unsigned int i = 0; i < maximal_set->elem_count; i++) {
        unsigned int queen_line = maximal_set->data[i].square.linha - 1;
        unsigned int queen_column = maximal_set->data[i].square.coluna;
        r[queen_line] = queen_column;
    }

    delete_set (independent_set);
    delete_set (maximal_set);
    delete_set (candidates_set);
    delete_set (prohibited_set);
    delete_graph (board);

    return r;
}
