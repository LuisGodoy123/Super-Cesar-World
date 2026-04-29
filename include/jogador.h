#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"
#include "fase.h"

#define JOGADOR_LARGURA         80
#define JOGADOR_ALTURA          55
#define JOGADOR_HITBOX_LARGURA  50
#define JOGADOR_HITBOX_OFFSET_X 15
#define PONTOS_BLOCO           200

/* constantes de fisica (por frame @60fps) */
#define GRAVIDADE                   0.25f
#define VELOCIDADE_QUEDA_MAXIMA     8.0f
#define VELOCIDADE_CAMINHADA        2.5f
#define VELOCIDADE_CORRIDA          3.8f
#define ACELERACAO                  0.15f
#define DESACELERACAO               0.12f
#define ACELERACAO_AR               0.12f
#define DESACELERACAO_AR            0.08f
#define FORCA_PULO                 -6.0f
#define FORCA_SEGURAR_PULO         -0.20f
#define TEMPO_MAXIMO_SEGURAR_PULO   12
#define CORTE_PULO_CURTO             6
#define FORCA_PULO_CORRENDO        -6.8f
#define FORCA_SEGURAR_PULO_CORRENDO -0.25f
#define DESACELERACAO_DERRAPAGEM    0.6f
#define LIMITE_DERRAPAGEM           2.0f
#define COYOTE_FRAMES                6
#define JUMP_BUFFER_FRAMES           8

/* temporizadores em segundos (nao dependem da fisica) */
#define TEMPO_INVENCIVEL            2.0f

#define VIVO       0
#define INVENCIVEL 1
#define MORTO      2

typedef enum {
    MOV_PARADO = 0,
    MOV_CAMINHANDO,
    MOV_CORRENDO,
    MOV_DERRAPANDO,
    MOV_PULANDO,
    MOV_CAINDO,
    MOV_AGACHADO,
    MOV_MORTO
} EstadoMovimento;

typedef struct {
    float x, y;           // posicao
    float vx, vy;         // velocidade
    int vidas;            // inicia com 3
    int pontos;
    int estado;           // VIVO | INVENCIVEL | MORTO
    EstadoMovimento estadoMov;
    int noChao;
    float timerInvencivel;
    int coyoteFrames;
    int jumpBufferFrames;
    int jumpHoldFrames;
    int jumpFrames;
    int jumpCutFeito;
    int puloCorrendo;
    int agachado;
    int emDerrapagem;
    int direcao;           // -1 esquerda, 1 direita
    int alturaAtual;
    float limiteVelocidadeAr;
    float animTimer;
    int animFrame;
    float cameraX;        // offset da camera (seguindo o jogador)
    Texture2D sprites[3];
    int numSprites;
    int temSprites;
} Jogador;

void IniciarJogador(Jogador *j);
void AtualizarJogador(Jogador *j, Fase *f);
void DesenharJogador(Jogador *j);

#endif
