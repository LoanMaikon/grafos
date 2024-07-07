#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rainhas.h"

static clock_t inicio, fim;

//------------------------------------------------------------------------------
#define CRONOMETRA(call,t) inicio = clock(); (call); fim = clock(); t=fim-inicio

//------------------------------------------------------------------------------
static void mostra_resposta(unsigned int n, unsigned int *r, FILE *arq) {

  for (unsigned int i=0; i<n; i++) {
    fprintf(arq, "%u ", r[i]);
  }
  fprintf(arq, "\n");
}
//------------------------------------------------------------------------------
// preenche proibido[0..2n-1] com as posições das diagonais do tabuleiro n x n
//
// devolve &(proibido[2n]): o endereço a partir do qual proibir novas posições

static casa *proibe_diagonais(unsigned int n, casa *proibido) {

  // proíbe todas as casas nas diagonais
  for(unsigned int i = 0, p = 1; i < 2 * n; i+=2, p++) {
    // diagonal "principal"
    proibido[i].linha = proibido[i].coluna = p;

    // "outra" diagonal
    proibido[i+1].linha = p;
    proibido[i+1].coluna = n - p + 1;
  }

  return proibido + 2*n;
}

static void proibe_casas(unsigned int *tabuleiro, unsigned int tamanho, casa *proibido, unsigned int n, FILE *arq){
  int pos = 0;
  fprintf (arq, "tabuleiro:\n");
  for (unsigned int i = 0; i < n; i++){
    for (unsigned int j = 0; j < n; j++){
      if (tabuleiro[i*n + j] == 1){
        proibido[pos].linha = i + 1;
        proibido[pos].coluna = j + 1;
        pos++;
      }
      fprintf(arq, "%u ", tabuleiro[i*n + j]);
    }
    fprintf(arq, "\n");
  }
}

static void testa(unsigned int *tabuleiro, unsigned int n, unsigned int tamanho, unsigned int *resposta, FILE *arq){
  casa *proibido = malloc(tamanho*sizeof(casa));
  proibe_casas(tabuleiro, tamanho, proibido, n, arq);
  fprintf(arq, "\nbacktracking: ");
  long int tempo_bt;
  CRONOMETRA(rainhas_bt(n, tamanho, proibido, resposta), tempo_bt);
  // fprintf(arq, "%ld\n", tempo_bt);
  mostra_resposta(n, resposta, arq);
  fprintf(arq, "grafo: ");
  long int tempo_ci;
  CRONOMETRA(rainhas_ci(n, tamanho, proibido, resposta), tempo_ci);
  // fprintf(arq, "%ld\n", tempo_ci);
  mostra_resposta(n, resposta, arq);
  // fprintf(arq, "%.2f\n", (double)tempo_ci/(double)tempo_bt);
  printf("\n");

  free(proibido);
}

static void todos_os_testes(unsigned int *tabuleiro, unsigned int n, unsigned int tamanho, int posicao, unsigned int *resposta, FILE *arq){
  if (posicao == n * n){
    testa(tabuleiro, n, tamanho, resposta, arq);
    return;
  }
  tabuleiro[posicao] = 0;
  todos_os_testes(tabuleiro, n, tamanho, posicao + 1, resposta, arq);
  tabuleiro[posicao] = 1;
  todos_os_testes(tabuleiro, n, tamanho + 1, posicao + 1, resposta, arq);
}

//------------------------------------------------------------------------------
int main (void) {
  unsigned int n = 4;
  unsigned int *resposta = malloc(n*sizeof(unsigned int));
  unsigned int *tabuleiro = malloc(n*n*sizeof(unsigned int));
  todos_os_testes(tabuleiro, n, 0, 0, resposta, stdout);
  free(resposta);
  free(tabuleiro);
  return 0;
}
