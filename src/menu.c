#include "menu.h"
#include <stdio.h>

#define LARGURA 1280
#define ALTURA   720

/* posicoes do menu */
#define MENU_X       530
#define MENU_Y_INICIO 340
#define MENU_ESPACO   52
#define CURSOR_X     492

static const char *LABELS[OPCAO_TOTAL] = {
    "CESAR A  A .. EMPTY",
    "CESAR B  B .. EMPTY",
    "CESAR C  C .. EMPTY",
    "COMECAR JOGO",
    "PLACAR",
    "OPCOES",
};

/* ------------------------------------------------------------------ */
/* Helpers visuais                                                       */
/* ------------------------------------------------------------------ */

static void desenhar_texto_contorno(const char *txt, int x, int y,
                                    int tamanho, Color preenchimento, Color contorno) {
    DrawText(txt, x - 2, y,     tamanho, contorno);
    DrawText(txt, x + 2, y,     tamanho, contorno);
    DrawText(txt, x, y - 2,     tamanho, contorno);
    DrawText(txt, x, y + 2,     tamanho, contorno);
    DrawText(txt, x, y,         tamanho, preenchimento);
}

static void desenhar_fundo_programatico(void) {
    /* parede superior */
    DrawRectangle(0, 0, LARGURA, 480, (Color){170, 155, 125, 255});

    /* textura de tijolos na parede */
    for (int l = 0; l < 15; l++) {
        for (int c = 0; c < 22; c++) {
            int ox = (l % 2 == 0) ? 0 : 30;
            DrawRectangleLines(c * 60 + ox, l * 32, 58, 30,
                               (Color){140, 125, 95, 80});
        }
    }

    /* chao */
    DrawRectangle(0, 480, LARGURA, 240, (Color){90, 50, 35, 255});

    /* tijolos do chao */
    for (int l = 0; l < 8; l++) {
        for (int c = 0; c < 22; c++) {
            int ox = (l % 2 == 0) ? 0 : 34;
            DrawRectangleLines(c * 68 + ox, 480 + l * 30, 66, 28,
                               (Color){60, 30, 15, 160});
        }
    }

    /* escada esquerda */
    for (int d = 0; d < 8; d++) {
        DrawRectangle(d * 28, 380 + d * 12, 220 - d * 28, 12,
                      (Color){120, 75, 45, 255});
        DrawRectangle(d * 28, 380 + d * 12, 220 - d * 28, 3,
                      (Color){160, 110, 70, 255});
    }
    /* corrimao esquerdo */
    DrawRectangle(0, 370, 8, 110, (Color){100, 60, 30, 255});

    /* escada direita (espelhada) */
    for (int d = 0; d < 8; d++) {
        int x = LARGURA - 220 + d * 28;
        DrawRectangle(x, 380 + d * 12, 220 - d * 28, 12,
                      (Color){120, 75, 45, 255});
        DrawRectangle(x, 380 + d * 12, 220 - d * 28, 3,
                      (Color){160, 110, 70, 255});
    }
    /* corrimao direito */
    DrawRectangle(LARGURA - 8, 370, 8, 110, (Color){100, 60, 30, 255});

    /* canos horizontais no topo */
    DrawRectangle(0,    28, LARGURA, 14, (Color){120, 120, 130, 255});
    DrawRectangle(0,    48, LARGURA,  6, (Color){ 90,  90, 100, 255});
    DrawRectangle(0,   100, LARGURA, 10, (Color){120, 120, 130, 255});

    /* canos verticais */
    DrawRectangle(180, 0,  18, 480, (Color){115, 115, 125, 255});
    DrawRectangle(184, 0,   4, 480, (Color){155, 155, 165, 255});
    DrawRectangle(LARGURA - 198, 0, 18, 480, (Color){115, 115, 125, 255});
    DrawRectangle(LARGURA - 202, 0,  4, 480, (Color){155, 155, 165, 255});

    /* janelas/portas ao fundo */
    DrawRectangle(350, 80, 160, 230, (Color){ 50,  50,  60, 200});
    DrawRectangle(354, 84, 152, 222, (Color){ 30,  30,  40, 255});
    DrawText("11",  410, 100, 36, (Color){180, 50, 50, 255});

    DrawRectangle(780, 80, 160, 230, (Color){ 50,  50,  60, 200});
    DrawRectangle(784, 84, 152, 222, (Color){ 30,  30,  40, 255});

    /* letreiros nas portas */
    DrawText("SALHS", 360, 72, 18, (Color){160, 120, 80, 255});
    DrawText("SALAS", 786, 72, 18, (Color){160, 120, 80, 255});

    /* sombra de gradiente entre parede e chao */
    for (int i = 0; i < 20; i++) {
        DrawRectangle(0, 470 + i, LARGURA, 1,
                      (Color){0, 0, 0, (unsigned char)(i * 8)});
    }
}

/* ------------------------------------------------------------------ */
/* IniciarMenu                                                          */
/* ------------------------------------------------------------------ */

void IniciarMenu(Menu *m) {
    m->selecionado = OPCAO_COMECAR;
    m->tempoCursor = 0.0f;
    m->temFundo    = 0;

    if (FileExists("assets/sprites/menu_bg.png")) {
        m->fundo    = LoadTexture("assets/sprites/menu_bg.png");
        m->temFundo = 1;
    }
}

void LiberarMenu(Menu *m) {
    if (m->temFundo) UnloadTexture(m->fundo);
}

/* ------------------------------------------------------------------ */
/* AtualizarMenu — retorna opcao confirmada ou -1                       */
/* ------------------------------------------------------------------ */

int AtualizarMenu(Menu *m) {
    m->tempoCursor += GetFrameTime();

    if (IsKeyPressed(KEY_UP)) {
        m->selecionado--;
        if (m->selecionado < 0) m->selecionado = OPCAO_TOTAL - 1;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        m->selecionado++;
        if (m->selecionado >= OPCAO_TOTAL) m->selecionado = 0;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        return m->selecionado;
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* DesenharMenu                                                          */
/* ------------------------------------------------------------------ */

void DesenharMenu(Menu *m, Placar *p) {
    /* --- fundo --- */
    if (m->temFundo) {
        DrawTexturePro(m->fundo,
            (Rectangle){0, 0, (float)m->fundo.width, (float)m->fundo.height},
            (Rectangle){0, 0, LARGURA, ALTURA},
            (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        desenhar_fundo_programatico();
    }

    /* --- titulo --- */
    const char *super = "SUPER";
    int lSuper = MeasureText(super, 68);
    desenhar_texto_contorno(super,
        LARGURA / 2 - lSuper / 2, 110,
        68, (Color){255, 165, 0, 255}, (Color){80, 40, 0, 255});

    const char *titulo = "CESAR WORLD";
    int lTitulo = MeasureText(titulo, 88);
    desenhar_texto_contorno(titulo,
        LARGURA / 2 - lTitulo / 2, 178,
        88, (Color){220, 220, 220, 255}, (Color){20, 20, 20, 255});

    /* TM superscrito */
    DrawText("TM", LARGURA / 2 + lTitulo / 2 + 4, 184, 22,
             (Color){200, 200, 200, 255});

    /* --- painel semi-transparente atras do menu --- */
    DrawRectangle(CURSOR_X - 20, MENU_Y_INICIO - 14,
                  520, OPCAO_TOTAL * MENU_ESPACO + 28,
                  (Color){0, 0, 0, 110});

    /* --- itens do menu --- */
    for (int i = 0; i < OPCAO_TOTAL; i++) {
        int y = MENU_Y_INICIO + i * MENU_ESPACO;
        int selecionado = (i == m->selecionado);

        Color cor = selecionado ? WHITE : (Color){180, 180, 180, 255};
        int tamanho = selecionado ? 30 : 26;

        DrawText(LABELS[i], MENU_X, y, tamanho, cor);

        /* cursor piscante no item selecionado */
        if (selecionado) {
            int visivel = (int)(m->tempoCursor * 4) % 2;
            if (visivel) DrawText(">", CURSOR_X, y, 30, YELLOW);
        }
    }

    /* --- top scores (so aparece se PLACAR selecionado) --- */
    if (m->selecionado == OPCAO_PLACAR && p != NULL) {
        int px = LARGURA / 2 - 80;
        int py = MENU_Y_INICIO + OPCAO_TOTAL * MENU_ESPACO + 20;
        DrawText("TOP 5:", px, py, 22, YELLOW);
        for (int i = 0; i < TOP_SCORES; i++) {
            DrawText(TextFormat("%d. %07d", i + 1, p->topScores[i]),
                     px, py + 28 + i * 26, 20, WHITE);
        }
    }

    /* --- rodape --- */
    const char *rodape = "(c) 2026 CESAR ESTUDIOS";
    int lRodape = MeasureText(rodape, 20);
    DrawText(rodape, LARGURA / 2 - lRodape / 2, ALTURA - 36, 20,
             (Color){140, 140, 140, 255});

    /* icone de engrenagem simples */
    DrawCircleLines(LARGURA / 2 - lRodape / 2 - 32, ALTURA - 22, 14,
                    (Color){140, 140, 140, 255});
    DrawText("C", LARGURA / 2 - lRodape / 2 - 38, ALTURA - 30, 18,
             (Color){140, 140, 140, 255});
}
