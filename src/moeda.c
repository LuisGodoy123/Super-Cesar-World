#include "moeda.h"
#include <stdlib.h>

// verifica colisao entre o retangulo da moeda e do jogador
static int colidiu_moeda_jogador(NoMoeda *m, Jogador *j) {
    Rectangle retJog = { j->x, j->y, JOGADOR_LARGURA, (float)j->alturaAtual };
    Rectangle retMoeda = { m->x, m->y, (float)TILE, (float)TILE };
    return VerificarColisao(retJog, retMoeda);
}

// CriarMoeda — malloc de novo no (ALOCACAO DINAMICA)

NoMoeda *CriarMoeda(float x, float y) {
    NoMoeda *novo = (NoMoeda *) malloc(sizeof(NoMoeda));   /* ALOCACAO DINAMICA */
    if (novo == NULL) return NULL;

    novo->x = x;
    novo->y = y;
    novo->coletada = 0;
    novo->proximo = NULL;
    return novo;
}

// AdicionarMoeda — insere no inicio da lista

void AdicionarMoeda(NoMoeda **lista, float x, float y) {
    NoMoeda *novo = CriarMoeda(x, y);
    if (novo == NULL) return;

    novo->proximo = *lista;
    *lista = novo;
}

// Comportamentos internos

// CarregarMoedasDaFase — le matriz e cria a lista de moedas


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

// AtualizarMoedas — percorre lista e verifica coleta

void AtualizarMoedas(NoMoeda *lista, Jogador *j) {
    if (j == NULL || j->estado == MORTO) return;

    NoMoeda *atual = lista;
    while (atual != NULL) {
        if (!atual->coletada && colidiu_moeda_jogador(atual, j)) {
            atual->coletada = 1;
            j->pontos += PONTOS_MOEDA;
        }
        atual = atual->proximo;
    }
}

// DesenharMoedas — percorre lista e renderiza


void DesenharMoedas(NoMoeda *lista, float cameraX, Texture2D texMoeda) {
    NoMoeda *atual = lista;
    float zoom = CAMERA_ZOOM;
    int tileSize = (int)(TILE * zoom);
    while (atual != NULL) {
        if (!atual->coletada) {
            int screenX = (int)((atual->x - cameraX) * zoom);
            int screenY = (int)((atual->y - CAMERA_Y_OFFSET) * zoom);

            if (screenX + tileSize >= 0 && screenX <= GetScreenWidth()) {
                if (texMoeda.id > 0) {
                    Rectangle src  = { 0, 0, (float)texMoeda.width, (float)texMoeda.height };
                    Rectangle dest = { (float)screenX, (float)screenY, (float)tileSize, (float)tileSize };
                    DrawTexturePro(texMoeda, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
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


// LiberarMoedas — free em cada no da lista

void LiberarMoedas(NoMoeda *lista) {
    while (lista != NULL) {
        NoMoeda *tmp = lista;
        lista = lista->proximo;
        free(tmp);               // LIBERACAO DINAMICA
    }
}
