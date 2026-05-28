#ifndef INIMIGO_H
#define INIMIGO_H

#include "raylib.h"
#include "fase.h"
#include "jogador.h"

#define CAMINHADOR  0
#define PERSEGUIDOR 1
#define BOSS        2

#define VEL_CAMINHADOR   80.0f
#define VEL_PERSEGUIDOR 120.0f
#define VEL_BOSS         50.0f
#define INTERVALO_POUSO   3.0f

typedef struct {
    float x, y;
    float vx, vy;
    int tipo;          // CAMINHADOR | PERSEGUIDOR | BOSS
    int vida;
    int ativo;
    int largura, altura;
    float animTimer;
    int   animFrame;
    float timerPouso;
    float stuckTimer;
    float patrulhaMin;  // limite esquerdo (voador/boss)
    float patrulhaMax;  // limite direito  (voador/boss)
    float patrulhaYMin; // limite superior do boss
    float patrulhaYMax; // limite inferior do boss
    int   spriteSet;    // 1 = fase1, 2 = fase2
} Inimigo;

typedef struct No {        // no da lista de inimigos 
    Inimigo dados;
    struct No *proximo;    // ponteiro para o proximo no
} No;

No  *CriarInimigo(int tipo, float x, float y);
void AdicionarInimigo(No **lista, int tipo, float x, float y);
void AtualizarInimigos(No *lista, Jogador *j, Fase *f, float dt, Sound sndKick);
void DesenharInimigos(No *lista, float cameraX, float cameraYOffset,
                      Texture2D texIni1, Texture2D texIni2, Texture2D texIniRebaixado,
                      Texture2D texF2Ini1, Texture2D texF2Ini2, Texture2D texF2Ini3,
                      Texture2D texBoss[6]);
void LiberarInimigos(No *lista);

#endif
