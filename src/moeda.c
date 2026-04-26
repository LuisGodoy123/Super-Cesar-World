#include "moeda.h"
#include <stdlib.h>

static int colidiu_moeda_jogador(NoMoeda *m, Jogador *j) {
	Rectangle retJog = { j->x, j->y, JOGADOR_LARGURA, JOGADOR_ALTURA };
	Rectangle retMoeda = { m->x, m->y, (float)TILE, (float)TILE };
	return VerificarColisao(retJog, retMoeda);
}

NoMoeda *CriarMoeda(float x, float y) {
	NoMoeda *novo = (NoMoeda *)malloc(sizeof(NoMoeda));
	if (novo == NULL) return NULL;

	novo->x = x;
	novo->y = y;
	novo->coletada = 0;
	novo->proximo = NULL;
	return novo;
}

void AdicionarMoeda(NoMoeda **lista, float x, float y) {
	NoMoeda *novo = CriarMoeda(x, y);
	if (novo == NULL) return;

	novo->proximo = *lista;
	*lista = novo;
}

void CarregarMoedasDaFase(NoMoeda **lista, Fase *f) {
	if (lista == NULL || f == NULL) return;

	for (int i = 0; i < LINHAS; i++) {
		for (int j = 0; j < COLUNAS; j++) {
			if (f->mapa[i][j] == MOEDA) {
				AdicionarMoeda(lista, (float)(j * TILE), (float)(i * TILE));
				f->mapa[i][j] = VAZIO;
			}
		}
	}
}

void AtualizarMoedas(NoMoeda *lista, Jogador *j) {
	if (j == NULL || j->estado == MORTO) return;

	NoMoeda *atual = lista;
	while (atual != NULL) {
		if (!atual->coletada && colidiu_moeda_jogador(atual, j)) {
			atual->coletada = 1;
			j->pontos += PONTOS_MOEDA;
		}
		atual = atual->proximo;
	}
}

void DesenharMoedas(NoMoeda *lista, float cameraX) {
	NoMoeda *atual = lista;
	while (atual != NULL) {
		if (!atual->coletada) {
			int screenX = (int)(atual->x - cameraX);
			int screenY = (int)(atual->y);

			if (screenX + TILE >= 0 && screenX <= GetScreenWidth()) {
				int cx = screenX + TILE / 2;
				int cy = screenY + TILE / 2;
				DrawCircle(cx, cy, (float)TILE * 0.30f, GOLD);
				DrawCircleLines(cx, cy, (float)TILE * 0.30f, ORANGE);
			}
		}
		atual = atual->proximo;
	}
}

void LiberarMoedas(NoMoeda *lista) {
	while (lista != NULL) {
		NoMoeda *tmp = lista;
		lista = lista->proximo;
		free(tmp);
	}
}
