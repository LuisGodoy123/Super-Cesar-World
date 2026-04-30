#include "jogador.h"
#include <math.h>

// Retorna 1 se o tile na posicao (col, linha) do mapa eh solido
static int tile_solido(Fase *f, int col, int linha) {
    if (col < 0 || col >= COLUNAS || linha < 0 || linha >= LINHAS)
        return 0;
    return f->mapa[linha][col] == BLOCO;
}

static int sinal(float v) {
    if (v > 0.0f) return 1;
    if (v < 0.0f) return -1;
    return 0;
}

static float aproximar(float v, float alvo, float delta) {
    if (v < alvo) return fminf(v + delta, alvo);
    if (v > alvo) return fmaxf(v - delta, alvo);
    return v;
}

static int pode_ficar_em_pe(Jogador *j, Fase *f) {
    int alturaCheia = JOGADOR_ALTURA;
    float novoY = j->y - (float)(alturaCheia - j->alturaAtual);

    int colEsq = (int)(j->x + JOGADOR_HITBOX_OFFSET_X) / TILE;
    int colDir = (int)(j->x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA - 1) / TILE;
    int linTop = (int)(novoY) / TILE;
    int linBot = (int)(novoY + alturaCheia - 1) / TILE;

    if (tile_solido(f, colEsq, linTop) || tile_solido(f, colDir, linTop)) return 0;
    if (tile_solido(f, colEsq, linBot) || tile_solido(f, colDir, linBot)) return 0;
    return 1;
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
    j->vidas          = 3;
    j->pontos         = 0;
    j->estado         = VIVO;
    j->estadoMov       = MOV_PARADO;
    j->noChao          = 0;
    j->timerInvencivel = 0.0f;
    j->coyoteFrames    = 0;
    j->jumpBufferFrames = 0;
    j->jumpHoldFrames  = 0;
    j->jumpFrames      = -1;
    j->jumpCutFeito    = 0;
    j->puloCorrendo    = 0;
    j->agachado        = 0;
    j->emDerrapagem    = 0;
    j->direcao         = 1;
    j->alturaAtual     = JOGADOR_ALTURA;
    j->limiteVelocidadeAr = VELOCIDADE_CAMINHADA;
    j->animTimer       = 0.0f;
    j->animFrame       = 0;
    j->cameraX         = 0.0f;
    j->devMode         = 0;
    j->cafeAtivo       = 0;
    j->timerCafe       = 0.0f;
    j->temSprites      = temSprites;
    j->numSprites      = numSprites;

    for (int i = 0; i < 3; i++) j->sprites[i] = sprites[i];
}

// AtualizarJogador — input, gravidade, movimento, colisao com mapa
void AtualizarJogador(Jogador *j, Fase *f) {
    if (j->estado == MORTO) {
        j->estadoMov = MOV_MORTO;
        return;
    }

    const float dt = 1.0f / 60.0f;

    if (IsKeyPressed(KEY_F1)) j->devMode = !j->devMode;

    float fator = (j->devMode ? 3.0f : 1.0f) * (j->cafeAtivo ? FATOR_CAFE : 1.0f);
    float vel_caminhada = VELOCIDADE_CAMINHADA * fator;
    float vel_corrida   = VELOCIDADE_CORRIDA   * fator;

    int left  = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    int right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    int down  = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    int run   = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

    int jumpPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    int jumpHeld    = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);

    int dirInput = (right ? 1 : 0) - (left ? 1 : 0);

    /* timers por frame */
    if (j->coyoteFrames > 0) j->coyoteFrames--;
    if (j->jumpBufferFrames > 0) j->jumpBufferFrames--;
    if (jumpPressed) j->jumpBufferFrames = JUMP_BUFFER_FRAMES;

    /* limite de velocidade no ar depende do estado no chao */
    if (j->noChao) {
        j->limiteVelocidadeAr = run ? vel_corrida : vel_caminhada;
    }

    /* atualiza direcao pelo input (fora da derrapagem) */
    if (!j->emDerrapagem && dirInput != 0) {
        j->direcao = dirInput;
    }

    int alturaCheia = JOGADOR_ALTURA;
    int alturaBaixa = JOGADOR_ALTURA / 2;

    /* agachamento */
    if (down && j->noChao && !j->agachado) {
        if (fabsf(j->vx) >= LIMITE_DERRAPAGEM) {
            j->emDerrapagem = 1;
        } else {
            j->agachado = 1;
            j->alturaAtual = alturaBaixa;
            j->y += (float)(alturaCheia - alturaBaixa);
        }
    } else if (!down && j->agachado) {
        if (pode_ficar_em_pe(j, f)) {
            j->agachado = 0;
            j->alturaAtual = alturaCheia;
            j->y -= (float)(alturaCheia - alturaBaixa);
        }
    }

    j->alturaAtual = j->agachado ? alturaBaixa : alturaCheia;

    /* pulo (buffer + coyote) */
    if (!j->agachado && j->jumpBufferFrames > 0 && (j->noChao || j->coyoteFrames > 0)) {
        float limiarCorrida = vel_corrida * 0.8f;
        if (fabsf(j->vx) >= limiarCorrida) {
            j->vy = FORCA_PULO_CORRENDO;
            j->puloCorrendo = 1;
        } else {
            j->vy = FORCA_PULO;
            j->puloCorrendo = 0;
        }
        j->noChao = 0;
        j->coyoteFrames = 0;
        j->jumpBufferFrames = 0;
        j->jumpHoldFrames = 0;
        j->jumpFrames = 0;
        j->jumpCutFeito = 0;
        j->emDerrapagem = 0;
    }

    /* movimento horizontal */
    if (j->noChao) {
        if (j->agachado) {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO);
        } else if (j->emDerrapagem) {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO_DERRAPAGEM);

            if (j->vx == 0.0f) {
                j->emDerrapagem = 0;
                if (down) {
                    j->agachado = 1;
                    j->alturaAtual = alturaBaixa;
                    j->y += (float)(alturaCheia - alturaBaixa);
                } else if (dirInput != 0) {
                    float limite = run ? vel_corrida : vel_caminhada;
                    j->vx += (float)dirInput * ACELERACAO;
                    j->vx = fminf(fmaxf(j->vx, -limite), limite);
                }
            }
        } else if (dirInput != 0) {
            if (sinal(j->vx) != 0 && dirInput != sinal(j->vx) && fabsf(j->vx) > LIMITE_DERRAPAGEM) {
                j->emDerrapagem = 1;
            } else {
                float limite = run ? vel_corrida : vel_caminhada;
                j->vx += (float)dirInput * ACELERACAO;
                j->vx = fminf(fmaxf(j->vx, -limite), limite);
            }
        } else {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO);
        }
    } else {
        float limite = j->limiteVelocidadeAr;
        if (dirInput != 0) {
            j->vx += (float)dirInput * ACELERACAO_AR;
            j->vx = fminf(fmaxf(j->vx, -limite), limite);
        } else {
            j->vx = aproximar(j->vx, 0.0f, DESACELERACAO_AR);
        }
    }

    if (j->emDerrapagem && j->vx != 0.0f) j->direcao = sinal(j->vx);

    /* movimento e colisao no eixo X */
    j->x += j->vx;

    int colEsq = (int)(j->x + JOGADOR_HITBOX_OFFSET_X) / TILE;
    int colDir = (int)(j->x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA - 1) / TILE;
    int linTop = (int)(j->y) / TILE;
    int linBot = (int)(j->y + j->alturaAtual - 1) / TILE;

    if (j->vx < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colEsq, linBot))) {
        j->x  = (float)((colEsq + 1) * TILE) - JOGADOR_HITBOX_OFFSET_X;
        j->vx = 0.0f;
        j->emDerrapagem = 0;
    }
    if (j->vx > 0 && (tile_solido(f, colDir, linTop) || tile_solido(f, colDir, linBot))) {
        j->x  = (float)(colDir * TILE - JOGADOR_HITBOX_LARGURA) - JOGADOR_HITBOX_OFFSET_X;
        j->vx = 0.0f;
        j->emDerrapagem = 0;
    }

    /* corte de pulo curto */
    if (j->jumpFrames >= 0 && !jumpHeld && !j->jumpCutFeito &&
        j->jumpFrames < CORTE_PULO_CURTO && j->vy < 0.0f) {
        j->vy *= 0.4f;
        j->jumpCutFeito = 1;
    }

    /* gravidade variavel */
    float grav = GRAVIDADE;
    if (j->vy < 0.0f) grav *= (jumpHeld ? 0.7f : 1.1f);
    else              grav *= 1.3f;
    j->vy += grav;

    /* pulo variavel (hold) */
    if (j->jumpFrames >= 0 && jumpHeld && j->vy < 0.0f &&
        j->jumpHoldFrames < TEMPO_MAXIMO_SEGURAR_PULO) {
        j->vy += j->puloCorrendo ? FORCA_SEGURAR_PULO_CORRENDO : FORCA_SEGURAR_PULO;
        j->jumpHoldFrames++;
    }

    if (j->vy > VELOCIDADE_QUEDA_MAXIMA) j->vy = VELOCIDADE_QUEDA_MAXIMA;

    /* movimento e colisao no eixo Y */
    int noChaoAnterior = j->noChao;
    j->y += j->vy;
    j->noChao = 0;

    colEsq = (int)(j->x + JOGADOR_HITBOX_OFFSET_X) / TILE;
    colDir = (int)(j->x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA - 1) / TILE;
    linTop = (int)(j->y) / TILE;
    linBot = (int)(j->y + j->alturaAtual - 1) / TILE;

    if (j->vy > 0 && (tile_solido(f, colEsq, linBot) || tile_solido(f, colDir, linBot))) {
        j->y      = (float)(linBot * TILE - j->alturaAtual);
        j->vy     = 0.0f;
        j->noChao = 1;
    }

    /* verificacao secundaria: gravidade pequena pode nao penetrar o tile,
       mas o chao ainda esta diretamente abaixo */
    if (!j->noChao && j->vy >= 0.0f) {
        int linBotRest = (int)(j->y + j->alturaAtual) / TILE;
        if (tile_solido(f, colEsq, linBotRest) || tile_solido(f, colDir, linBotRest)) {
            j->noChao = 1;
            j->vy     = 0.0f;
        }
    }

    if (j->vy < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colDir, linTop))) {
        for (int c = colEsq; c <= colDir; c++) {
            if (c >= 0 && c < COLUNAS &&
                f->blocos[linTop][c].tipo  == BLOCO_TIPO_POWERUP &&
                f->blocos[linTop][c].estado == BLOCO_ESTADO_ATIVO) {
                f->blocos[linTop][c].estado = BLOCO_ESTADO_USADO;
                j->pontos += PONTOS_BLOCO;
            }
        }
        j->y  = (float)((linTop + 1) * TILE);
        j->vy = 0.0f;
    }

    if (noChaoAnterior && !j->noChao && j->vy > 0.0f) {
        j->coyoteFrames = COYOTE_FRAMES;
        j->emDerrapagem = 0;
        if (j->agachado) {
            j->agachado = 0;
            j->alturaAtual = alturaCheia;
            j->y -= (float)(alturaCheia - alturaBaixa);
        }
    }

    if (j->noChao) {
        j->jumpFrames = -1;
        j->jumpHoldFrames = 0;
        j->jumpCutFeito = 0;
        j->puloCorrendo = 0;
    } else if (j->jumpFrames >= 0) {
        j->jumpFrames++;
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
    } else if (j->agachado) {
        j->estadoMov = MOV_AGACHADO;
    } else if (!j->noChao) {
        j->estadoMov = (j->vy < 0.0f) ? MOV_PULANDO : MOV_CAINDO;
    } else if (j->emDerrapagem) {
        j->estadoMov = MOV_DERRAPANDO;
    } else if (fabsf(j->vx) < 0.01f) {
        j->estadoMov = MOV_PARADO;
    } else if (fabsf(j->vx) > VELOCIDADE_CAMINHADA + 0.01f || run) {
        j->estadoMov = MOV_CORRENDO;
    } else {
        j->estadoMov = MOV_CAMINHANDO;
    }

    if (j->estadoMov == MOV_CAMINHANDO || j->estadoMov == MOV_CORRENDO) {
        float speed = fabsf(j->vx);
        float ratio = (VELOCIDADE_CORRIDA > 0.0f) ? (speed / VELOCIDADE_CORRIDA) : 0.0f;
        float step = 0.10f + 0.15f * ratio;
        int frames = (j->numSprites > 0) ? j->numSprites : 1;

        j->animTimer += step;
        if (j->animTimer >= (float)frames) j->animTimer -= (float)frames;
        j->animFrame = (int)j->animTimer;
    } else {
        j->animTimer = 0.0f;
        if (j->estadoMov == MOV_DERRAPANDO)      j->animFrame = 2;
        else if (j->estadoMov == MOV_PULANDO)    j->animFrame = 0;
        else if (j->estadoMov == MOV_CAINDO)     j->animFrame = 1;
        else if (j->estadoMov == MOV_AGACHADO)   j->animFrame = 3;
        else                                      j->animFrame = 0;
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
            IniciarJogador(j); // reinicia posicao
    }
}

// DesenharJogador — renderiza sprite com Raylib
void DesenharJogador(Jogador *j, float cameraYOffset) {
    if (j->estado == MORTO) return;

    // Piscar quando INVENCIVEL: some nos frames impares
    if (j->estado == INVENCIVEL && (int)(GetTime() * 10) % 2 == 0) return;

    float zoom = CAMERA_ZOOM;
    float screenX = (j->x - j->cameraX) * zoom;
    float screenY = (j->y - cameraYOffset) * zoom;
    float largura = (float)JOGADOR_LARGURA * zoom;
    float altura  = (float)j->alturaAtual * zoom;

    if (j->temSprites && j->numSprites > 0) {
        int frame = 0;
        if (j->estadoMov == MOV_CAMINHANDO || j->estadoMov == MOV_CORRENDO) {
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
        Color corpo = (j->estadoMov == MOV_CORRENDO) ? ORANGE : RED;
        if (j->estadoMov == MOV_AGACHADO) corpo = MAROON;
        if (j->estadoMov == MOV_DERRAPANDO) corpo = DARKGRAY;

        DrawRectangle((int)screenX, (int)screenY, (int)largura, (int)altura, corpo);

        /* olhos simples para indicar direcao */
        int olhoY = (int)screenY + (int)(altura / 4.0f);
        int olhoX1 = (j->direcao >= 0) ? ((int)screenX + (int)largura - 12) : ((int)screenX + 6);
        int olhoX2 = (j->direcao >= 0) ? ((int)screenX + (int)largura - 6)  : ((int)screenX + 12);
        DrawRectangle(olhoX1, olhoY, 4, 4, WHITE);
        DrawRectangle(olhoX2, olhoY, 4, 4, WHITE);

        /* pezinhos simples para animacao */
        if (j->estadoMov == MOV_CAMINHANDO || j->estadoMov == MOV_CORRENDO) {
            int footY = (int)screenY + (int)altura - 5;
            int off = (j->animFrame % 2 == 0) ? 3 : 7;
            DrawRectangle((int)screenX + off, footY, 6, 4, DARKGRAY);
            DrawRectangle((int)screenX + (int)largura - off - 6, footY, 6, 4, DARKGRAY);
        }
    }
}
