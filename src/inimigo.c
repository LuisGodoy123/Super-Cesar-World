#include "inimigo.h"
#include "fase.h"
#include <stdlib.h>

/* pontos concedidos ao eliminar cada tipo */
int pontos_por_tipo(int tipo) {
    if (tipo == CAMINHADOR)  return 200;
    if (tipo == PERSEGUIDOR) return 300;
    if (tipo == BOSS)        return 1000;
    return 0;
}

/* dimensoes de cada tipo */
void definir_dimensoes(Inimigo *ini) {
    if      (ini->tipo == BOSS)       { ini->largura = 96; ini->altura = 96; }
    else if (ini->tipo == CAMINHADOR) { ini->largura = 32; ini->altura = 32; }
    else if (ini->tipo == PERSEGUIDOR) { ini->largura = 48; ini->altura = 48; }
    else                              { ini->largura = 32; ini->altura = 32; }
}

/* ------------------------------------------------------------------ */
/* CriarInimigo — malloc de novo no (ALOCACAO DINAMICA)                */
/* ------------------------------------------------------------------ */

No *CriarInimigo(int tipo, float x, float y) {
    No *novo = (No *) malloc(sizeof(No));   /* ALOCACAO DINAMICA */

    novo->dados.x       = x;
    novo->dados.y       = y;
    novo->dados.vx      = (tipo == BOSS) ? VEL_BOSS : (tipo == PERSEGUIDOR) ? VEL_PERSEGUIDOR : VEL_CAMINHADOR;
    novo->dados.vy      = 0.0f;
    novo->dados.tipo    = tipo;
    novo->dados.vida    = (tipo == BOSS) ? 8 : (tipo == CAMINHADOR) ? 2 : 1;
    novo->dados.ativo   = 1;
    novo->dados.animTimer  = 0.0f;
    novo->dados.animFrame  = 0;
    novo->dados.timerPouso = INTERVALO_POUSO;
    novo->dados.stuckTimer = 0.0f;
    novo->dados.patrulhaMin  = 0.0f;
    novo->dados.patrulhaMax  = 0.0f;
    novo->dados.patrulhaYMin = 0.0f;
    novo->dados.patrulhaYMax = 0.0f;
    novo->dados.spriteSet    = 1;
    definir_dimensoes(&novo->dados);

    novo->proximo = NULL;
    return novo;
}

/* ------------------------------------------------------------------ */
/* AdicionarInimigo — insere no inicio da lista                        */
/* ------------------------------------------------------------------ */

void AdicionarInimigo(No **lista, int tipo, float x, float y) {
    No *novo  = CriarInimigo(tipo, x, y);
    novo->proximo = *lista;
    *lista = novo;
}

/* ------------------------------------------------------------------ */
/* Comportamentos internos                                             */
/* ------------------------------------------------------------------ */

void aplicar_fisica_inimigo(Inimigo *ini, Fase *f, float dt) {
    if (ini->vx == 0.0f && ini->tipo != PERSEGUIDOR)
        ini->vx = (ini->tipo == BOSS) ? VEL_BOSS : VEL_CAMINHADOR;

    ini->x += ini->vx * dt;

    int colEsq = (int)(ini->x) / TILE;
    int colDir = (int)(ini->x + ini->largura - 1) / TILE;
    int linTop = (int)(ini->y) / TILE;
    int linBot = (int)(ini->y + ini->altura - 1) / TILE;

    if (ini->vx < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colEsq, linBot))) {
        ini->x = (float)((colEsq + 1) * TILE);
        if (ini->tipo != PERSEGUIDOR && ini->vy >= 0.0f && linTop > 0 && !tile_solido(f, colEsq, linTop - 1)) {
            ini->vy = -290.0f;
            ini->stuckTimer = 0.0f;
        } else if (ini->tipo != PERSEGUIDOR) {
            ini->vx = -ini->vx;
        }
    }
    if (ini->vx > 0 && (tile_solido(f, colDir, linTop) || tile_solido(f, colDir, linBot))) {
        ini->x = (float)(colDir * TILE - ini->largura);
        if (ini->tipo != PERSEGUIDOR && ini->vy >= 0.0f && linTop > 0 && !tile_solido(f, colDir, linTop - 1)) {
            ini->vy = -290.0f;
            ini->stuckTimer = 0.0f;
        } else if (ini->tipo != PERSEGUIDOR) {
            ini->vx = -ini->vx;
        }
    }

    /* stuck timer: so o caminhador pula para se desgrudar */
    if (ini->tipo != PERSEGUIDOR) {
        if (ini->vy >= 0.0f) {
            ini->stuckTimer += dt;
            if (ini->stuckTimer >= 1.2f) {
                ini->vy = -290.0f;
                ini->stuckTimer = 0.0f;
            }
        } else {
            ini->stuckTimer = 0.0f;
        }
    }

    float grav = GRAVIDADE * 60.0f * 60.0f;
    ini->vy += grav * dt;
    ini->y += ini->vy * dt;

    colEsq = (int)(ini->x) / TILE;
    colDir = (int)(ini->x + ini->largura - 1) / TILE;
    linTop = (int)(ini->y) / TILE;
    linBot = (int)(ini->y + ini->altura - 1) / TILE;

    if (ini->vy > 0.0f && (tile_solido(f, colEsq, linBot) || tile_solido(f, colDir, linBot))) {
        ini->y  = (float)(linBot * TILE - ini->altura);
        ini->vy = 0.0f;
    }
    if (ini->vy < 0.0f && (tile_solido(f, colEsq, linTop) || tile_solido(f, colDir, linTop))) {
        ini->y  = (float)((linTop + 1) * TILE);
        ini->vy = 0.0f;
    }

    /* limites do mapa — impede o inimigo de sair pelos lados */
    if (ini->x < 0.0f) {
        ini->x  = 0.0f;
        if (ini->vx < 0) ini->vx = -ini->vx;
    }
    float maxX = (float)((COLUNAS - 1) * TILE - ini->largura);
    if (ini->x > maxX) {
        ini->x  = maxX;
        if (ini->vx > 0) ini->vx = -ini->vx;
    }
}

void aplicar_fisica_boss(Inimigo *ini, float dt) {
    ini->x += ini->vx * dt;
    ini->y += ini->vy * dt;

    if (ini->vx > 0.0f && ini->x >= ini->patrulhaMax) {
        ini->x  = ini->patrulhaMax;
        ini->vx = -ini->vx;
    } else if (ini->vx < 0.0f && ini->x <= ini->patrulhaMin) {
        ini->x  = ini->patrulhaMin;
        ini->vx = -ini->vx;
    }

    if (ini->vy > 0.0f && ini->y >= ini->patrulhaYMax) {
        ini->y  = ini->patrulhaYMax;
        ini->vy = -ini->vy;
    } else if (ini->vy < 0.0f && ini->y <= ini->patrulhaYMin) {
        ini->y  = ini->patrulhaYMin;
        ini->vy = -ini->vy;
    }
}

/* verifica colisao entre retangulos (usa VerificarColisao de fase.c) */
int colidiu(Inimigo *ini, Jogador *j) {
    Rectangle retJog = { j->x + JOGADOR_HITBOX_OFFSET_X, j->y, JOGADOR_HITBOX_LARGURA, (float)JOGADOR_ALTURA };
    Rectangle retIni = { ini->x,  ini->y,  (float)ini->largura, (float)ini->altura };
    return VerificarColisao(retJog, retIni);
}

/* ------------------------------------------------------------------ */
/* AtualizarInimigos — percorre lista, move cada inimigo               */
/* ------------------------------------------------------------------ */

void AtualizarInimigos(No *lista, Jogador *j, Fase *f, float dt, Sound sndKick) {
    No *atual = lista;

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            if (ini->tipo == BOSS) {
                ini->animTimer += dt;
                if (ini->animTimer >= 0.12f) {
                    ini->animTimer = 0.0f;
                    ini->animFrame = (ini->animFrame + 1) % 6;
                }
                if (ini->stuckTimer > 0.0f) {
                    if (ini->y < 576.0f) {
                        // descendo suavemente ate o chao
                        ini->y += ini->vy * dt;
                        if (ini->y >= 576.0f) {
                            ini->y  = 576.0f;
                            ini->vy = 0.0f;
                        }
                    } else {
                        // parado no chao, contagem regressiva
                        ini->stuckTimer -= dt;
                        if (ini->stuckTimer <= 0.0f) {
                            ini->stuckTimer = 0.0f;
                            ini->timerPouso = INTERVALO_POUSO;
                            ini->vx = VEL_BOSS;
                            ini->vy = -400.0f; // decolagem rapida para cima
                        }
                    }
                } else {
                    // voando
                    aplicar_fisica_boss(ini, dt);
                    ini->timerPouso -= dt;
                    if (ini->timerPouso <= 0.0f) {
                        ini->stuckTimer = 2.5f;
                        ini->vx = 0.0f;
                        ini->vy = 200.0f;
                    }
                }
            } else {
                /* PERSEGUIDOR: persegue o jogador e para na borda do mapa */
                if (ini->tipo == PERSEGUIDOR) {
                    int dir = (j->x > ini->x) ? 1 : -1;
                    int linPe     = (int)(ini->y + ini->altura) / TILE;
                    int colCentro = (int)(ini->x + ini->largura * 0.5f) / TILE;
                    int noChao    = tile_solido(f, colCentro, linPe);

                    if (noChao) {
                        int colFrente = (dir > 0) ? (int)(ini->x + ini->largura) / TILE
                                                  : (int)(ini->x - 1.0f) / TILE;
                        if (tile_solido(f, colFrente, linPe)) {
                            ini->vx = (float)dir * VEL_PERSEGUIDOR;
                        } else {
                            ini->vx = 0.0f;
                            ini->stuckTimer = 0.0f;
                        }
                    } else {
                        ini->vx = (float)dir * VEL_PERSEGUIDOR;
                    }
                }
                /* movimento e colisao com o terreno */
                aplicar_fisica_inimigo(ini, f, dt);

                /* animacao de caminhada */
                if (ini->tipo == CAMINHADOR || ini->tipo == PERSEGUIDOR) {
                    ini->animTimer += dt;
                    if (ini->animTimer >= 0.18f) {
                        ini->animTimer = 0.0f;
                        if (ini->spriteSet == 2)
                            ini->animFrame = (ini->animFrame + 1) % 3;
                        else
                            ini->animFrame = 1 - ini->animFrame;
                    }
                }
            }

            /* colisao com o jogador (todos os tipos) */
            if (j->estado != MORTO && colidiu(ini, j)) {
                float jogadorBase = j->y + (float)JOGADOR_ALTURA;
                float iniMeio     = ini->y + (float)ini->altura * 0.5f;
                int pisou_em_cima = (j->vyAnterior >= 1.5f) && (jogadorBase <= iniMeio);

                if (pisou_em_cima) {
                    PlaySound(sndKick);
                    ini->vida--;
                    if (ini->vida <= 0) {
                        ini->ativo = 0;
                        j->pontos += pontos_por_tipo(ini->tipo);
                    }
                    j->vy = FORCA_PULO;
                    j->noChao = 0;
                    if (j->estado == VIVO) {
                        j->estado = INVENCIVEL;
                        j->timerInvencivel = 0.3f;
                    }
                } else if (j->estado == VIVO) {
                    j->vidas--;
                    j->estado          = INVENCIVEL;
                    j->timerInvencivel = TEMPO_INVENCIVEL;
                    if (j->vidas <= 0) j->estado = MORTO;
                }
            }
        }

        atual = atual->proximo;   /* avanca na lista */
    }
}

/* ------------------------------------------------------------------ */
/* DesenharInimigos — percorre lista e renderiza                        */
/* ------------------------------------------------------------------ */

void DesenharInimigos(No *lista, float cameraX, float cameraYOffset,
                      Texture2D texIni1, Texture2D texIni2, Texture2D texIniRebaixado,
                      Texture2D texF2Ini1, Texture2D texF2Ini2, Texture2D texF2Ini3,
                      Texture2D texBoss[6]) {
    No *atual = lista;
    float zoom = CAMERA_ZOOM;

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            int screenX = (int)((ini->x - cameraX) * zoom);
            int screenY = (int)((ini->y - cameraYOffset) * zoom);
            int largura = (int)((float)ini->largura * zoom);
            int altura  = (int)((float)ini->altura  * zoom);

            if (ini->tipo == BOSS) {
                int frame = ini->animFrame % 6;
                Texture2D texB = texBoss[frame];
                if (texB.id > 0) {
                    float tw = (float)texB.width;
                    float th = (float)texB.height;
                    Rectangle src  = { 0, 0, tw, th };
                    Rectangle dest = { (float)screenX, (float)screenY, (float)largura, (float)altura };
                    if (ini->vx > 0) { src.x = tw; src.width = -tw; }
                    DrawTexturePro(texB, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                } else {
                    DrawRectangle(screenX, screenY, largura, altura, PURPLE);
                }
            } else if (ini->tipo == PERSEGUIDOR) {
                Texture2D texP = (ini->animFrame == 0) ? texF2Ini1 :
                                 (ini->animFrame == 1) ? texF2Ini2 : texF2Ini3;
                if (texP.id > 0) {
                    float tw = (float)texP.width;
                    float th = (float)texP.height;
                    Rectangle src  = { 0, 0, tw, th };
                    Rectangle dest = { (float)screenX, (float)screenY, (float)largura, (float)altura };
                    if (ini->vx > 0) { src.x = tw; src.width = -tw; }
                    DrawTexturePro(texP, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                } else {
                    DrawRectangle(screenX, screenY, largura, altura, ORANGE);
                }
            } else {
                /* CAMINHADOR: rebaixado apos 1 pulo, animado no estado normal */
                Texture2D tex;
                if (ini->vida <= 1 && texIniRebaixado.id > 0) {
                    tex = texIniRebaixado;
                } else {
                    tex = (ini->animFrame == 0 && texIni1.id > 0) ? texIni1 : texIni2;
                }

                if (tex.id > 0) {
                    float tw = (float)tex.width;
                    float th = (float)tex.height;
                    Rectangle src  = { 0, 0, tw, th };
                    Rectangle dest = { (float)screenX, (float)screenY, (float)largura, (float)altura };
                    if (ini->vx > 0) { src.x = tw; src.width = -tw; }
                    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                } else {
                    DrawRectangle(screenX, screenY, largura, altura, BROWN);
                }
            }
        }

        atual = atual->proximo;
    }
}

/* ------------------------------------------------------------------ */
/* LiberarInimigos — free em cada no da lista                          */
/* ------------------------------------------------------------------ */

void LiberarInimigos(No *lista) {
    while (lista) {
        No *tmp = lista;
        lista   = lista->proximo;
        free(tmp);               /* LIBERACAO DINAMICA */
    }
}
