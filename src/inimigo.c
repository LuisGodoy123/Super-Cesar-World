#include "inimigo.h"
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
/* Comportamentos internos                                              */
/* ------------------------------------------------------------------ */

static void mover_caminhador(Inimigo *ini, float dt) {
    ini->x += ini->vx * dt;

    /* inverte direcao ao sair do alcance de patrulha */
    if (ini->x < ini->origemX - PATROL_RANGE) {
        ini->x  = ini->origemX - PATROL_RANGE;
        ini->vx = VEL_CAMINHADOR;
    }
    if (ini->x > ini->origemX + PATROL_RANGE) {
        ini->x  = ini->origemX + PATROL_RANGE;
        ini->vx = -VEL_CAMINHADOR;
    }
}

static void mover_perseguidor(Inimigo *ini, Jogador *j, float dt) {
    /* segue o jogador horizontalmente */
    if (j->x > ini->x) ini->vx =  VEL_PERSEGUIDOR;
    else                ini->vx = -VEL_PERSEGUIDOR;

    ini->x += ini->vx * dt;
}

static void mover_boss(Inimigo *ini, Jogador *j, float dt) {
    /* move-se lentamente em direcao ao jogador */
    if (j->x > ini->x) ini->vx =  VEL_BOSS;
    else                ini->vx = -VEL_BOSS;

    ini->x += ini->vx * dt;

    /* contagem regressiva para tiro */
    ini->timerTiro -= dt;
    if (ini->timerTiro <= 0.0f) {
        ini->timerTiro = INTERVALO_TIRO;
        /* TODO: criar projetil na direcao do jogador */
    }
}

/* verifica colisao entre retangulos (usa VerificarColisao de fase.c) */
static int colidiu(Inimigo *ini, Jogador *j) {
    Rectangle retJog = { j->x,    j->y,    JOGADOR_LARGURA, (float)j->alturaAtual };
    Rectangle retIni = { ini->x,  ini->y,  (float)ini->largura, (float)ini->altura };
    return VerificarColisao(retJog, retIni);
}

/* ------------------------------------------------------------------ */
/* AtualizarInimigos — percorre lista, move cada inimigo               */
/* ------------------------------------------------------------------ */

void AtualizarInimigos(No *lista, Jogador *j, float dt) {
    No *atual = lista;

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            /* movimento por tipo */
            if      (ini->tipo == CAMINHADOR)  mover_caminhador(ini, dt);
            else if (ini->tipo == PERSEGUIDOR) mover_perseguidor(ini, j, dt);
            else if (ini->tipo == BOSS)        mover_boss(ini, j, dt);

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

    while (atual != NULL) {
        Inimigo *ini = &atual->dados;

        if (ini->ativo) {
            int screenX = (int)(ini->x - cameraX);
            int screenY = (int)(ini->y);

            if (ini->tipo == BOSS) {
                DrawRectangle(screenX, screenY, ini->largura, ini->altura, PURPLE);
                /* barra de vida do boss */
                DrawRectangle(screenX, screenY - 10, ini->largura, 6, DARKGRAY);
                DrawRectangle(screenX, screenY - 10,
                              ini->largura * ini->vida / 5, 6, RED);
            } else if (ini->tipo == PERSEGUIDOR) {
                DrawRectangle(screenX, screenY, ini->largura, ini->altura, ORANGE);
            } else {
                DrawRectangle(screenX, screenY, ini->largura, ini->altura, BROWN);
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
