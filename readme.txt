Alunos - GRR
Luan Marko Kujavski - GRR20221236
Vinicius Evair da Silva - GRR20221251

Backtracking:

A ideia do algoritmo é passar por cada uma das n linhas do tabuleiro encontrando
a(s) melhor(es) posições para colocarmos as rainhas. Ao encontrar uma solução
com n rainhas, o algoritmo retorna ela. Senão, retorna a maior solução possível.

Dado um tabuleiro, existe um pré-processamento que podemos fazer para
descobrirmos aproximadamente o número máximo de rainhas que podemos colocar. Se
uma linha ou uma coluna está inteiramente proibida, esse número diminui. Isso
otimiza o algoritmo já que podemos retornar a primeira solução que tenha o
número máximo de rainhas.

A cada passagem por todo o backtracking, guardamos a melhor solução. Quando
temos uma solução k rainhas, podemos descartar respostas no meio do algoritmo
que não consigam alcançar essas k rainhas mesmo com todas as iterações.

Ademais, ao colocarmos uma rainha na coluna i, não podemos colocar outra na
próxima linha nas colunas [i - 1..i + 1]. Sendo assim, podemos otimizar o
algoritmo para que ele não verifique essas colunas. Assim, como verificamos as
cadas do índice 0 ao n - 1, encontramos vantagem em começar a procurar a melhor
solução nos índices [i + 2..n] e, se não encontrarmos solução com o número
máximo de rainhas, testamos [0..i - 2].

Grafos:

A estrutura utilizada para representar o grafo foi uma matriz de tamanho (n^2) X
(4n - 2). O tamanho é específico para emular uma lista de adjacência utilizando
uma matriz, uma vez que sabemos a quantidade máxima de arestas que cada vértice
pode ter. Com esta estrutura, temos todos os benefícios da lista de adjacência,
mas com o acesso direto de um vetor.

Explicando o valor da quantidade de vizinhos:
  - n - 1 da linha horizontal
  - n - 1 da linha vertical
  - n - 1 + 2*(min(i, j)) da diagonal
  total: 3(n -1) + 2*(min(i, j)) para cada casa
  O maior valor da diagonal acontece no centro, que por sua vez, ocorre em
  ceil(n/2). Logo, cada vértice possui no máximo 3(n - 1) + n = 4n - 3 vizinhos.
  Uma posição a mais é alocada para guardar a quantidade de vizinhos de cada
  casa, chegando no tamanho utilizado para a matriz.

A escolha do vértice a ser removido do conjunto C foi simplesmente o menor
índice encontrado no conjunto, i.e., a primeira casa possível varrendo o
tabuleiro da esquerda para a direita, e de cima para baixo. Desta forma,
garantimos que a próxima casa a ser escolhida siga uma ordem próxima à descrita
no artigo de E. J. Hoffman anexado ao enunciado.
