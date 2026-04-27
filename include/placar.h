#ifndef PLACAR_H
#define PLACAR_H

#include "raylib.h"

#define TOP_SCORES 5

typedef struct {
	int pontuacao;
	int vidas;
	int faseAtual;
	int topScores[TOP_SCORES]; // top 5 lidos do arquivo
} Placar;

void IniciarPlacar(Placar *p);
void AtualizarPlacar(Placar *p, int pontuacao, int vidas, int faseAtual);
void CarregarPlacar(Placar *p);
void SalvarPlacar(Placar *p);
void RegistrarPontuacaoFinal(Placar *p, int pontuacaoFinal);
void DesenharPlacar(Placar *p, Font *fonte, int temFonte);
void DesenharTopScores(Placar *p, int x, int y);

#endif
