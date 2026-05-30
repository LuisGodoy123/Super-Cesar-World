#include "placar.h"
#include <stdio.h>

#define HUD_ALTURA   68
#define HUD_COR      ((Color){0, 0, 0, 200})
#define HUD_BORDA    ((Color){70, 70, 70, 255})

const char *ARQ_PLACAR = "data/placar.dat";

void desenhar_texto_ui(Font *fonte, int temFonte, const char *txt,
                              int x, int y, int tamanho, Color cor) {
	if (temFonte && fonte != NULL) {
		DrawTextEx(*fonte, txt, (Vector2){(float)x, (float)y}, (float)tamanho, 1, cor);
	} else {
		DrawText(txt, x, y, tamanho, cor);
	}
}

int medir_texto(Font *fonte, int temFonte, const char *txt, int tamanho) {
	if (temFonte && fonte != NULL)
		return (int)MeasureTextEx(*fonte, txt, (float)tamanho, 1).x;
	return MeasureText(txt, tamanho);
}

void limpar_top_scores(Placar *p) {
	for (int i = 0; i < TOP_SCORES; i++) p->topScores[i] = 0;
}

void IniciarPlacar(Placar *p) {
	if (p == NULL) return;

	p->pontuacao = 0;
	p->vidas     = 3;
	p->moedas    = 0;
	p->faseAtual = 1;
	p->tempo     = 300;
	limpar_top_scores(p);
}

void AtualizarPlacar(Placar *p, int pontuacao, int vidas, int moedas, int faseAtual, int tempo) {
	if (p == NULL) return;

	p->pontuacao = pontuacao;
	p->vidas     = vidas;
	p->moedas    = moedas;
	p->faseAtual = faseAtual;
	p->tempo     = tempo;
}

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

void SalvarPlacar(Placar *p) {
	if (p == NULL) return;

	FILE *f = fopen(ARQ_PLACAR, "w");
	if (f == NULL) return;

	for (int i = 0; i < TOP_SCORES; i++) fprintf(f, "%d\n", p->topScores[i]);

	fclose(f);
}

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

void DesenharPlacar(Placar *p, Font *fonte, int temFonte, Texture2D texMoeda, int temTexMoeda) {
	if (p == NULL) return;

	int sw = GetScreenWidth();

	int lx = 30;

	const char *nomeTxt  = "CESAR";
	const char *vidasTxt = TextFormat("x%d", p->vidas);

	desenhar_texto_ui(fonte, temFonte, nomeTxt,  lx, 8,  26, ORANGE);
	desenhar_texto_ui(fonte, temFonte, vidasTxt, lx, 40, 26, WHITE);

	int cx = sw / 2;

	const char *timeLbl = "TIME";
	const char *timeVal = TextFormat("%d", p->tempo);

	Color corTempo = (p->tempo <= 60) ? RED : WHITE;

	int timeLblW = medir_texto(fonte, temFonte, timeLbl, 26);
	int timeValW = medir_texto(fonte, temFonte, timeVal, 28);

	desenhar_texto_ui(fonte, temFonte, timeLbl, cx - timeLblW / 2, 8,  26, YELLOW);
	desenhar_texto_ui(fonte, temFonte, timeVal, cx - timeValW / 2, 36, 28, corTempo);

	int iconSz  = 36;
	int iconX   = sw - 230;
	int iconY   = (HUD_ALTURA - iconSz) / 2 - 4;

	if (temTexMoeda && texMoeda.id > 0) {
		Rectangle src  = { 0, 0, (float)texMoeda.width, (float)texMoeda.height };
		Rectangle dest = { (float)iconX, (float)iconY, (float)iconSz, (float)iconSz };
		DrawTexturePro(texMoeda, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
	} else {
		DrawCircle(iconX + iconSz / 2, iconY + iconSz / 2, (float)iconSz / 2, GOLD);
	}

	int txtX = iconX + iconSz + 8;

	const char *moedasTxt = TextFormat("x%d", p->moedas);
	const char *pontosTxt = TextFormat("%d", p->pontuacao);

	desenhar_texto_ui(fonte, temFonte, moedasTxt, txtX, 8,  26, WHITE);
	desenhar_texto_ui(fonte, temFonte, pontosTxt, txtX, 36, 26, WHITE);
}

void DesenharTopScores(Placar *p, int x, int y) {
	if (p == NULL) return;

	DrawText("TOP 5 SCORES", x, y, 32, YELLOW);

	for (int i = 0; i < TOP_SCORES; i++) {
		DrawText(TextFormat("%d. %d", i + 1, p->topScores[i]), x, y + 40 + i * 28, 24, WHITE);
	}
}

void DesenharBarraVidaBoss(int vida, int vidaMax) {
	if (vidaMax <= 0) return;

	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	int barW    = 400;
	int barH    = 28;
	int padding = 6;
	int outW    = barW + padding * 2;
	int outH    = barH + padding * 2;
	int labelH  = 24;
	int gap     = 6;
	int totalH  = labelH + gap + outH;

	int blockY = sh - 20 - totalH;
	int outX   = sw / 2 - outW / 2;
	int outY   = blockY + labelH + gap;

	const char *label = "BOSS";
	DrawText(label, sw / 2 - MeasureText(label, labelH) / 2, blockY, labelH, RED);

	DrawRectangle(outX - 4, outY - 4, outW + 8, outH + 8, (Color){0, 0, 0, 180});
	DrawRectangle(outX, outY, outW, outH, (Color){60, 60, 60, 255});

	float ratio = (float)vida / (float)vidaMax;
	Color cor;
	if      (ratio > 0.60f) cor = (Color){40, 200, 40, 255};
	else if (ratio > 0.30f) cor = (Color){230, 180, 0, 255};
	else                    cor = (Color){210, 30, 30, 255};

	int fillW = (int)(ratio * (float)barW);
	if (fillW > 0)
		DrawRectangle(outX + padding, outY + padding, fillW, barH, cor);

	DrawRectangleLines(outX, outY, outW, outH, WHITE);

	const char *hp = TextFormat("%d / %d", vida, vidaMax);
	DrawText(hp, outX + outW - MeasureText(hp, 18) - 6, outY + (outH - 18) / 2, 18, WHITE);
}
