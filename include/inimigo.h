#ifndef INIMIGO_H
#define INIMIGO_H

#include "raylib.h"
#include "jogador.h"

#define CAMINHADOR  0
#define PERSEGUIDOR 1
#define BOSS        2

#define VEL_CAMINHADOR   80.0f
#define VEL_PERSEGUIDOR 120.0f
#define VEL_BOSS         50.0f
#define PATROL_RANGE     96.0f  // 3 tiles de patrulha para cada lado
#define INTERVALO_TIRO    3.0f  // boss atira a cada 3 segundos

typedef struct {
    float x, y;
    float vx, vy;
    int tipo;          // CAMINHADOR | PERSEGUIDOR | BOSS
    int vida;
    int ativo;
    int largura, altura;
    float origemX;     // centro da patrulha do CAMINHADOR
    float timerTiro;   // contador de tiro do BOSS
} Inimigo;

typedef struct No {        // NO DA LISTA ENCADEADA
    Inimigo dados;
    struct No *proximo;    // ponteiro para o proximo no
} No;

No  *CriarInimigo(int tipo, float x, float y);
void AdicionarInimigo(No **lista, int tipo, float x, float y);
void AtualizarInimigos(No *lista, Jogador *j, Fase *f, float dt);
void DesenharInimigos(No *lista, float cameraX);
void LiberarInimigos(No *lista);

#endif
