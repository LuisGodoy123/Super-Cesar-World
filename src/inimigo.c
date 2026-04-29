#include "inimigo.h"
#include "fase.h"
#include <stdlib.h>

/* pontos concedidos ao eliminar cada tipo */
static int pontos_por_tipo(int tipo) {
    if (tipo == CAMINHADOR)  return 200;
    if (tipo == PERSEGUIDOR) return 300;
    if (tipo == BOSS)        return 1000;
    return 0;
}

/* dimensoes de cada tipo */
static void definir_dimensoes(Inimigo *ini) {
    if (ini->tipo == BOSS) { ini->largura = 64; ini->altura = 64; }
    else                   { ini->largura = 32; ini->altura = 32; }
}

/* ------------------------------------------------------------------ */
/* CriarInimigo — malloc de novo no (ALOCACAO DINAMICA)                */
/* ------------------------------------------------------------------ */

No *CriarInimigo(int tipo, float x, float y) {
    No *novo = (No *) malloc(sizeof(No));   /* ALOCACAO DINAMICA */

    novo->dados.x       = x;
    novo->dados.y       = y;
    novo->dados.vx      = (tipo == BOSS) ? VEL_BOSS : VEL_CAMINHADOR;
    novo->dados.vy      = 0.0f;
    novo->dados.tipo    = tipo;
    novo->dados.vida    = (tipo == BOSS) ? 5 : 1;
    novo->dados.ativo   = 1;
    novo->dados.origemX = x;
    novo->dados.timerTiro = INTERVALO_TIRO;
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

static int tile_solido(Fase *f, int col, int linha) {
    if (f == NULL) return 0;
    if (col < 0 || col >= COLUNAS || linha < 0 || linha >= LINHAS)
        return 0;
    int t = f->mapa[linha][col];
    return t == PLATAFORMA || t == BLOCO_INTERROGACAO || t == BLOCO_USADO;
}

static float velocidade_por_tipo(int tipo) {
    if (tipo == BOSS) return VEL_BOSS;
    if (tipo == PERSEGUIDOR) return VEL_PERSEGUIDOR;
    return VEL_CAMINHADOR;
}

static void aplicar_fisica_inimigo(Inimigo *ini, Fase *f, float dt) {
    if (ini->vx == 0.0f) ini->vx = velocidade_por_tipo(ini->tipo);

    ini->x += ini->vx * dt;

    int colEsq = (int)(ini->x) / TILE;
    int colDir = (int)(ini->x + ini->largura - 1) / TILE;
    int linTop = (int)(ini->y) / TILE;
    int linBot = (int)(ini->y + ini->altura - 1) / TILE;

    if (ini->vx < 0 && (tile_solido(f, colEsq, linTop) || tile_solido(f, colEsq, linBot))) {
        ini->x  = (float)((colEsq + 1) * TILE);
        ini->vx = -ini->vx;
    }
    if (ini->vx > 0 && (tile_solido(f, colDir, linTop) || tile_solido(f, colDir, linBot))) {
        ini->x  = (float)(colDir * TILE - ini->largura);
        ini->vx = -ini->vx;
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
}

/* verifica colisao entre retangulos (usa VerificarColisao de fase.c) */
static int colidiu(Inimigo *ini, Jogador *j) {
    Rectangle retJog = { j->x + JOGADOR_HITBOX_OFFSET_X, j->y, JOGADOR_HITBOX_LARGURA, (float)j->alturaAtual };
    Rectangle retIni = { ini->x,  ini->y,  (float)ini->largura, (float)ini->altura };
    return VerificarColisao(retJog, retIni);
}

/* ------------------------------------------------------------------ */
/* AtualizarInimigos — percorre lista, move cada inimigo               */
/* ------------------------------------------------------------------ */

void AtualizarInimigos(No *lista, Jogador *j, Fase *f, float dt) {
    No *atual = lista;

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            /* movimento e colisao com o terreno */
            aplicar_fisica_inimigo(ini, f, dt);

            if (ini->tipo == BOSS) {
                ini->timerTiro -= dt;
                if (ini->timerTiro <= 0.0f) {
                    ini->timerTiro = INTERVALO_TIRO;
                    /* TODO: criar projetil na direcao do jogador */
                }
            }

            /* colisao com o jogador */
            if (j->estado != MORTO && colidiu(ini, j)) {
                /* jogador pulou em cima? vy > 0 e base do jogador acima do centro do inimigo */
                int pisou = (j->vy > 0.0f &&
                             j->y + j->alturaAtual < ini->y + ini->altura * 0.6f);

                if (pisou) {
                    ini->vida--;
                    if (ini->vida <= 0) ini->ativo = 0;

                    j->vy      = FORCA_PULO / 2.0f;   /* pequeno quique */
                    j->pontos += pontos_por_tipo(ini->tipo);
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

void DesenharInimigos(No *lista, float cameraX) {
    No *atual = lista;
    float zoom = CAMERA_ZOOM;

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            int screenX = (int)((ini->x - cameraX) * zoom);
            int screenY = (int)((ini->y - CAMERA_Y_OFFSET) * zoom);
            int largura = (int)((float)ini->largura * zoom);
            int altura = (int)((float)ini->altura * zoom);

            if (ini->tipo == BOSS) {
                DrawRectangle(screenX, screenY, largura, altura, PURPLE);
                /* barra de vida do boss */
                DrawRectangle(screenX, screenY - (int)(10 * zoom), largura, (int)(6 * zoom), DARKGRAY);
                DrawRectangle(screenX, screenY - (int)(10 * zoom),
                              largura * ini->vida / 5, (int)(6 * zoom), RED);
            } else if (ini->tipo == PERSEGUIDOR) {
                DrawRectangle(screenX, screenY, largura, altura, ORANGE);
            } else {
                DrawRectangle(screenX, screenY, largura, altura, BROWN);
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
