#include "jogador.h"
#include <math.h>

// Retorna 1 se qualquer tile entre colEsq e colDir (inclusive) na linha eh solido
int faixa_solida(Fase *f, int colEsq, int colDir, int linha) {
    for (int c = colEsq; c <= colDir; c++)
        if (tile_solido(f, c, linha)) return 1;
    return 0;
}

float aproximar(float v, float alvo, float delta) {
    if (v < alvo) return fminf(v + delta, alvo);
    if (v > alvo) return fmaxf(v - delta, alvo);
    return v;
}


// IniciarJogador — inicializa posicao, vidas e pontos
void IniciarJogador(Jogador *j) {
    Texture2D sprites[3];
    int numSprites = j->numSprites;
    int temSprites = j->temSprites;

    for (int i = 0; i < 3; i++) sprites[i] = j->sprites[i];

    j->x              = 100.0f;
    j->y              = 300.0f;
    j->vx             = 0.0f;
    j->vy             = 0.0f;
    j->vidas          = 5;
    j->pontos         = 0;
    j->estado         = VIVO;
    j->estadoMov       = MOV_PARADO;
    j->noChao          = 0;
    j->timerInvencivel = 0.0f;
    j->coyoteFrames    = 0;
    j->jumpBufferFrames = 0;
    j->direcao         = 1;
    j->animTimer       = 0.0f;
    j->vyAnterior      = 0.0f;
    j->animFrame       = 0;
    j->cameraX         = 0.0f;
    j->devMode         = 0;
    j->cafeAtivo       = 0;
    j->timerCafe       = 0.0f;
    j->moedas          = 0;
    j->respawn         = 0;
    j->temSprites      = temSprites;
    j->numSprites      = numSprites;

    for (int i = 0; i < 3; i++) j->sprites[i] = sprites[i];
}

// AtualizarJogador — input, gravidade, movimento, colisao com mapa
void AtualizarJogador(Jogador *j, Fase *f, int bloqueado, Sound sndJump, Sound snd1up) {
    if (j->estado == MORTO) {
        j->estadoMov = MOV_MORTO;
        return;
    }

    const float dt = 1.0f / 60.0f;

    if (!bloqueado && IsKeyPressed(KEY_F1)) j->devMode = !j->devMode;

    float fator = (j->devMode ? 3.0f : 1.0f) * (j->cafeAtivo ? FATOR_CAFE : 1.0f);
    float vel_caminhada = VELOCIDADE_CAMINHADA * fator;

    int left  = !bloqueado && (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    int right = !bloqueado && (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D));

    int jumpPressed = !bloqueado && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W));

    int dirInput = (right ? 1 : 0) - (left ? 1 : 0);

    /* timers por frame */
    if (j->coyoteFrames > 0) j->coyoteFrames--;
    if (j->jumpBufferFrames > 0) j->jumpBufferFrames--;
    if (jumpPressed) j->jumpBufferFrames = JUMP_BUFFER_FRAMES;

    if (dirInput != 0) {
        j->direcao = dirInput;
    }

    /* pulo (buffer + coyote) */
    if (j->jumpBufferFrames > 0 && (j->noChao || j->coyoteFrames > 0)) {
        j->vy = FORCA_PULO;
        j->noChao = 0;
        j->coyoteFrames = 0;
        j->jumpBufferFrames = 0;
        PlaySound(sndJump);
    }

    /* movimento horizontal */
    if (j->noChao) {
        if (dirInput != 0) {
            j->vx += (float)dirInput * ACELERACAO;
            j->vx = fminf(fmaxf(j->vx, -vel_caminhada), vel_caminhada);
        } else {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO);
        }
    } else {
        if (dirInput != 0) {
            j->vx += (float)dirInput * ACELERACAO_AR;
            j->vx = fminf(fmaxf(j->vx, -vel_caminhada), vel_caminhada);
        } else {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO_AR);
        }
    }

    /* movimento e colisao no eixo X */
    j->x += j->vx;

    int colEsq = (int)(j->x + JOGADOR_HITBOX_OFFSET_X) / TILE;
    int colDir = (int)(j->x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA - 1) / TILE;
    int linTop = (int)(j->y) / TILE;
    int linBot = (int)(j->y + JOGADOR_ALTURA - 1) / TILE;

    if (j->vx < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colEsq, linBot))) {
        j->x  = (float)((colEsq + 1) * TILE) - JOGADOR_HITBOX_OFFSET_X;
        j->vx = 0.0f;
    }
    if (j->vx > 0 && (tile_solido(f, colDir, linTop) || tile_solido(f, colDir, linBot))) {
        j->x  = (float)(colDir * TILE - JOGADOR_HITBOX_LARGURA) - JOGADOR_HITBOX_OFFSET_X;
        j->vx = 0.0f;
    }

    /* gravidade */
    float grav = GRAVIDADE;
    if (j->vy >= 0.0f) grav *= 2.5f;
    j->vy += grav;

    if (j->vy > VELOCIDADE_QUEDA_MAXIMA) j->vy = VELOCIDADE_QUEDA_MAXIMA;

    /* movimento e colisao no eixo Y */
    int noChaoAnterior = j->noChao;
    j->vyAnterior = j->vy;   /* salva antes do chao zerar vy */
    j->y += j->vy;
    j->noChao = 0;

    colEsq = (int)(j->x + JOGADOR_HITBOX_OFFSET_X) / TILE;
    colDir = (int)(j->x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA - 1) / TILE;
    linTop = (int)(j->y) / TILE;
    linBot = (int)(j->y + JOGADOR_ALTURA - 1) / TILE;

    if (j->vy > 0 && faixa_solida(f, colEsq, colDir, linBot)) {
        j->y      = (float)(linBot * TILE - JOGADOR_ALTURA);
        j->vy     = 0.0f;
        j->noChao = 1;
    }

    /* verificacao secundaria: gravidade pequena pode nao penetrar o tile,
       mas o chao ainda esta diretamente abaixo */
    if (!j->noChao && j->vy >= 0.0f) {
        int linBotRest = (int)(j->y + JOGADOR_ALTURA) / TILE;
        if (faixa_solida(f, colEsq, colDir, linBotRest)) {
            j->noChao = 1;
            j->vy     = 0.0f;
        }
    }

    if (j->vy < 0 && faixa_solida(f, colEsq, colDir, linTop)) {
        for (int c = colEsq; c <= colDir; c++) {
            if (c >= 0 && c < COLUNAS &&
                f->blocos[linTop][c].tipo  == BLOCO_TIPO_POWERUP &&
                f->blocos[linTop][c].estado == BLOCO_ESTADO_ATIVO) {
                f->blocos[linTop][c].estado = BLOCO_ESTADO_USADO;
                j->pontos += PONTOS_BLOCO;
                PlaySound(snd1up);
            }
        }
        j->y  = (float)((linTop + 1) * TILE);
        j->vy = 0.0f;
    }

    if (noChaoAnterior && !j->noChao && j->vy > 0.0f) {
        j->coyoteFrames = COYOTE_FRAMES;
    }

    /* limites da fase */
    if (j->x + JOGADOR_HITBOX_OFFSET_X < 0) j->x = -(float)JOGADOR_HITBOX_OFFSET_X;
    float limDir = (float)((COLUNAS - 1) * TILE - JOGADOR_HITBOX_LARGURA) - JOGADOR_HITBOX_OFFSET_X;
    if (j->x > limDir) j->x = limDir;

    /* camera segue o jogador */
    float viewWidth = GetScreenWidth() / CAMERA_ZOOM;
    float maxCam = (float)(COLUNAS * TILE - viewWidth);
    f->cameraX = j->x - viewWidth / 2.0f + JOGADOR_LARGURA / 2.0f;
    if (f->cameraX < 0)       f->cameraX = 0;
    if (f->cameraX > maxCam)  f->cameraX = maxCam;
    j->cameraX = f->cameraX;

    /* estado de movimento e animacao */
    if (j->estado == MORTO) {
        j->estadoMov = MOV_MORTO;
    } else if (!j->noChao) {
        j->estadoMov = (j->vy < 0.0f) ? MOV_PULANDO : MOV_CAINDO;
    } else if (fabsf(j->vx) < 0.01f) {
        j->estadoMov = MOV_PARADO;
    } else {
        j->estadoMov = MOV_CAMINHANDO;
    }

    if (j->estadoMov == MOV_CAMINHANDO) {
        float speed = fabsf(j->vx);
        float ratio = (VELOCIDADE_CAMINHADA > 0.0f) ? (speed / VELOCIDADE_CAMINHADA) : 0.0f;
        float step = 0.10f + 0.15f * ratio;
        int frames = (j->numSprites > 0) ? j->numSprites : 1;

        j->animTimer += step;
        if (j->animTimer >= (float)frames) j->animTimer -= (float)frames;
        j->animFrame = (int)j->animTimer;
    } else {
        j->animTimer = 0.0f;
        if (j->estadoMov == MOV_PULANDO)     j->animFrame = 0;
        else if (j->estadoMov == MOV_CAINDO) j->animFrame = 1;
        else                                  j->animFrame = 0;
    }

    /* timer de invencibilidade */
    if (j->estado == INVENCIVEL) {
        j->timerInvencivel -= dt;
        if (j->timerInvencivel <= 0.0f) {
            j->estado         = VIVO;
            j->timerInvencivel = 0.0f;
        }
    }

    /* timer do cafe */
    if (j->cafeAtivo) {
        j->timerCafe -= dt;
        if (j->timerCafe <= 0.0f) {
            j->cafeAtivo = 0;
            j->timerCafe = 0.0f;
        }
    }

    /* caiu fora da tela */
    if (j->y > LINHAS * TILE) {
        j->vidas--;
        if (j->vidas <= 0)
            j->estado = MORTO;
        else
            j->respawn = 1;
    }
}

// DesenharJogador — renderiza sprite com Raylib
void DesenharJogador(Jogador *j, float cameraYOffset) {
    if (j->estado == MORTO) return;

    // Piscar quando INVENCIVEL: some nos frames impares
    if (j->estado == INVENCIVEL && j->timerInvencivel > 0.5f && (int)(GetTime() * 10) % 2 == 0) return;

    float zoom = CAMERA_ZOOM;
    float screenX = (j->x - j->cameraX) * zoom;
    float screenY = (j->y - cameraYOffset) * zoom;
    float largura = (float)JOGADOR_LARGURA * zoom;
    float altura  = (float)JOGADOR_ALTURA * zoom;

    if (j->temSprites && j->numSprites > 0) {
        int frame = 0;
        if (j->estadoMov == MOV_CAMINHANDO) {
            frame = j->animFrame % (j->numSprites > 2 ? 2 : j->numSprites);
        } else if ((j->estadoMov == MOV_PULANDO || j->estadoMov == MOV_CAINDO) && j->numSprites > 2) {
            frame = 2;
        } else {
            frame = 0;
        }

        Texture2D tex = j->sprites[frame];
        Rectangle src  = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dest = { screenX + largura / 2.0f,
                   screenY + altura,
                   largura, altura };
        Vector2 origin = { largura / 2.0f, altura };

        if (j->direcao > 0) {
            src.x += src.width;
            src.width *= -1.0f;
        }
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    } else {
        Color corpo = RED;

        DrawRectangle((int)screenX, (int)screenY, (int)largura, (int)altura, corpo);

        /* olhos simples para indicar direcao */
        int olhoY = (int)screenY + (int)(altura / 4.0f);
        int olhoX1 = (j->direcao >= 0) ? ((int)screenX + (int)largura - 12) : ((int)screenX + 6);
        int olhoX2 = (j->direcao >= 0) ? ((int)screenX + (int)largura - 6)  : ((int)screenX + 12);
        DrawRectangle(olhoX1, olhoY, 4, 4, WHITE);
        DrawRectangle(olhoX2, olhoY, 4, 4, WHITE);

        /* pezinhos simples para animacao */
        if (j->estadoMov == MOV_CAMINHANDO) {
            int footY = (int)screenY + (int)altura - 5;
            int off = (j->animFrame % 2 == 0) ? 3 : 7;
            DrawRectangle((int)screenX + off, footY, 6, 4, DARKGRAY);
            DrawRectangle((int)screenX + (int)largura - off - 6, footY, 6, 4, DARKGRAY);
        }
    }
}
