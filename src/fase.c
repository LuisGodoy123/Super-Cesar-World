#include "fase.h"
#include <string.h>

// Mapas das fases

static void preencher_chao(int mapa[LINHAS][COLUNAS], int cIni, int cFim, int linhaTopo) {
    if (cIni < 0) cIni = 0;
    if (cFim >= COLUNAS) cFim = COLUNAS - 1;
    if (linhaTopo < 0) linhaTopo = 0;
    if (linhaTopo >= LINHAS) linhaTopo = LINHAS - 1;

    for (int c = cIni; c <= cFim; c++)
        for (int l = linhaTopo; l < LINHAS; l++)
            mapa[l][c] = PLATAFORMA;
}

static void preencher_fase1(int mapa[LINHAS][COLUNAS]) {
    // Inspirado em Yoshi's Island 1 (Super Mario World)
    // Ritmo: plano → subida leve → plano → vale raso → plano
    // Linha base: 17 | elevacao: 15-16 | vale raso: 18

    // Inicio totalmente plano
    preencher_chao(mapa,   0,  15, 17);

    // Subida leve de 1 bloco
    preencher_chao(mapa,  16,  26, 16);

    // Retorno ao plano
    preencher_chao(mapa,  27,  35, 17);

    // Vale raso (exige salto simples para subir de volta)
    preencher_chao(mapa,  36,  39, 18);

    // Espaco de respiro
    preencher_chao(mapa,  40,  51, 17);

    // Degraus naturais: subida progressiva (2 degraus)
    preencher_chao(mapa,  52,  53, 16);
    preencher_chao(mapa,  54,  61, 15);

    // Descida suave e retorno ao plano
    preencher_chao(mapa,  62,  67, 16);
    preencher_chao(mapa,  68,  78, 17);

    // Vale raso
    preencher_chao(mapa,  79,  82, 18);

    // Longo espaco de respiro
    preencher_chao(mapa,  83,  95, 17);

    // Subida leve
    preencher_chao(mapa,  96, 103, 16);

    // Pequeno vale
    preencher_chao(mapa, 104, 107, 18);

    // Plano
    preencher_chao(mapa, 108, 116, 17);

    // Elevacao leve antes do fim
    preencher_chao(mapa, 117, 122, 16);

    // Chegada final plana
    preencher_chao(mapa, 123, 129, 17);

    // Plataformas elevadas simples
    for (int c =  8; c <= 12; c++) mapa[14][c] = PLATAFORMA; // intro ao salto
    for (int c = 44; c <= 49; c++) mapa[14][c] = PLATAFORMA; // respiro mid-fase
    for (int c = 70; c <= 75; c++) mapa[14][c] = PLATAFORMA; // apos vale central
    for (int c = 85; c <= 90; c++) mapa[13][c] = PLATAFORMA; // plataforma mais alta

    // Moedas guiando o jogador
    for (int c =  3; c <=  7; c++) mapa[16][c] = MOEDA;   // arco inicial no chao
    for (int c =  9; c <= 11; c++) mapa[13][c] = MOEDA;   // acima da plataforma inicial
    for (int c = 41; c <= 46; c++) mapa[16][c] = MOEDA;   // guia no espaco de respiro
    for (int c = 55; c <= 59; c++) mapa[14][c] = MOEDA;   // sobre a elevacao maxima
    for (int c = 71; c <= 74; c++) mapa[13][c] = MOEDA;   // acima da plataforma central
    for (int c = 86; c <= 89; c++) mapa[12][c] = MOEDA;   // sobre a plataforma alta
    for (int c = 109; c <= 113; c++) mapa[16][c] = MOEDA; // perto do fim

    // Blocos de interrogacao (?) — ensinam o jogador e recompensam saltos
    mapa[13][ 5] = BLOCO_INTERROGACAO;  // intro: primeiro bloco facil de achar
    mapa[13][31] = BLOCO_INTERROGACAO;  // grupo de 3 antes do primeiro vale
    mapa[13][32] = BLOCO_INTERROGACAO;
    mapa[13][33] = BLOCO_INTERROGACAO;
    mapa[11][58] = BLOCO_INTERROGACAO;  // acima da elevacao maxima (row 15)
    mapa[13][83] = BLOCO_INTERROGACAO;  // pos segundo vale, espaco de respiro
    mapa[13][112] = BLOCO_INTERROGACAO; // grupo de 2 perto do fim
    mapa[13][113] = BLOCO_INTERROGACAO;
}

static void preencher_fase2(int mapa[LINHAS][COLUNAS]) {
    //chao com uma lacuna
    preencher_chao(mapa, 0, 34, 21);
    preencher_chao(mapa, 40, COLUNAS - 1, 21);

    //plataformas intermediarias
    for (int c =  3; c <=  8; c++) mapa[16][c] = PLATAFORMA;
    for (int c = 11; c <= 16; c++) mapa[13][c] = PLATAFORMA;
    for (int c = 20; c <= 24; c++) mapa[17][c] = PLATAFORMA;
    for (int c = 27; c <= 33; c++) mapa[12][c] = PLATAFORMA;
    for (int c = 38; c <= 43; c++) mapa[15][c] = PLATAFORMA;
    for (int c = 46; c <= 51; c++) mapa[11][c] = PLATAFORMA;
    for (int c = 55; c <= 60; c++) mapa[14][c] = PLATAFORMA;
    for (int c = 63; c <= 68; c++) mapa[17][c] = PLATAFORMA;
    for (int c = 72; c <= 78; c++) mapa[13][c] = PLATAFORMA;

    //moedas
    for (int c =  4; c <=  7; c++) mapa[15][c] = MOEDA;
    for (int c = 12; c <= 15; c++) mapa[12][c] = MOEDA;
    for (int c = 28; c <= 32; c++) mapa[11][c] = MOEDA;
    for (int c = 39; c <= 42; c++) mapa[14][c] = MOEDA;
    for (int c = 47; c <= 50; c++) mapa[10][c] = MOEDA;
    for (int c = 56; c <= 59; c++) mapa[13][c] = MOEDA;
    for (int c = 73; c <= 77; c++) mapa[12][c] = MOEDA;
}

static void preencher_fase3(int mapa[LINHAS][COLUNAS]) {
    //chao com multiplos buracos
    preencher_chao(mapa, 0, 13, 21);
    preencher_chao(mapa, 19, 33, 21);
    preencher_chao(mapa, 41, 53, 21);
    preencher_chao(mapa, 62, COLUNAS - 1, 21);

    //plataformas complexas
    for (int c =  2; c <=  6; c++) mapa[17][c] = PLATAFORMA;
    for (int c =  9; c <= 13; c++) mapa[14][c] = PLATAFORMA;
    for (int c = 16; c <= 20; c++) mapa[11][c] = PLATAFORMA;
    for (int c = 23; c <= 27; c++) mapa[15][c] = PLATAFORMA;
    for (int c = 29; c <= 33; c++) mapa[12][c] = PLATAFORMA;
    for (int c = 36; c <= 41; c++) mapa[ 9][c] = PLATAFORMA;
    for (int c = 43; c <= 47; c++) mapa[13][c] = PLATAFORMA;
    for (int c = 49; c <= 53; c++) mapa[16][c] = PLATAFORMA;
    for (int c = 56; c <= 61; c++) mapa[10][c] = PLATAFORMA;
    for (int c = 63; c <= 67; c++) mapa[14][c] = PLATAFORMA;
    //arena do boss
    for (int c = 70; c <= 79; c++) mapa[20][c] = PLATAFORMA;
    for (int c = 70; c <= 79; c++) mapa[21][c] = PLATAFORMA;

    //moedas
    for (int c =  3; c <=  5; c++) mapa[16][c] = MOEDA;
    for (int c = 10; c <= 12; c++) mapa[13][c] = MOEDA;
    for (int c = 17; c <= 19; c++) mapa[10][c] = MOEDA;
    for (int c = 24; c <= 26; c++) mapa[14][c] = MOEDA;
    for (int c = 37; c <= 40; c++) mapa[ 8][c] = MOEDA;
    for (int c = 57; c <= 60; c++) mapa[ 9][c] = MOEDA;
    for (int c = 64; c <= 66; c++) mapa[13][c] = MOEDA;
}

//CarregarFase

void CarregarFase(Fase *f, int n) {
    memset(f->mapa, VAZIO, sizeof(f->mapa));
    f->numero  = n;
    f->cameraX = 0.0f;

    switch (n) {
        case 1:
            f->corFundo = (Color){  92, 148, 252, 255 }; //azul claro
            preencher_fase1(f->mapa);
            break;
        case 2:
            f->corFundo = (Color){ 255, 140,  60, 255 }; //laranja
            preencher_fase2(f->mapa);
            break;
        case 3:
            f->corFundo = (Color){  30,  10,  60, 255 }; //roxo escuro
            preencher_fase3(f->mapa);
            break;
    }
}

// Nuvens decorativas

typedef struct { float x, y, escala; } NuvemDeco;

static void desenhar_nuvem_deco(float wx, float wy, float escala, float cameraX) {
    float zoom = CAMERA_ZOOM;
    // parallax: nuvens se movem a 50% da camera para dar sensacao de profundidade
    float sx = (wx - cameraX * 0.5f) * zoom;
    float sy = (wy - CAMERA_Y_OFFSET) * zoom;

    float r  = 22.0f * escala * zoom;
    float r2 = 16.0f * escala * zoom;
    float r3 = 13.0f * escala * zoom;

    // culling simples
    if (sx + r + r3 < 0 || sx - r - r2 > GetScreenWidth()) return;
    if (sy + r < 0 || sy - r > GetScreenHeight()) return;

    Color cor  = (Color){ 255, 255, 255, 220 };
    Color somb = (Color){ 195, 210, 235, 170 };

    // base retangular para fechar os gaps entre as bolhas
    DrawRectangle((int)(sx - r2 * 0.7f), (int)(sy),
                  (int)(r2 * 0.7f + r + r3 * 0.7f), (int)(r * 0.9f), cor);
    // tres puffs
    DrawCircleV((Vector2){ sx,               sy              }, r,  cor);
    DrawCircleV((Vector2){ sx - r * 0.73f,   sy + r * 0.32f }, r2, cor);
    DrawCircleV((Vector2){ sx + r * 0.76f,   sy + r * 0.26f }, r3, cor);
    // sombra suave na base
    DrawRectangle((int)(sx - r2 * 0.7f), (int)(sy + r * 0.82f),
                  (int)(r2 * 0.7f + r + r3 * 0.7f), (int)(r * 0.22f), somb);
}

static void desenhar_nuvens(int faseNum, float cameraX) {
    static const NuvemDeco n1[] = {
        {  180,  60, 1.00f }, {  640,  38, 1.20f }, { 1090,  72, 0.85f },
        { 1570,  46, 1.10f }, { 2060,  66, 0.95f }, { 2510,  36, 1.15f },
        { 2960,  60, 1.00f }, { 3460,  78, 0.85f }, { 3960,  42, 1.05f },
    };
    static const NuvemDeco n2[] = {
        {  300,  52, 0.90f }, {  920,  34, 1.10f }, { 1620,  64, 0.80f },
        { 2240,  40, 1.00f }, { 3020,  68, 1.20f },
    };
    static const NuvemDeco n3[] = {
        {  420,  48, 0.70f }, { 1230,  30, 0.90f },
        { 2020,  54, 0.80f }, { 2830,  36, 1.00f },
    };

    const NuvemDeco *nuvens = NULL;
    int n = 0;
    if      (faseNum == 1) { nuvens = n1; n = 9; }
    else if (faseNum == 2) { nuvens = n2; n = 5; }
    else if (faseNum == 3) { nuvens = n3; n = 4; }

    for (int i = 0; i < n; i++)
        desenhar_nuvem_deco(nuvens[i].x, nuvens[i].y, nuvens[i].escala, cameraX);
}

//DesenharFase

void DesenharFase(Fase *f, Texture2D texBloco) {
    ClearBackground(f->corFundo);
    desenhar_nuvens(f->numero, f->cameraX);

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            int tipo = f->mapa[i][j];
            if (tipo == VAZIO || tipo == MOEDA) continue;

            float zoom = CAMERA_ZOOM;
            int screenX = (int)((j * TILE - (int)f->cameraX) * zoom);
            int screenY = (int)(((i * TILE) - CAMERA_Y_OFFSET) * zoom);
            int tileSize = (int)(TILE * zoom);

            if (screenX + tileSize < 0 || screenX > GetScreenWidth()) continue;
            if (screenY + tileSize < 0 || screenY > GetScreenHeight()) continue;

            int x = screenX;
            int y = screenY;

            if (tipo == PLATAFORMA) {
                int topo = (i == 0 || f->mapa[i - 1][j] != PLATAFORMA);
                if (topo) {
                    Color tijoloBase   = (Color){ 150, 150, 150, 255 };
                    Color tijoloBorda  = (Color){  90,  90,  90, 255 };
                    Color tijoloLuz    = (Color){ 175, 175, 175, 255 };
                    Color tijoloSombra = (Color){ 110, 110, 110, 255 };

                    DrawRectangle(x, y, tileSize, tileSize, tijoloBase);
                    DrawRectangleLines(x, y, tileSize, tileSize, tijoloBorda);
                    DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, tijoloLuz);
                    DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, tijoloLuz);
                    DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                    DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                    DrawLine(x + 1, y + tileSize / 2, x + tileSize - 2, y + tileSize / 2, tijoloSombra);
                } else {
                    Color terra      = (Color){ 120,  95,  60, 255 };
                    Color terraBorda = (Color){  80,  65,  45, 255 };
                    Color terraLuz   = (Color){ 145, 115,  75, 255 };
                    Color terraSombra = (Color){ 90,  70,  50, 255 };

                    DrawRectangle(x, y, tileSize, tileSize, terra);
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
            } else if (tipo == BLOCO_INTERROGACAO) {
                float bSize = (float)TILE * zoom; // tamanho exato sem truncamento
                if (texBloco.id > 0) {
                    // a imagem tem ~20% de margem branca em cada lado;
                    // o src recorta apenas a area do bloco (~60% central)
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

                    DrawRectangle(x, y, tileSize, tileSize, amarelo);
                    DrawRectangleLines(x, y, tileSize, tileSize, amareloBorda);
                    DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, amareloLuz);
                    DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, amareloLuz);
                    DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, amareloSombra);
                    DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, amareloSombra);

                    int fSize = tileSize * 2 / 3;
                    int tw = MeasureText("?", fSize);
                    DrawText("?", x + (tileSize - tw) / 2, y + (tileSize - fSize) / 2, fSize, amareloBorda);
                }

            } else if (tipo == BLOCO_USADO) {
                // igual ao tijolo de topo do terreno
                Color tijoloBase   = (Color){ 150, 150, 150, 255 };
                Color tijoloBorda  = (Color){  90,  90,  90, 255 };
                Color tijoloLuz    = (Color){ 175, 175, 175, 255 };
                Color tijoloSombra = (Color){ 110, 110, 110, 255 };

                DrawRectangle(x, y, tileSize, tileSize, tijoloBase);
                DrawRectangleLines(x, y, tileSize, tileSize, tijoloBorda);
                DrawLine(x + 1, y + 1, x + tileSize - 2, y + 1, tijoloLuz);
                DrawLine(x + 1, y + 1, x + 1, y + tileSize - 2, tijoloLuz);
                DrawLine(x + 1, y + tileSize - 2, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                DrawLine(x + tileSize - 2, y + 1, x + tileSize - 2, y + tileSize - 2, tijoloSombra);
                DrawLine(x + 1, y + tileSize / 2, x + tileSize - 2, y + tileSize / 2, tijoloSombra);
            }
        }
    }
}

//VerificarColisao

int VerificarColisao(Rectangle a, Rectangle b) {
    if (a.x + a.width  <= b.x) return 0;
    if (b.x + b.width  <= a.x) return 0;
    if (a.y + a.height <= b.y) return 0;
    if (b.y + b.height <= a.y) return 0;
    return 1;
}
