#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define OBSTACLE 1
#define FREE 0

typedef struct {
  int x, y;
} Coordenada;

typedef struct {
  Coordenada coord;
  int custo;      // g(x)
  int heuristica; // h(x)
  Coordenada pai;
} Nodo;

Coordenada inicio, final;
int mapa[MAX][MAX];
int linhas, colunas;
Nodo listaAberta[MAX * MAX];
Nodo listaFechada[MAX * MAX];
int tamanhoAberta = 0;
int tamanhoFechada = 0;

int distanciaManhattan(Coordenada atual, Coordenada final) {
  return abs(final.x - atual.x) + abs(final.y - atual.y);
}

bool estaNaLista(Coordenada coord, Nodo *lista, int tamanho) {
  for (int i = 0; i < tamanho; i++) {
    if (lista[i].coord.x == coord.x && lista[i].coord.y == coord.y) {
      return true;
    }
  }
  return false;
}

void adicionarLista(Nodo nodo, Nodo *lista, int *tamanho) {
  lista[*tamanho] = nodo;
  (*tamanho)++;
}

void removerLista(Coordenada coord, Nodo *lista, int *tamanho) {
  for (int i = 0; i < *tamanho; i++) {
    if (lista[i].coord.x == coord.x && lista[i].coord.y == coord.y) {
      for (int j = i; j < *tamanho - 1; j++) {
        lista[j] = lista[j + 1];
      }
      (*tamanho)--;
      return;
    }
  }
}

int obterVizinhos(Coordenada atual, Coordenada *vizinhos) {
  int dx[] = {-1, 1, 0, 0};
  int dy[] = {0, 0, -1, 1};
  int count = 0;

  for (int i = 0; i < 4; i++) {
    int nx = atual.x + dx[i];
    int ny = atual.y + dy[i];

    if (nx >= 0 && nx < linhas && ny >= 0 && ny < colunas &&
        mapa[nx][ny] != OBSTACLE) {
      vizinhos[count++] = (Coordenada){nx, ny};
    }
  }
  return count;
}

void ordenarLista(Nodo *lista, int tamanho) {
  for (int i = 0; i < tamanho - 1; i++) {
    for (int j = i + 1; j < tamanho; j++) {
      int custoI = lista[i].custo + lista[i].heuristica;
      int custoJ = lista[j].custo + lista[j].heuristica;
      if (custoI > custoJ) {
        Nodo temp = lista[i];
        lista[i] = lista[j];
        lista[j] = temp;
      }
    }
  }
}

void recuperarCaminho(Coordenada atual) {
  printf("Caminho encontrado:\n");
  while (!(atual.x == inicio.x && atual.y == inicio.y)) {
    printf("(%d, %d) <- ", atual.x, atual.y);
    for (int i = 0; i < tamanhoFechada; i++) {
      if (listaFechada[i].coord.x == atual.x &&
          listaFechada[i].coord.y == atual.y) {
        atual = listaFechada[i].pai;
        break;
      }
    }
  }
  printf("(%d, %d)\n", inicio.x, inicio.y);
}

void buscar() {
  Nodo inicial = {
      inicio, 0, distanciaManhattan(inicio, final), {inicio.x, inicio.y}};
  adicionarLista(inicial, listaAberta, &tamanhoAberta);

  while (tamanhoAberta > 0) {
    ordenarLista(listaAberta, tamanhoAberta);
    Nodo atual = listaAberta[0];
    removerLista(atual.coord, listaAberta, &tamanhoAberta);
    adicionarLista(atual, listaFechada, &tamanhoFechada);

    if (atual.coord.x == final.x && atual.coord.y == final.y) {
      recuperarCaminho(final);
      return;
    }

    Coordenada vizinhos[4];
    int numVizinhos = obterVizinhos(atual.coord, vizinhos);

    for (int i = 0; i < numVizinhos; i++) {
      Coordenada vizinho = vizinhos[i];

      if (estaNaLista(vizinho, listaFechada, tamanhoFechada)) {
        continue;
      }

      int novoCusto = atual.custo + 1;
      bool melhorCaminho = false;

      if (!estaNaLista(vizinho, listaAberta, tamanhoAberta)) {
        melhorCaminho = true;
      } else {
        for (int j = 0; j < tamanhoAberta; j++) {
          if (listaAberta[j].coord.x == vizinho.x &&
              listaAberta[j].coord.y == vizinho.y) {
            if (novoCusto < listaAberta[j].custo) {
              melhorCaminho = true;
            }
            break;
          }
        }
      }

      if (melhorCaminho) {
        Nodo novoNodo = {vizinho, novoCusto, distanciaManhattan(vizinho, final),
                         atual.coord};
        adicionarLista(novoNodo, listaAberta, &tamanhoAberta);
      }
    }
  }

  printf("Caminho não encontrado!\n");
}

void carregarMapa(const char *arquivo) {
  FILE *fp = fopen(arquivo, "r");
  if (!fp) {
    printf("Erro ao abrir o arquivo.\n");
    exit(1);
  }

  fscanf(fp, "%d %d", &linhas, &colunas);
  for (int i = 0; i < linhas; i++) {
    for (int j = 0; j < colunas; j++) {
      fscanf(fp, "%d", &mapa[i][j]);
    }
  }
  fclose(fp);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Uso: %s <arquivo_mapa>\n", argv[0]);
    return 1;
  }

  carregarMapa(argv[1]);

  printf("Digite o ponto inicial (linha coluna): ");
  scanf("%d %d", &inicio.x, &inicio.y);
  printf("Digite o ponto final (linha coluna): ");
  scanf("%d %d", &final.x, &final.y);

  if (mapa[inicio.x][inicio.y] == OBSTACLE ||
      mapa[final.x][final.y] == OBSTACLE) {
    printf("Ponto inicial ou final é um obstáculo.\n");
    return 1;
  }

  buscar();

  return 0;
}
