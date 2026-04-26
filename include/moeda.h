#ifndef MOEDA_H
#define MOEDA_H

#include "raylib.h"
#include "fase.h"
#include "jogador.h"

#define PONTOS_MOEDA 100

typedef struct NoMoeda {
	float x, y;
	int coletada;
	struct NoMoeda *proximo;
} NoMoeda;

NoMoeda *CriarMoeda(float x, float y);
void AdicionarMoeda(NoMoeda **lista, float x, float y);
void CarregarMoedasDaFase(NoMoeda **lista, Fase *f);
void AtualizarMoedas(NoMoeda *lista, Jogador *j);
void DesenharMoedas(NoMoeda *lista, float cameraX);
void LiberarMoedas(NoMoeda *lista);

#endif
