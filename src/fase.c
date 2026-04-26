#include "fase.h"
#include <string.h>

// Mapas das fases

static void preencher_fase1(int mapa[LINHAS][COLUNAS]) {
    //chao continuo
    for (int c = 0; c < COLUNAS; c++)
        mapa[21][c] = PLATAFORMA;

    //plataformas
    for (int c =  4; c <=  8; c++) mapa[17][c] = PLATAFORMA;
    for (int c = 12; c <= 17; c++) mapa[15][c] = PLATAFORMA;
    for (int c = 22; c <= 27; c++) mapa[17][c] = PLATAFORMA;
    for (int c = 31; c <= 37; c++) mapa[14][c] = PLATAFORMA;
    for (int c = 41; c <= 47; c++) mapa[16][c] = PLATAFORMA;
    for (int c = 51; c <= 57; c++) mapa[18][c] = PLATAFORMA;
    for (int c = 62; c <= 67; c++) mapa[15][c] = PLATAFORMA;
    for (int c = 72; c <= 78; c++) mapa[17][c] = PLATAFORMA;

    //moedas (linha acima de cada plataforma)
    for (int c =  5; c <=  7; c++) mapa[16][c] = MOEDA;
    for (int c = 13; c <= 16; c++) mapa[14][c] = MOEDA;
    for (int c = 32; c <= 36; c++) mapa[13][c] = MOEDA;
    for (int c = 42; c <= 46; c++) mapa[15][c] = MOEDA;
    for (int c = 63; c <= 66; c++) mapa[14][c] = MOEDA;
}

static void preencher_fase2(int mapa[LINHAS][COLUNAS]) {
    //chao com uma lacuna
    for (int c = 0; c < COLUNAS; c++)
        mapa[21][c] = PLATAFORMA;
    for (int c = 35; c <= 39; c++)
        mapa[21][c] = VAZIO;

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
    for (int c = 0; c < COLUNAS; c++)
        mapa[21][c] = PLATAFORMA;
    for (int c = 14; c <= 18; c++) mapa[21][c] = VAZIO;
    for (int c = 34; c <= 40; c++) mapa[21][c] = VAZIO;
    for (int c = 54; c <= 61; c++) mapa[21][c] = VAZIO;

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

//DesenharFase

void DesenharFase(Fase *f) {
    ClearBackground(f->corFundo);

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (f->mapa[i][j] != PLATAFORMA) continue;

            int screenX = j * TILE - (int)f->cameraX;

            //ignora tiles fora da tela
            if (screenX + TILE < 0 || screenX > GetScreenWidth()) continue;

            DrawRectangle(screenX, i * TILE, TILE, TILE, BROWN);
            DrawRectangleLines(screenX, i * TILE, TILE, TILE, DARKBROWN);
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
