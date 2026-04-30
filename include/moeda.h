#ifndef MOEDA_H
#define MOEDA_H

#include "raylib.h"
#include "fase.h"
#include "jogador.h"

#define PONTOS_MOEDA 100

typedef struct NoMoeda {        // NO DA LISTA ENCADEADA
    float x, y;
    int coletada;
    struct NoMoeda *proximo;    // ponteiro para o proximo no
} NoMoeda;

NoMoeda *CriarMoeda(float x, float y);
void AdicionarMoeda(NoMoeda **lista, float x, float y);
void CarregarMoedasDaFase(NoMoeda **lista, Fase *f);
void AtualizarMoedas(NoMoeda *lista, Jogador *j);
void DesenharMoedas(NoMoeda *lista, float cameraX, float cameraYOffset, Texture2D *texMoedas, int numFrames, float tempoAnim);
void LiberarMoedas(NoMoeda *lista);

#endif
