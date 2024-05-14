#include "rainhas.h"

#include <string.h>

unsigned int *posicionarRainhas(unsigned int *t, unsigned int n, unsigned int r) {
    unsigned int i, j, valido;

    if (r == n) {
        return t;
    }

    for (j = 0; j < n; j++) {
        valido = 1;

        for (i = 0; i < r; i++)
            if (t[i] == j || t[i] == j + r - i || t[i] == j - r + i)
                valido = 0;

        if (valido) {
            t[r] = j;
            if (posicionarRainhas(t, n, r + 1) != NULL)
                return t;
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
    return posicionarRainhas(r, n, 0);
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
