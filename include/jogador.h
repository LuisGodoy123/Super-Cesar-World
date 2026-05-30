#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"
#include "fase.h"

#define JOGADOR_LARGURA         60
#define JOGADOR_ALTURA          42
#define JOGADOR_HITBOX_LARGURA  38
#define JOGADOR_HITBOX_OFFSET_X 11
#define PONTOS_BLOCO           200
#define FATOR_CAFE             2.0f
#define TEMPO_CAFE             5.0f

#define GRAVIDADE                   0.25f
#define VELOCIDADE_QUEDA_MAXIMA     8.0f
#define VELOCIDADE_CAMINHADA        2.5f
#define ACELERACAO                  0.15f
#define DESACELERACAO               0.12f
#define ACELERACAO_AR               0.12f
#define DESACELERACAO_AR            0.08f
#define FORCA_PULO                 -8.5f
#define COYOTE_FRAMES                6
#define JUMP_BUFFER_FRAMES           8

#define TEMPO_INVENCIVEL            2.0f

#define VIVO       0
#define INVENCIVEL 1
#define MORTO      2

typedef enum {
    MOV_PARADO = 0,
    MOV_CAMINHANDO,
    MOV_PULANDO,
    MOV_CAINDO,
    MOV_MORTO
} EstadoMovimento;

typedef struct {
    float x, y;
    float vx, vy;
    float vyAnterior;
    int vidas;
    int pontos;
    int estado;
    EstadoMovimento estadoMov;
    int noChao;
    float timerInvencivel;
    int coyoteFrames;
    int jumpBufferFrames;
    int direcao;
    float animTimer;
    int animFrame;
    float cameraX;
    int cafeAtivo;
    float timerCafe;
    int moedas;
    Texture2D sprites[3];
    int numSprites;
    int temSprites;
    int respawn;
} Jogador;

void IniciarJogador(Jogador *j);
void AtualizarJogador(Jogador *j, Fase *f, int bloqueado, Sound sndJump, Sound snd1up);
void DesenharJogador(Jogador *j, float cameraYOffset);

#endif
