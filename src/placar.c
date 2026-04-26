#include "placar.h"
#include <stdio.h>

static const char *ARQ_PLACAR = "data/placar.dat";

/* inicializa vetor de top scores com zero */
static void limpar_top_scores(Placar *p) {
	for (int i = 0; i < TOP_SCORES; i++) p->topScores[i] = 0;
}

/* ------------------------------------------------------------------ */
/* IniciarPlacar -- inicializa valores padrao                         */
/* ------------------------------------------------------------------ */

void IniciarPlacar(Placar *p) {
	if (p == NULL) return;

	p->pontuacao = 0;
	p->vidas = 3;
	p->faseAtual = 1;
	limpar_top_scores(p);
}

/* ------------------------------------------------------------------ */
/* AtualizarPlacar -- sincroniza dados atuais da partida               */
/* ------------------------------------------------------------------ */

void AtualizarPlacar(Placar *p, int pontuacao, int vidas, int faseAtual) {
	if (p == NULL) return;

	p->pontuacao = pontuacao;
	p->vidas = vidas;
	p->faseAtual = faseAtual;
}

/* ------------------------------------------------------------------ */
/* CarregarPlacar -- leitura de arquivo (data/placar.dat)             */
/* ------------------------------------------------------------------ */

void CarregarPlacar(Placar *p) {
	if (p == NULL) return;

	FILE *f = fopen(ARQ_PLACAR, "r");
	if (f == NULL) {
		limpar_top_scores(p);
		return;
	}

	for (int i = 0; i < TOP_SCORES; i++) {
		if (fscanf(f, "%d", &p->topScores[i]) != 1) p->topScores[i] = 0;
	}

	fclose(f);
}

/* ------------------------------------------------------------------ */
/* SalvarPlacar -- escrita de arquivo (data/placar.dat)               */
/* ------------------------------------------------------------------ */

void SalvarPlacar(Placar *p) {
	if (p == NULL) return;

	FILE *f = fopen(ARQ_PLACAR, "w");
	if (f == NULL) return;

	for (int i = 0; i < TOP_SCORES; i++) fprintf(f, "%d\n", p->topScores[i]);

	fclose(f);
}

/* ------------------------------------------------------------------ */
/* RegistrarPontuacaoFinal -- insere no top 5 em ordem decrescente    */
/* ------------------------------------------------------------------ */

void RegistrarPontuacaoFinal(Placar *p, int pontuacaoFinal) {
	if (p == NULL) return;

	int pos = -1;
	for (int i = 0; i < TOP_SCORES; i++) {
		if (pontuacaoFinal > p->topScores[i]) {
			pos = i;
			break;
		}
	}

	if (pos < 0) return;

	for (int i = TOP_SCORES - 1; i > pos; i--) p->topScores[i] = p->topScores[i - 1];

	p->topScores[pos] = pontuacaoFinal;
}

/* ------------------------------------------------------------------ */
/* DesenharPlacar -- HUD da partida (pontos, vidas e fase)            */
/* ------------------------------------------------------------------ */

void DesenharPlacar(Placar *p) {
	if (p == NULL) return;

	DrawText(TextFormat("Pontos: %d", p->pontuacao), 20, 20, 24, WHITE);
	DrawText(TextFormat("Vidas: %d", p->vidas), 20, 50, 24, WHITE);
	DrawText(TextFormat("Fase: %d", p->faseAtual), 20, 80, 24, WHITE);
}

/* ------------------------------------------------------------------ */
/* DesenharTopScores -- mostra ranking de pontuacoes                  */
/* ------------------------------------------------------------------ */

void DesenharTopScores(Placar *p, int x, int y) {
	if (p == NULL) return;

	DrawText("TOP 5 SCORES", x, y, 32, YELLOW);

	for (int i = 0; i < TOP_SCORES; i++) {
		DrawText(TextFormat("%d. %d", i + 1, p->topScores[i]), x, y + 40 + i * 28, 24, WHITE);
	}
}
