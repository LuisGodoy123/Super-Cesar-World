#ifndef FASE_H
#define FASE_H

#include "raylib.h"
#include "bloco.h"

#define TILE     32
#define LINHAS   23
#define COLUNAS  130
#define CAMERA_ZOOM 1.35f
#define CAMERA_Y_OFFSET 128.0f

#define VAZIO  0
#define BLOCO  1
#define MOEDA  2
#define PORTA  3

typedef struct {
    int   mapa[LINHAS][COLUNAS];
    Bloco blocos[LINHAS][COLUNAS];
    int numero;
    Color corFundo;
    float cameraX;
    float cameraYOffset;
} Fase;

int  tile_solido(Fase *f, int col, int linha);
void CarregarFase(Fase *f, int n);
void DesenharFase(Fase *f, Texture2D texBloco, Texture2D texTijoloCinza, Texture2D texTerra,
                  Texture2D texCafe, Texture2D texFundo2);
int  VerificarColisao(Rectangle a, Rectangle b);

#endif
