#ifndef CHAVE_H
#define CHAVE_H

#include "raylib.h"
#include "jogador.h"

typedef struct {
    float x, y;
    int coletada;
} Chave;

void IniciarChave(Chave *chave, float x, float y);
void AtualizarChave(Chave *chave, Jogador *j);
void DesenharChave(Chave chave, float cameraX, float cameraYOffset);

#endif
