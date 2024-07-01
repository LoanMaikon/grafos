#include "rainhas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SQUARES (n * n) // Board size will always be n^2.
#define ADD 1
#define REM 0
#define TRUE 1
#define FALSE 0

//-------------------------- Parte backtracking -------------------------------
int
isValid (unsigned int *t, unsigned int r, unsigned int j, casa *c,
         unsigned int k) {
    for (unsigned int i = 0; i < k; i++) {
        if (c[i].linha == r + 1 && c[i].coluna == j + 1) {
            return FALSE;
        }
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

void
posicionarRainhas (unsigned int *t, unsigned int n, unsigned int r, casa *c,
                   unsigned int k, unsigned int *bestSolution,
                   unsigned int *maxRainhas) {
    if (r == n) {
        unsigned int count = 0;
        for (unsigned int i = 0; i < n; i++) {
            if (t[i] > 0) {
                count++;
            }
        }

        if (count > *maxRainhas) {
            *maxRainhas = count;
            for (unsigned int i = 0; i < n; i++) {
                bestSolution[i] = t[i];
            }
        }

        // if (count == n) {
        //     return count;
        // }

        return;
    }

    unsigned int cont = 0;
    for (unsigned int j = 0; j < n; j++) {
        if (isValid (t, r, j, c, k)) {
            t[r] = j + 1;
            posicionarRainhas (t, n, r + 1, c, k, bestSolution, maxRainhas);
        } else {
            cont++;
        }
    }

    if (cont == n) {
        t[r] = 0;
        posicionarRainhas (t, n, r + 1, c, k, bestSolution, maxRainhas);
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
    unsigned int maxRainhas = 0;
    unsigned int *t = (unsigned int *)calloc (n, sizeof (unsigned int));
    unsigned int *bestSolution =
        (unsigned int *)calloc (n, sizeof (unsigned int));

    posicionarRainhas (t, n, 0, c, k, bestSolution, &maxRainhas);

    for (unsigned int i = 0; i < n; i++) {
        r[i] = bestSolution[i];
    }

    free (t);
    free (bestSolution);

    return r;
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
    unsigned int capacity;   // Guarda o tamanho do conjunto
    unsigned int *data;
};

// Aloca espaço para um grafo (matriz de adjacência).
struct graph_t *
create_graph (unsigned int n) {
    int i;
    struct graph_t *g;

    if (!(g = malloc (sizeof (struct graph_t)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    g->size = n;

    if (!(g->adjacency = calloc (BOARD_SQUARES, sizeof (int *)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    for (i = 0; i < BOARD_SQUARES; i++)
        if (!(g->adjacency[i] = calloc (BOARD_SQUARES, sizeof (int)))) {
            fprintf (stderr, "Erro ao alocar memória.\n");
            return NULL;
        }

    return g;
}

// Aloca espaço para um conjunto.
struct set_t *
create_set (unsigned int n) {
    struct set_t *s;
    if (!(s = malloc (sizeof (struct set_t)))) {
        fprintf (stderr, "Erro ao alocar memória.\n");
        return NULL;
    }
    if (!(s->data = calloc (n, sizeof (int)))) {
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
    memcpy (dest->data, source->data, source->capacity * sizeof (unsigned int));
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
int
get_index_from_position (unsigned int board_size, casa c) {
    return board_size * (c.linha - 1) + (c.coluna - 1);
}

// Transforma um índice do vetor em coordenada do tabuleiro.
casa
get_position_from_index (unsigned int index, unsigned int board_size) {
    casa c;
    c.linha = (index / board_size) + 1;
    c.coluna = (index % board_size) + 1;

    return c;
}

unsigned int
get_smaller_element (struct set_t *s) {
    unsigned int min_element = s->capacity + 1;
    for (int i = 0; i < s->elem_count; i++) {
        if (s->data[i] < min_element)
            min_element = s->data[i];
    }

    return min_element;
}

casa
next_candidate (struct graph_t *graph, struct set_t *c) {
    casa candidate;
    unsigned int first_element = get_smaller_element (c);

    candidate = get_position_from_index (first_element, graph->size);
    return candidate;
}

// Adiciona ou remove uma aresta entre duas casas.
void
update_edge (struct graph_t *graph, casa c1, casa c2, int operation) {
    int v1 = get_index_from_position (graph->size, c1);
    int v2 = get_index_from_position (graph->size, c2);
    graph->adjacency[v1][v2] = operation;
    graph->adjacency[v2][v1] = operation;
}

// Imprime a matriz de adjacência.
void
print_graph (struct graph_t *g) {
    int i, j;
    int n = g->size;

    for (i = 0; i < BOARD_SQUARES; i++) {
        for (j = 0; j < BOARD_SQUARES; j++)
            printf ("%d ", g->adjacency[i][j]);
        printf ("\n");
    }
}

int
set_contains (struct set_t *s, unsigned int square_index) {
    for (int i = 0; i < s->elem_count; i++)
        if (s->data[i] == square_index)
            return TRUE;

    return FALSE;
}

struct set_t *
append_element (struct set_t *s, casa v, unsigned int board_size) {
    if (s == NULL) {
        fprintf (stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->capacity == s->elem_count) {
        return s;
    }

    unsigned int square_index = get_index_from_position (board_size, v);

    if (set_contains (s, square_index))
        return s;

    s->data[s->elem_count] = square_index;
    s->elem_count += 1;

    return s;
}

struct set_t *
remove_element (struct set_t *s, casa v, unsigned int board_size) {
    if (s == NULL) {
        fprintf (stderr, "Erro: ponteiro para conjunto não encontrado.\n");
        return NULL;
    } else if (s->elem_count == 0) {
        return s;
    }
    unsigned int square_index = get_index_from_position (board_size, v);

    if (!set_contains (s, square_index))
        return s;

    for (int i = 0; i < s->elem_count; i++) {
        if (s->data[i] == square_index) {
            casa aux = get_position_from_index (s->data[s->elem_count - 1],
                                                board_size);
            s->data[i] = s->data[s->elem_count - 1];
            s->elem_count--;
            break;
        }
    }

    return s;
}

struct set_t *
add_neighbours_set (struct graph_t *g, struct set_t *s, casa c) {
    unsigned int n = g->size;
    unsigned int square_index = get_index_from_position (g->size, c);
    for (unsigned int i = 0; i < n; i++) {
        if (g->adjacency[i][square_index] == TRUE) {
            casa neighbour = get_position_from_index (i, g->size);
            append_element (s, neighbour, g->size);
        }
    }

    return s;
}

struct set_t *
remove_neighbours_set (struct graph_t *g, struct set_t *s, casa c) {
    unsigned int n = g->size;
    unsigned int square_index = get_index_from_position (g->size, c);
    for (unsigned int i = 0; i < BOARD_SQUARES; i++) {
        if (g->adjacency[i][square_index] == TRUE) {
            casa neighbour = get_position_from_index (i, g->size);
            remove_element (s, neighbour, g->size);
        }
    }

    return s;
}

// Cria as arestas entre a casa que possui uma rainha e todas as casas
// proibidas.
void
add_neighbours_board (struct graph_t *g, casa v, struct set_t *c) {
    // Adiciona aresta entre todas as casas nos sentidos horizontal e vertical.
    for (int i = 1; i <= g->size; i++) {
        if (i != v.coluna) {
            casa c_horizontal;
            c_horizontal.linha = i;
            c_horizontal.coluna = v.coluna;
            update_edge (g, v, c_horizontal, ADD);
        }
        if (i != v.linha) {
            casa c_vertical;
            c_vertical.linha = v.linha;
            c_vertical.coluna = i;
            update_edge (g, v, c_vertical, ADD);
        }
    }

    // Adiciona aresta entre todas as casas nas quatro diagonais.
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (int d = 0; d < 4; d++) {
        int horizontal_dir = directions[d][0];
        int vertical_dir = directions[d][1];
        int i = v.linha + horizontal_dir;
        int j = v.coluna + vertical_dir;

        while (i > 0 && i <= g->size && j > 0 && j <= g->size) {
            casa c_diag;
            c_diag.linha = i;
            c_diag.coluna = j;
            update_edge (g, v, c_diag, ADD);
            i += horizontal_dir;
            j += vertical_dir;
        }
    }

    // Adiciona aresta entre todas as casas proibídas
    for (int p = 0; p < c->elem_count; p++) {
        casa prohibited = get_position_from_index(c->data[p], g->size);
        update_edge (g, v, prohibited, ADD);
    }
}

// Função que realiza todo o trabalho da parte de grafos.
unsigned int *
find_independent_set (struct graph_t *g, struct set_t *i, struct set_t *c,
                      struct set_t *m) {
    if (m->elem_count == m->capacity)
        return m->data;
    if (i->elem_count > m->elem_count)
        copy_set_data (m, i);
    if (c->elem_count == 0)
        return i->data;

    casa v;
    struct set_t *c_copy = copy_set (c);
    v = next_candidate (g, c);
    c_copy = remove_element (c_copy, v, g->size);

    find_independent_set (g, append_element (i, v, g->size),
                          remove_neighbours_set (g, c_copy, v), m);

    delete_set (c_copy);
    c_copy = NULL;
    i = remove_element (i, v, g->size);
    c = remove_element (c, v, g->size);

    return find_independent_set (g, i, c, m);
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
    struct set_t *candidates_set = create_set (BOARD_SQUARES);
    struct set_t *prohibited_set = create_set (k);
    memset (r, 0, n * sizeof (unsigned int));

    for (int i = 0; i < k; i++) {
        unsigned int index = get_index_from_position(n, c[i]);
        prohibited_set->data[i] = index;
    }
    for (int i = 0; i < candidates_set->capacity; i++) {
        candidates_set->data[i] = i;
        candidates_set->elem_count++;
        casa vertice = get_position_from_index (i, board->size);
        add_neighbours_board (board, vertice, prohibited_set);
    }

    for (int i = 0; i < 2 * n; i++)
        remove_element (candidates_set, c[i], n);

    find_independent_set (board, independent_set, candidates_set, maximal_set);
    for (int i = 0; i < maximal_set->elem_count; i++) {
        casa aux = get_position_from_index (maximal_set->data[i], n);
        r[aux.linha - 1] = aux.coluna;
    }

    return r;
}
