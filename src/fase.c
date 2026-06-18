#include "fase.h"
#include <string.h>

int tile_solido(Fase *f, int col, int linha) {
    if (col < 0 || col >= COLUNAS || linha < 0 || linha >= LINHAS) return 0;
    return f->mapa[linha][col] == BLOCO;
}

void colocar_bloco(Fase *f, int l, int c) {
    f->mapa[l][c] = BLOCO;
    f->blocos[l][c] = (Bloco){ BLOCO_TIPO_NORMAL, BLOCO_ESTADO_ATIVO, 0.0f, 0 };
}

void colocar_powerup(Fase *f, int l, int c) {
    f->mapa[l][c] = BLOCO;
    f->blocos[l][c] = (Bloco){ BLOCO_TIPO_POWERUP, BLOCO_ESTADO_ATIVO, 0.0f, 0 };
}

void preencher_chao(Fase *f, int cIni, int cFim, int linhaTopo) {
    if (cIni < 0) cIni = 0;
    if (cFim >= COLUNAS) cFim = COLUNAS - 1;
    if (linhaTopo < 0) linhaTopo = 0;
    if (linhaTopo >= LINHAS) linhaTopo = LINHAS - 1;

    for (int c = cIni; c <= cFim; c++)
        for (int l = linhaTopo; l < LINHAS; l++)
            colocar_bloco(f, l, c);
}

void colocar_porta(Fase *f, int l, int c) {
    if (l < 0 || l >= LINHAS || c < 0 || c >= COLUNAS) return;
    f->mapa[l][c] = PORTA;
}

void plataforma(Fase *f, int cIni, int cFim, int linha) {
    for (int c = cIni; c <= cFim; c++)
        colocar_bloco(f, linha, c);
}

void preencher_fase1(Fase *f) {
    preencher_chao(f,   0,  15, 17);
    preencher_chao(f,  16,  26, 16);
    preencher_chao(f,  27,  51, 17);
    preencher_chao(f,  52,  53, 16);
    preencher_chao(f,  54,  61, 15);
    preencher_chao(f,  62,  67, 16);
    preencher_chao(f,  68,  78, 17);
    preencher_chao(f,  79,  82, 18);
    preencher_chao(f,  83,  95, 17);
    preencher_chao(f,  96, 103, 16);
    preencher_chao(f, 104, 116, 17);
    preencher_chao(f, 117, 122, 16);
    preencher_chao(f, 123, 129, 17);

    for (int c =  8; c <= 12; c++) colocar_bloco(f, 14, c);
    for (int c = 44; c <= 49; c++) colocar_bloco(f, 14, c);
    for (int c = 70; c <= 75; c++) colocar_bloco(f, 14, c);
    for (int c = 85; c <= 90; c++) colocar_bloco(f, 13, c);

    for (int c =  3; c <=  7; c++) f->mapa[16][c] = MOEDA;
    for (int c =  9; c <= 11; c++) f->mapa[13][c] = MOEDA;
    for (int c = 41; c <= 46; c++) f->mapa[16][c] = MOEDA;
    for (int c = 55; c <= 59; c++) f->mapa[14][c] = MOEDA;
    for (int c = 71; c <= 74; c++) f->mapa[13][c] = MOEDA;
    for (int c = 86; c <= 89; c++) f->mapa[12][c] = MOEDA;
    for (int c = 109; c <= 113; c++) f->mapa[16][c] = MOEDA;

    colocar_bloco(f, 13, 28); colocar_bloco(f, 13, 29);
    colocar_bloco(f, 13, 30); colocar_bloco(f, 13, 31);
    colocar_bloco(f, 13, 32); colocar_bloco(f, 13, 33);

    colocar_powerup(f, 10,  7);
    colocar_powerup(f, 10, 50);

    colocar_bloco(f, 13, 17); colocar_bloco(f, 13, 18); colocar_bloco(f, 13, 19);
    for (int c = 22; c <= 28; c++) colocar_bloco(f, 9, c);
    colocar_powerup(f, 9, 31);
    for (int c = 22; c <= 28; c++) f->mapa[8][c] = MOEDA;

    for (int c = 68; c <= 74; c++) colocar_bloco(f, 10, c);
    colocar_bloco(f, 13, 81);
    for (int c = 68; c <= 74; c++) f->mapa[8][c] = MOEDA;

    colocar_bloco(f, 13, 97); colocar_bloco(f, 13, 98); colocar_bloco(f, 13, 99);
    for (int c = 102; c <= 108; c++) f->mapa[8][c] = MOEDA;

    plataforma(f, 104, 106, 13);
    plataforma(f, 114, 116, 13);
    colocar_bloco(f, 12, 103);
    colocar_bloco(f, 12, 107);
    colocar_bloco(f, 12, 113);
    colocar_bloco(f, 12, 117);
    plataforma(f, 109, 111, 9);
    colocar_bloco(f, 8, 108);
    colocar_bloco(f, 8, 112);

    // extensão: colunas 130-163
    preencher_chao(f, 130, 140, 17);
    preencher_chao(f, 141, 148, 15);
    preencher_chao(f, 149, 155, 17);
    preencher_chao(f, 156, 163, 16);

    for (int c = 131; c <= 136; c++) colocar_bloco(f, 13, c);
    for (int c = 143; c <= 148; c++) colocar_bloco(f, 11, c);
    plataforma(f, 151, 155, 9);
    colocar_bloco(f, 8, 150);
    colocar_bloco(f, 8, 156);

    colocar_powerup(f, 13, 133);
    colocar_powerup(f,  9, 152);

    for (int c = 132; c <= 135; c++) f->mapa[12][c] = MOEDA;
    for (int c = 144; c <= 147; c++) f->mapa[10][c] = MOEDA;
    for (int c = 152; c <= 154; c++) f->mapa[ 8][c] = MOEDA;
    for (int c = 157; c <= 161; c++) f->mapa[15][c] = MOEDA;

    colocar_porta(f, 14, 161);
    colocar_porta(f, 15, 161);

    preencher_chao(f, 162, 164, 16);
}

void preencher_fase2(Fase *f) {
    preencher_chao(f,  0,  58, 21);
    preencher_chao(f, 65, 129, 21);

    for (int c =  4; c <=  9; c++) colocar_bloco(f, 18, c);
    for (int c = 14; c <= 19; c++) colocar_bloco(f, 15, c);
    for (int c = 37; c <= 42; c++) colocar_bloco(f, 13, c);
    for (int c = 49; c <= 54; c++) colocar_bloco(f, 16, c);
    for (int c = 59; c <= 64; c++) colocar_bloco(f, 17, c);
    for (int c = 73; c <= 78; c++) colocar_bloco(f, 18, c);
    for (int c = 80; c <= 85; c++) colocar_bloco(f, 14, c);
    for (int c = 92; c <= 97; c++) colocar_bloco(f, 17, c);
    for (int c = 108; c <= 113; c++) colocar_bloco(f, 11, c);

    for (int c =  5; c <=  8; c++) f->mapa[17][c] = MOEDA;
    for (int c = 15; c <= 18; c++) f->mapa[14][c] = MOEDA;
    for (int c = 38; c <= 41; c++) f->mapa[12][c] = MOEDA;
    for (int c = 50; c <= 53; c++) f->mapa[15][c] = MOEDA;
    for (int c = 60; c <= 63; c++) f->mapa[16][c] = MOEDA;
    for (int c = 74; c <= 77; c++) f->mapa[17][c] = MOEDA;
    for (int c = 81; c <= 84; c++) f->mapa[13][c] = MOEDA;
    for (int c = 109; c <= 112; c++) f->mapa[10][c] = MOEDA;

    colocar_bloco(f,   17, 31); colocar_powerup(f, 17, 32);
    colocar_bloco(f,   17, 33); colocar_bloco(f,   17, 34);
    colocar_powerup(f, 17, 35); colocar_bloco(f,   17, 36);
    colocar_powerup(f, 14, 33);

    preencher_chao(f, 61, 62, 21);

    for (int c = 100; c <= 106; c++) colocar_bloco(f, 14, c);
    colocar_bloco(f, 18, 113);
    for (int c = 100; c <= 106; c++) f->mapa[13][c] = MOEDA;

    colocar_bloco(f, 13, 100);
    colocar_bloco(f, 13, 106);
    colocar_bloco(f, 10, 108);
    colocar_bloco(f, 10, 113);

    // extensão: colunas 130-163
    preencher_chao(f, 130, 163, 21);

    for (int c = 131; c <= 137; c++) colocar_bloco(f, 17, c);
    for (int c = 141; c <= 147; c++) colocar_bloco(f, 14, c);
    for (int c = 152; c <= 158; c++) colocar_bloco(f, 11, c);
    colocar_bloco(f, 10, 151);
    colocar_bloco(f, 10, 159);

    colocar_powerup(f, 17, 134);
    colocar_powerup(f, 11, 155);

    for (int c = 132; c <= 136; c++) f->mapa[16][c] = MOEDA;
    for (int c = 142; c <= 146; c++) f->mapa[13][c] = MOEDA;
    for (int c = 153; c <= 157; c++) f->mapa[10][c] = MOEDA;

    colocar_porta(f, 19, 161);
    colocar_porta(f, 20, 161);

    preencher_chao(f, 162, 164, 21);
}

void preencher_fase3(Fase *f) {
    preencher_chao(f, 0, 13, 21);
    preencher_chao(f, 19, 33, 21);
    preencher_chao(f, 41, 53, 21);
    preencher_chao(f, 62, 129, 21);

    for (int c =  2; c <=  6; c++) colocar_bloco(f, 17, c);
    for (int c =  9; c <= 13; c++) colocar_bloco(f, 14, c);
    for (int c = 16; c <= 20; c++) colocar_bloco(f, 11, c);
    for (int c = 23; c <= 27; c++) colocar_bloco(f, 15, c);
    for (int c = 29; c <= 33; c++) colocar_bloco(f, 12, c);
    for (int c = 29; c <= 33; c++) colocar_bloco(f, 18, c);
    for (int c = 36; c <= 41; c++) colocar_bloco(f,  9, c);
    for (int c = 43; c <= 47; c++) colocar_bloco(f, 13, c);
    for (int c = 49; c <= 53; c++) colocar_bloco(f, 16, c);
    for (int c = 57; c <= 59; c++) colocar_bloco(f, 13, c);
    colocar_bloco(f, 19, 57);
    colocar_bloco(f, 19, 58);
    for (int c = 63; c <= 67; c++) colocar_bloco(f, 14, c);

    for (int c =  3; c <=  5; c++) f->mapa[16][c] = MOEDA;
    for (int c = 10; c <= 12; c++) f->mapa[13][c] = MOEDA;
    for (int c = 17; c <= 19; c++) f->mapa[10][c] = MOEDA;
    for (int c = 24; c <= 26; c++) f->mapa[14][c] = MOEDA;
    for (int c = 37; c <= 40; c++) f->mapa[ 8][c] = MOEDA;
    for (int c = 57; c <= 60; c++) f->mapa[ 9][c] = MOEDA;
    for (int c = 64; c <= 66; c++) f->mapa[13][c] = MOEDA;

    colocar_bloco(f,   17,  7); colocar_powerup(f, 17,  8);
    colocar_bloco(f,   17,  9); colocar_bloco(f,   17, 10);
    colocar_bloco(f,   17, 11);
    colocar_powerup(f, 14,  5);

    for (int c = 68; c <= 70; c++) colocar_bloco(f, 17, c);
    for (int c = 70; c <= 76; c++) colocar_bloco(f, 13, c);
    colocar_bloco(f, 13, 79);
    for (int c = 70; c <= 76; c++) f->mapa[12][c] = MOEDA;

    colocar_bloco(f, 20, 75);
    colocar_bloco(f, 20, 76);
    colocar_bloco(f, 19, 76);

    plataforma(f, 80, 83, 13);
    colocar_bloco(f, 20, 104);
    colocar_bloco(f, 20, 105);
    colocar_bloco(f, 19, 104);

    preencher_chao(f, 130, 164, 21);
}

void CarregarFase(Fase *f, int n) {
    memset(f->mapa,   VAZIO, sizeof(f->mapa));
    memset(f->blocos, 0,     sizeof(f->blocos));
    f->numero  = n;
    f->cameraX = 0.0f;

    if (n == 1) {
        f->corFundo      = (Color){  92, 148, 252, 255 };
        f->cameraYOffset = 128.0f;
        preencher_fase1(f);
    } else if (n == 2) {
        f->corFundo      = (Color){ 255, 140,  60, 255 };
        f->cameraYOffset = 196.0f;
        preencher_fase2(f);
    } else if (n == 3) {
        f->corFundo      = (Color){  30,  10,  60, 255 };
        f->cameraYOffset = 196.0f;
        preencher_fase3(f);
    }
}

void DesenharFase(Fase *f, Texture2D texBloco, Texture2D texTijoloCinza, Texture2D texTerra,
                  Texture2D texCafe, Texture2D texFundo2) {
    ClearBackground(f->corFundo);
    if (f->numero == 2 && texFundo2.id > 0) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();
        DrawTexturePro(texFundo2,
            (Rectangle){0, 0, (float)texFundo2.width, (float)texFundo2.height},
            (Rectangle){0, 0, sw, sh},
            (Vector2){0, 0}, 0.0f, WHITE);
    }

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            int tipo = f->mapa[i][j];
            if (tipo == VAZIO || tipo == MOEDA) continue;

            float zoom = CAMERA_ZOOM;
            int screenX = (int)((j       * TILE - (int)f->cameraX) * zoom);
            int screenY = (int)((i       * TILE - f->cameraYOffset) * zoom);
            int nextX   = (int)(((j + 1) * TILE - (int)f->cameraX) * zoom);
            int nextY   = (int)(((i + 1) * TILE - f->cameraYOffset) * zoom);
            int tileSize = nextX - screenX;
            int tileH    = nextY - screenY;

            if (screenX + tileSize < 0 || screenX > GetScreenWidth()) continue;
            if (screenY + tileSize < 0 || screenY > GetScreenHeight()) continue;

            int x = screenX;
            int y = screenY;

            Bloco b = f->blocos[i][j];

            if (tipo == PORTA) {
                int isTop = (i == 0 || f->mapa[i - 1][j] != PORTA);
                Color portaFundo = (Color){  20, 120,  50, 255 };
                Color portaBorda = (Color){   5,  60,  25, 255 };
                Color portaLuz   = (Color){  80, 200, 110, 255 };
                DrawRectangle(x, y, tileSize, tileH, portaFundo);
                DrawRectangleLines(x, y, tileSize, tileH, portaBorda);
                DrawLine(x + 2, y + 2, x + tileSize - 3, y + 2, portaLuz);
                DrawLine(x + 2, y + 2, x + 2, y + tileH - 3, portaLuz);
                if (isTop) {
                    int fSize = tileSize * 2 / 3;
                    const char *num = TextFormat("%d", f->numero + 1);
                    int tw = MeasureText(num, fSize);
                    DrawText(num, x + (tileSize - tw) / 2, y + (tileH - fSize) / 2, fSize, portaLuz);
                }
                continue;
            }

            if (b.tipo == BLOCO_TIPO_POWERUP) {
                if (b.estado == BLOCO_ESTADO_ATIVO) {
                    float bSize = (float)TILE * zoom;
                    if (texBloco.id > 0) {
                        float pad = 0.20f;
                        Rectangle src  = {
                            pad * texBloco.width,
                            pad * texBloco.height,
                            (1.0f - 2*pad) * texBloco.width,
                            (1.0f - 2*pad) * texBloco.height
                        };
                        Rectangle dest = { (float)x, (float)y, bSize, bSize };
                        DrawTexturePro(texBloco, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        Color amarelo       = (Color){ 255, 200,   0, 255 };
                        Color amareloBorda  = (Color){ 150,  90,   0, 255 };
                        Color amareloLuz    = (Color){ 255, 235, 110, 255 };
                        Color amareloSombra = (Color){ 185, 115,   0, 255 };

                        DrawRectangle(x, y, tileSize, tileH, amarelo);
                        DrawRectangleLines(x, y, tileSize, tileSize, amareloBorda);
                        DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, amareloLuz);
                        DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, amareloLuz);
                        DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, amareloSombra);
                        DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, amareloSombra);

                        int fSize = tileSize * 2 / 3;
                        int tw = MeasureText("?", fSize);
                        DrawText("?", x + (tileSize - tw) / 2, y + (tileSize - fSize) / 2, fSize, amareloBorda);
                    }
                } else {
                    if (texTijoloCinza.id > 0) {
                        float tw = (float)texTijoloCinza.width;
                        float th = (float)texTijoloCinza.height;
                        float sq = tw < th ? tw : th;
                        Rectangle src  = { (tw - sq) / 2.0f, (th - sq) / 2.0f, sq, sq };
                        Rectangle dest = { (float)x, (float)y, (float)tileSize, (float)tileH };
                        DrawTexturePro(texTijoloCinza, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        Color tijoloBase   = (Color){ 150, 150, 150, 255 };
                        Color tijoloBorda  = (Color){  90,  90,  90, 255 };
                        Color tijoloLuz    = (Color){ 175, 175, 175, 255 };
                        Color tijoloSombra = (Color){ 110, 110, 110, 255 };

                        DrawRectangle(x, y, tileSize, tileH, tijoloBase);
                        DrawRectangleLines(x, y, tileSize, tileSize, tijoloBorda);
                        DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, tijoloLuz);
                        DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, tijoloLuz);
                        DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                        DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                        DrawLine(x + 1, y + tileSize / 2, x + tileSize - 2, y + tileSize / 2, tijoloSombra);
                    }

                    if (!b.cafeColetado) {
                        int pad   = (int)(4.0f * zoom);
                        int cafeY = (int)(((float)(i * TILE) + b.cafeOffset - f->cameraYOffset) * zoom);
                        if (texCafe.id > 0) {
                            int cafeSize = (int)(tileSize * 0.9f);
                            int cOff     = (tileSize - cafeSize) / 2;
                            Rectangle src  = { 0, 0, (float)texCafe.width, (float)texCafe.height };
                            Rectangle dest = { (float)(x + cOff), (float)(cafeY + cOff),
                                               (float)cafeSize, (float)cafeSize };
                            DrawTexturePro(texCafe, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                        } else {
                            DrawRectangle(x + pad, cafeY + pad, tileSize - 2*pad, tileSize - 2*pad, (Color){120, 70, 30, 255});
                            DrawRectangleLines(x + pad, cafeY + pad, tileSize - 2*pad, tileSize - 2*pad, (Color){60, 30, 10, 255});
                        }
                    }
                }
            } else {
                int topo = (i == 0 || f->mapa[i - 1][j] != BLOCO);
                if (topo) {
                    if (texTijoloCinza.id > 0) {
                        float tw = (float)texTijoloCinza.width;
                        float th = (float)texTijoloCinza.height;
                        float sq = tw < th ? tw : th;
                        Rectangle src  = { (tw - sq) / 2.0f, (th - sq) / 2.0f, sq, sq };
                        Rectangle dest = { (float)x, (float)y, (float)tileSize, (float)tileH };
                        DrawTexturePro(texTijoloCinza, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        Color tijoloBase   = (Color){ 150, 150, 150, 255 };
                        Color tijoloBorda  = (Color){  90,  90,  90, 255 };
                        Color tijoloLuz    = (Color){ 175, 175, 175, 255 };
                        Color tijoloSombra = (Color){ 110, 110, 110, 255 };

                        DrawRectangle(x, y, tileSize, tileH, tijoloBase);
                        DrawRectangleLines(x, y, tileSize, tileSize, tijoloBorda);
                        DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, tijoloLuz);
                        DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, tijoloLuz);
                        DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                        DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                        DrawLine(x + 1, y + tileSize / 2, x + tileSize - 2, y + tileSize / 2, tijoloSombra);
                    }
                } else {
                    if (texTerra.id > 0) {
                        float tw = (float)texTerra.width;
                        float th = (float)texTerra.height;
                        float sq = tw < th ? tw : th;
                        Rectangle src  = { (tw - sq) / 2.0f, (th - sq) / 2.0f, sq, sq };
                        Rectangle dest = { (float)x, (float)y, (float)tileSize, (float)tileH };
                        DrawTexturePro(texTerra, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                    Color terra       = (Color){ 120,  95,  60, 255 };
                    Color terraBorda  = (Color){  80,  65,  45, 255 };
                    Color terraLuz    = (Color){ 145, 115,  75, 255 };
                    Color terraSombra = (Color){  90,  70,  50, 255 };

                    DrawRectangle(x, y, tileSize, tileH, terra);
                    DrawRectangleLines(x, y, tileSize, tileSize, terraBorda);
                    DrawLine(x + 2, y + 4,  x + tileSize - 3, y + 4,  terraSombra);
                    DrawLine(x + 3, y + 9,  x + tileSize - 4, y + 9,  terraSombra);
                    DrawLine(x + 2, y + 15, x + tileSize - 3, y + 15, terraSombra);
                    DrawLine(x + 3, y + 20, x + tileSize - 4, y + 20, terraSombra);
                    DrawPixel(x +  4, y +  6, terraSombra); DrawPixel(x + 10, y +  4, terraSombra);
                    DrawPixel(x + 16, y +  7, terraSombra); DrawPixel(x + 22, y +  5, terraSombra);
                    DrawPixel(x +  7, y + 14, terraSombra); DrawPixel(x + 13, y + 12, terraSombra);
                    DrawPixel(x + 20, y + 15, terraSombra); DrawPixel(x +  5, y + 20, terraSombra);
                    DrawPixel(x + 11, y + 19, terraSombra); DrawPixel(x + 18, y + 21, terraSombra);
                    DrawPixel(x +  3, y +  9, terraLuz); DrawPixel(x + 12, y +  8, terraLuz);
                    DrawPixel(x + 19, y + 10, terraLuz); DrawPixel(x +  8, y + 17, terraLuz);
                    DrawPixel(x + 15, y + 16, terraLuz); DrawPixel(x + 23, y + 18, terraLuz);
                    DrawPixel(x +  2, y + 12, terraLuz); DrawPixel(x +  9, y + 22, terraLuz);
                    DrawPixel(x + 17, y + 13, terraLuz); DrawPixel(x + 24, y +  8, terraLuz);
                    DrawPixel(x +  6, y +  2, terraSombra); DrawPixel(x + 14, y +  3, terraSombra);
                    DrawPixel(x + 21, y +  2, terraSombra);
                    }
                }
            }
        }
    }
}

int VerificarColisao(Rectangle a, Rectangle b) {
    if (a.x + a.width  <= b.x) return 0;
    if (b.x + b.width  <= a.x) return 0;
    if (a.y + a.height <= b.y) return 0;
    if (b.y + b.height <= a.y) return 0;
    return 1;
}
