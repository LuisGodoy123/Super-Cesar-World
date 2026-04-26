#include "jogador.h"

// Retorna 1 se o tile na posicao (col, linha) do mapa eh solido
static int tile_solido(Fase *f, int col, int linha) {
    if (col < 0 || col >= COLUNAS || linha < 0 || linha >= LINHAS)
        return 0;
    return f->mapa[linha][col] == PLATAFORMA;
}

// IniciarJogador — inicializa posicao, vidas e pontos
void IniciarJogador(Jogador *j) {
    j->x              = 100.0f;
    j->y              = 300.0f;
    j->vx             = 0.0f;
    j->vy             = 0.0f;
    j->vidas          = 3;
    j->pontos         = 0;
    j->estado         = VIVO;
    j->noChao         = 0;
    j->timerInvencivel = 0.0f;
    j->cameraX        = 0.0f;
    j->temSprite      = 0;
}

// AtualizarJogador — input, gravidade, movimento, colisao com mapa
void AtualizarJogador(Jogador *j, Fase *f) {
    if (j->estado == MORTO) return;

    float delta = GetFrameTime();

    // --- Input horizontal ---
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        j->vx = -VELOCIDADE_X;
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        j->vx = VELOCIDADE_X;
    else
        j->vx = 0.0f;

    // --- Pulo ---
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && j->noChao) {
        j->vy = FORCA_PULO;
        j->noChao = 0;
    }

    // --- Gravidade ---
    j->vy += GRAVIDADE * delta;

    // --- Movimento e colisao no eixo X ---
    j->x += j->vx * delta;

    int colEsq = (int)(j->x) / TILE;
    int colDir = (int)(j->x + JOGADOR_LARGURA - 1) / TILE;
    int linTop = (int)(j->y) / TILE;
    int linBot = (int)(j->y + JOGADOR_ALTURA - 1) / TILE;

    if (j->vx < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colEsq, linBot))) {
        j->x  = (float)((colEsq + 1) * TILE);
        j->vx = 0.0f;
    }
    if (j->vx > 0 && (tile_solido(f, colDir, linTop) || tile_solido(f, colDir, linBot))) {
        j->x  = (float)(colDir * TILE - JOGADOR_LARGURA);
        j->vx = 0.0f;
    }

    // --- Movimento e colisao no eixo Y ---
    j->y += j->vy * delta;
    j->noChao = 0;

    colEsq = (int)(j->x) / TILE;
    colDir = (int)(j->x + JOGADOR_LARGURA - 1) / TILE;
    linTop = (int)(j->y) / TILE;
    linBot = (int)(j->y + JOGADOR_ALTURA - 1) / TILE;

    if (j->vy > 0 && (tile_solido(f, colEsq, linBot) || tile_solido(f, colDir, linBot))) {
        j->y      = (float)(linBot * TILE - JOGADOR_ALTURA);
        j->vy     = 0.0f;
        j->noChao = 1;
    }
    if (j->vy < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colDir, linTop))) {
        j->y  = (float)((linTop + 1) * TILE);
        j->vy = 0.0f;
    }

    // --- Limites da fase ---
    if (j->x < 0) j->x = 0;
    float limDir = (float)((COLUNAS - 1) * TILE - JOGADOR_LARGURA);
    if (j->x > limDir) j->x = limDir;

    // --- Camera segue o jogador ---
    float maxCam = (float)(COLUNAS * TILE - GetScreenWidth());
    f->cameraX = j->x - GetScreenWidth() / 2.0f + JOGADOR_LARGURA / 2.0f;
    if (f->cameraX < 0)       f->cameraX = 0;
    if (f->cameraX > maxCam)  f->cameraX = maxCam;
    j->cameraX = f->cameraX;

    // --- Timer de invencibilidade ---
    if (j->estado == INVENCIVEL) {
        j->timerInvencivel -= delta;
        if (j->timerInvencivel <= 0.0f) {
            j->estado         = VIVO;
            j->timerInvencivel = 0.0f;
        }
    }

    // --- Caiu fora da tela ---
    if (j->y > LINHAS * TILE) {
        j->vidas--;
        if (j->vidas <= 0)
            j->estado = MORTO;
        else
            IniciarJogador(j); // reinicia posicao
    }
}

// DesenharJogador — renderiza sprite com Raylib
void DesenharJogador(Jogador *j) {
    if (j->estado == MORTO) return;

    // Piscar quando INVENCIVEL: some nos frames impares
    if (j->estado == INVENCIVEL && (int)(GetTime() * 10) % 2 == 0) return;

    int screenX = (int)(j->x - j->cameraX);
    int screenY = (int)(j->y);

    if (j->temSprite) {
        DrawTexture(j->sprite, screenX, screenY, WHITE);
    } else {
        DrawRectangle(screenX, screenY, JOGADOR_LARGURA, JOGADOR_ALTURA, RED);
        // olhos simples para indicar direcao
        DrawRectangle(screenX + 6,  screenY + 10, 6, 6, WHITE);
        DrawRectangle(screenX + 20, screenY + 10, 6, 6, WHITE);
    }
}
