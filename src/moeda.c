#include "moeda.h"
#include <stdlib.h>

int colidiu_moeda_jogador(NoMoeda *m, Jogador *j) {
    Rectangle retJog = { j->x + JOGADOR_HITBOX_OFFSET_X, j->y, JOGADOR_HITBOX_LARGURA, (float)JOGADOR_ALTURA };
    Rectangle retMoeda = { m->x, m->y, (float)TILE, (float)TILE };
    return VerificarColisao(retJog, retMoeda);
}

NoMoeda *CriarMoeda(float x, float y) {
    NoMoeda *novo = (NoMoeda *) malloc(sizeof(NoMoeda));
    if (novo == NULL) return NULL;

    novo->x = x;
    novo->y = y;
    novo->coletada = 0;
    novo->proximo = NULL;
    return novo;
}

void AdicionarMoeda(NoMoeda **lista, float x, float y) {
    NoMoeda *novo = CriarMoeda(x, y);
    if (novo == NULL) return;

    novo->proximo = *lista;
    *lista = novo;
}

void CarregarMoedasDaFase(NoMoeda **lista, Fase *f) {
    if (lista == NULL || f == NULL) return;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (f->mapa[i][j] == MOEDA) {
                AdicionarMoeda(lista, (float)(j * TILE), (float)(i * TILE));
                f->mapa[i][j] = VAZIO;
            }
        }
    }
}

void AtualizarMoedas(NoMoeda *lista, Jogador *j, Sound sndCoin) {
    if (j == NULL || j->estado == MORTO) return;

    NoMoeda *atual = lista;
    while (atual != NULL) {
        if (!atual->coletada && colidiu_moeda_jogador(atual, j)) {
            atual->coletada = 1;
            j->pontos += PONTOS_MOEDA;
            j->moedas++;
            PlaySound(sndCoin);
        }
        atual = atual->proximo;
    }
}

void DesenharMoedas(NoMoeda *lista, float cameraX, float cameraYOffset, Texture2D *texMoedas, int numFrames, float tempoAnim) {
    NoMoeda *atual = lista;
    float zoom = CAMERA_ZOOM;
    int tileSize = (int)(TILE * zoom);

    int frameAtual = 0;
    if (numFrames > 0)
        frameAtual = (int)(tempoAnim * 10.0f) % numFrames;

    while (atual != NULL) {
        if (!atual->coletada) {
            int screenX = (int)((atual->x - cameraX) * zoom);
            int screenY = (int)((atual->y - cameraYOffset) * zoom);

            if (screenX + tileSize >= 0 && screenX <= GetScreenWidth()) {
                if (numFrames > 0 && texMoedas[frameAtual].id > 0) {
                    Texture2D tex = texMoedas[frameAtual];
                    float coinSize = tileSize * 1.0f;
                    float offset = (coinSize - tileSize) / 2.0f;
                    Rectangle src  = { 0, 0, (float)tex.width, (float)tex.height };
                    Rectangle dest = { (float)screenX - offset, (float)screenY - offset, coinSize, coinSize };
                    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                } else {
                    int cx = screenX + tileSize / 2;
                    int cy = screenY + tileSize / 2;
                    float raio = (float)TILE * 0.30f * zoom;
                    DrawCircle(cx, cy, raio, GOLD);
                    DrawCircleLines(cx, cy, raio, ORANGE);
                }
            }
        }

        atual = atual->proximo;
    }
}

void LiberarMoedas(NoMoeda *lista) {
    while (lista != NULL) {
        NoMoeda *tmp = lista;
        lista = lista->proximo;
        free(tmp);
    }
}
