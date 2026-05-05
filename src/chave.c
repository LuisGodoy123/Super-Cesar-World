#include "chave.h"
#include "fase.h"
#include "jogador.h"
#include <stddef.h>

void IniciarChave(Chave *chave, float x, float y) {
    chave->x = x;
    chave->y = y;
    chave->coletada = 0;
}

static int colidiu_chave_jogador(Chave *chave, Jogador *j) {
    Rectangle rj = { j->x + JOGADOR_HITBOX_OFFSET_X, j->y,
                     (float)JOGADOR_HITBOX_LARGURA, (float)j->alturaAtual };
    Rectangle rc = { chave->x, chave->y, (float)TILE, (float)TILE };
    return VerificarColisao(rj, rc);
}

void AtualizarChave(Chave *chave, Jogador *j) {
    if (chave->coletada || j == NULL || j->estado == MORTO) return;
    if (colidiu_chave_jogador(chave, j)) chave->coletada = 1;
}

void DesenharChave(Chave chave, float cameraX, float cameraYOffset) {
    if (chave.coletada) return;
    float zoom = CAMERA_ZOOM;
    int ts = (int)(TILE * zoom);
    int sx = (int)((chave.x - cameraX) * zoom);
    int sy = (int)((chave.y - cameraYOffset) * zoom);
    if (sx + ts < 0 || sx > GetScreenWidth()) return;
    DrawRectangle(sx + 4, sy + 4, ts - 8, ts - 8, GOLD);
    DrawRectangleLines(sx + 2, sy + 2, ts - 4, ts - 4, ORANGE);
    DrawText("K", sx + ts / 3, sy + ts / 5, (int)(18 * zoom), DARKBROWN);
}
