#include <stdio.h>
#include <stdlib.h>

void imprimeVetor(int *t, int n) {
    int i;

    for (i = 0; i < n; i++)
        printf("%d ", t[i]);
    printf("\n");
}

void imprimeSolucao(int *t, int n) {
    int i, j;

    printf("\nVetor:\n");
    imprimeVetor(t, n);
    printf("Tabuleiro:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (t[i] == j)
                printf("|Q|");
            else
                printf("| |");;
        }
        printf("\n");
    }
    printf("\n\n-----------------------\n");
}

void posicionarRainhas(int *t, int n, int r, int *count) {
    int i, j, valido;

    if (r == n) {
        (*count)++;
        imprimeSolucao(t, n);
        return;
    }

    for (j = 0; j < n; j++) {
        valido = 1;

        for (i = 0; i < r; i++)
            if (t[i] == j || t[i] == j + r - i || t[i] == j - r + i)
                valido = 0;

        if (valido) {
            t[r] = j;
            posicionarRainhas(t, n, r + 1, count);
        }
    }
}

int main () {
    int *t;
    int n, r, count;

    scanf("%d", &n);
    r = 0;

    if (! (t = calloc (n, sizeof (int))))
        return 1;

    count = 0;
    posicionarRainhas(t, n, r, &count);
    printf("O total de soluções possíveis foi %d.\n", count);

    free(t);

    return 0;
}
