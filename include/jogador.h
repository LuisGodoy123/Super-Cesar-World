#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"
#include "fase.h"

#define JOGADOR_LARGURA    32
#define JOGADOR_ALTURA     48
#define GRAVIDADE         850.0f
#define MULT_QUEDA          1.7f
#define VELOCIDADE_X      300.0f
#define FORCA_PULO       -550.0f
#define TEMPO_INVENCIVEL    2.0f
#define COYOTE_TIME        0.10f
#define JUMP_BUFFER        0.12f

#define VIVO       0
#define INVENCIVEL 1
#define MORTO      2

typedef struct {
    float x, y;           // posicao
    float vx, vy;         // velocidade
    int vidas;            // inicia com 3
    int pontos;
    int estado;           // VIVO | INVENCIVEL | MORTO
    int noChao;
    float timerInvencivel;
    float timerCoyote;
    float timerJumpBuffer;
    float cameraX;        // offset da camera (seguindo o jogador)
    Texture2D sprite;
    int temSprite;
} Jogador;

void IniciarJogador(Jogador *j);
void AtualizarJogador(Jogador *j, Fase *f);
void DesenharJogador(Jogador *j);

#endif
