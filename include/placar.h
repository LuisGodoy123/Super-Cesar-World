#ifndef PLACAR_H
#define PLACAR_H

#include "raylib.h"

#define TOP_SCORES 5

typedef struct {
	int pontuacao;
	int vidas;
	int moedas;
	int faseAtual;
	int tempo;
	int topScores[TOP_SCORES];
} Placar;

void IniciarPlacar(Placar *p);
void AtualizarPlacar(Placar *p, int pontuacao, int vidas, int moedas, int faseAtual, int tempo);
void CarregarPlacar(Placar *p);
void SalvarPlacar(Placar *p);
void RegistrarPontuacaoFinal(Placar *p, int pontuacaoFinal);
void DesenharPlacar(Placar *p, Font *fonte, int temFonte, Texture2D texMoeda, int temTexMoeda);
void DesenharTopScores(Placar *p, int x, int y);

#endif
