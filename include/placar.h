#ifndef PLACAR_H
#define PLACAR_H

#include "raylib.h"

#define TOP_SCORES 5
#define NICK_MAX   12

typedef struct {
	int pontuacao;
	int vidas;
	int moedas;
	int faseAtual;
	int tempo;
	int  topScores[TOP_SCORES];
	char topNicks[TOP_SCORES][NICK_MAX + 1];
} Placar;

void IniciarPlacar(Placar *p);
void AtualizarPlacar(Placar *p, int pontuacao, int vidas, int moedas, int faseAtual, int tempo);
void CarregarPlacar(Placar *p);
void SalvarPlacar(Placar *p);
void RegistrarPontuacaoFinal(Placar *p, int pontuacaoFinal, const char *nick);
void DesenharPlacar(Placar *p, Font *fonte, int temFonte, Texture2D texMoeda, int temTexMoeda);
void DesenharTopScores(Placar *p, int x, int y);
void DesenharBarraVidaBoss(int vida, int vidaMax);

#endif
