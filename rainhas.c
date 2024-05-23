#include "rainhas.h"

#include <string.h>


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
