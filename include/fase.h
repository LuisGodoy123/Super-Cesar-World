#ifndef FASE_H
#define FASE_H

#include "raylib.h"

#define TILE     32
#define LINHAS   23
#define COLUNAS  130

//valores do mapa
#define VAZIO      0
#define PLATAFORMA 1
#define MOEDA      2

typedef struct {
    int mapa[LINHAS][COLUNAS]; // 0=vazio  1=plataforma  2=moeda
    int numero;                // 1, 2 ou 3
    Color corFundo;
    float cameraX;             // scroll horizontal
} Fase;

void CarregarFase(Fase *f, int n);
void DesenharFase(Fase *f);
int  VerificarColisao(Rectangle a, Rectangle b);

#endif
