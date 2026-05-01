#include "placar.h"
#include <stdio.h>

#define HUD_ALTURA   68
#define HUD_COR      ((Color){0, 0, 0, 200})
#define HUD_BORDA    ((Color){70, 70, 70, 255})

static const char *ARQ_PLACAR = "data/placar.dat";

static void desenhar_texto_ui(Font *fonte, int temFonte, const char *txt,
                              int x, int y, int tamanho, Color cor) {
	if (temFonte && fonte != NULL) {
		DrawTextEx(*fonte, txt, (Vector2){(float)x, (float)y}, (float)tamanho, 1, cor);
	} else {
		DrawText(txt, x, y, tamanho, cor);
	}
}

static int medir_texto(Font *fonte, int temFonte, const char *txt, int tamanho) {
	if (temFonte && fonte != NULL)
		return (int)MeasureTextEx(*fonte, txt, (float)tamanho, 1).x;
	return MeasureText(txt, tamanho);
}

static void limpar_top_scores(Placar *p) {
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

// DesenharPlacar — HUD superior com vidas, timer, moedas e pontuacao

void DesenharPlacar(Placar *p, Font *fonte, int temFonte, Texture2D texMoeda, int temTexMoeda) {
	if (p == NULL) return;

	int sw = GetScreenWidth();

	// ==============================
	// ESQUERDA: "CESAR" + "x{vidas}"
	// ==============================
	int lx = 30;

	const char *nomeTxt  = "CESAR";
	const char *vidasTxt = TextFormat("x%d", p->vidas);

	desenhar_texto_ui(fonte, temFonte, nomeTxt,  lx, 8,  26, ORANGE);
	desenhar_texto_ui(fonte, temFonte, vidasTxt, lx, 40, 26, WHITE);

	// ==============================
	// CENTRO: "TIME" + valor
	// ==============================
	int cx = sw / 2;

	const char *timeLbl = "TIME";
	const char *timeVal = TextFormat("%d", p->tempo);

	Color corTempo = (p->tempo <= 60) ? RED : WHITE;

	int timeLblW = medir_texto(fonte, temFonte, timeLbl, 26);
	int timeValW = medir_texto(fonte, temFonte, timeVal, 28);

	desenhar_texto_ui(fonte, temFonte, timeLbl, cx - timeLblW / 2, 8,  26, YELLOW);
	desenhar_texto_ui(fonte, temFonte, timeVal, cx - timeValW / 2, 36, 28, corTempo);

	// ==============================
	// DIREITA: icone moeda + "x{n}" + pontuacao
	// ==============================
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

// DesenharTopScores — mostra ranking de pontuacoes

void DesenharTopScores(Placar *p, int x, int y) {
	if (p == NULL) return;

	DrawText("TOP 5 SCORES", x, y, 32, YELLOW);

	for (int i = 0; i < TOP_SCORES; i++) {
		DrawText(TextFormat("%d. %d", i + 1, p->topScores[i]), x, y + 40 + i * 28, 24, WHITE);
	}
}
