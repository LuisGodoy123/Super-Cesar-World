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

/* contorno grosso (3px) para o titulo principal */
static void desenhar_texto_contorno_grosso(const char *txt, int x, int y,
                                           int tamanho, Color preenchimento, Color contorno) {
    for (int dx = -3; dx <= 3; dx++)
        for (int dy = -3; dy <= 3; dy++)
            if (dx != 0 || dy != 0)
                DrawText(txt, x + dx, y + dy, tamanho, contorno);
    DrawText(txt, x, y, tamanho, preenchimento);
}

/* desenha palavra letra a letra com fonte padrao, cada uma com sua cor */
static void desenhar_palavra_colorida(const char *palavra, Color *cores,
                                      int x, int y, int tamanho, Color contorno) {
    char buf[2] = {0, 0};
    int curX = x;
    for (int i = 0; palavra[i] != '\0'; i++) {
        buf[0] = palavra[i];
        desenhar_texto_contorno_grosso(buf, curX, y, tamanho, cores[i], contorno);
        curX += MeasureText(buf, tamanho) - 2;
    }
}

/* desenha palavra letra a letra com fonte customizada, cada uma com sua cor */
static void desenhar_palavra_colorida_fonte(const char *palavra, Color *cores,
                                            Font fonte, int x, int y,
                                            float tamanho, Color contorno) {
    char buf[2] = {0, 0};
    int curX = x;
    for (int i = 0; palavra[i] != '\0'; i++) {
        buf[0] = palavra[i];
        Vector2 tam = MeasureTextEx(fonte, buf, tamanho, 0);
        DrawTextEx(fonte, buf, (Vector2){curX, y}, tamanho, 0, cores[i]);
        curX += (int)tam.x - 3;
    }
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
    m->temFonte    = 0;

    if (FileExists("assets/sprites/menu_bg.png")) {
        m->fundo    = LoadTexture("assets/sprites/menu_bg.png");
        m->temFundo = 1;
    }

    /* tenta carregar fonte local sem antialiasing (pixel perfeito) */
    const char *caminhos_fonte[] = {
        "assets/fontes/SuperMarioWorld.ttf",
        "assets/fontes/PressStart2P-Regular.ttf",
        "assets/fontes/SuperMario256.ttf",
    };
    int n = sizeof(caminhos_fonte) / sizeof(caminhos_fonte[0]);
    for (int i = 0; i < n; i++) {
        if (FileExists(caminhos_fonte[i])) {
            m->fonte = LoadFontEx(caminhos_fonte[i], 128, NULL, 0);
            /* desativa antialiasing — filtro de ponto mantem aspecto pixel */
            SetTextureFilter(m->fonte.texture, TEXTURE_FILTER_POINT);
            m->temFonte = 1;
            break;
        }
    }
}

void LiberarMenu(Menu *m) {
    if (m->temFundo) UnloadTexture(m->fundo);
    if (m->temFonte) UnloadFont(m->fonte);
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

    /* 4 cores que se revezam letra a letra em todo o titulo */
    static const Color PALETA[4] = {
        {255, 140,   0, 255},  /* 0 laranja      */
        { 55,  55,  60, 255},  /* 1 cinza escuro */
        {175, 130,   0, 255},  /* 2 amarelo escuro */
        {255, 255, 255, 255},  /* 3 branco       */
    };

    /* SUPER = letras 0-4  → ciclo começa em 0 */
    Color cor_super[] = {
        PALETA[0],  /* S */
        PALETA[1],  /* U */
        PALETA[2],  /* P */
        PALETA[3],  /* E */
        PALETA[0],  /* R */
    };

    /* CESAR = letras 5-9  → ciclo continua em 1 */
    Color cor_cesar[] = {
        PALETA[1],  /* C */
        PALETA[2],  /* E */
        PALETA[3],  /* S */
        PALETA[0],  /* A */
        PALETA[1],  /* R */
    };

    /* WORLD = letras 10-14 → ciclo continua em 2 */
    Color cor_world[] = {
        PALETA[2],  /* W */
        PALETA[3],  /* O */
        PALETA[0],  /* R */
        PALETA[1],  /* L */
        PALETA[2],  /* D */
    };

    /* calcula larguras para centralizar */
    int lSuper = MeasureText("SUPER",      72);
    int lCesar = MeasureText("CESAR",      96);
    int lWorld = MeasureText(" WORLD",     96);
    int lTitulo = lCesar + MeasureText(" ", 96) + lWorld;

    int xSuper  = LARGURA / 2 - lSuper  / 2 + 30;
    int xCesar  = LARGURA / 2 - lTitulo / 2;
    int xWorld  = xCesar + lCesar + MeasureText(" ", 96) - 10;

    if (m->temFonte) {
        float tSuper = 58.0f;
        float tTit   = 80.0f;
        int   gap    = 10;   /* espaco fixo entre CESAR e WORLD */

        Vector2 szSuper = MeasureTextEx(m->fonte, "SUPER", tSuper, 0);
        Vector2 szCesar = MeasureTextEx(m->fonte, "CESAR", tTit,   0);
        Vector2 szWorld = MeasureTextEx(m->fonte, "WORLD", tTit,   0);
        int totalTit = (int)(szCesar.x + gap + szWorld.x);

        int fxSuper = LARGURA / 2 - (int)szSuper.x / 2 + 20;
        int fxCesar = LARGURA / 2 - totalTit / 2;
        int fxWorld = fxCesar + (int)szCesar.x + gap;

        desenhar_palavra_colorida_fonte("SUPER", cor_super, m->fonte,
                                        fxSuper, 92, tSuper, (Color){80, 35, 0, 255});
        desenhar_palavra_colorida_fonte("CESAR", cor_cesar, m->fonte,
                                        fxCesar, 155, tTit, WHITE);
        desenhar_palavra_colorida_fonte("WORLD", cor_world, m->fonte,
                                        fxWorld, 155, tTit, WHITE);

        Vector2 szTM = MeasureTextEx(m->fonte, "WORLD", tTit, 0);
        DrawTextEx(m->fonte, "TM",
                   (Vector2){fxWorld + (int)szTM.x + 4, 160}, 22, 0, WHITE);
    } else {
        /* fallback: fonte padrao */
        desenhar_palavra_colorida("SUPER",  cor_super, xSuper, 95,  72, (Color){80, 35, 0, 255});
        desenhar_palavra_colorida("CESAR",  cor_cesar, xCesar, 168, 96, WHITE);
        desenhar_palavra_colorida("WORLD",  cor_world, xWorld, 168, 96, WHITE);
        DrawText("TM", xWorld + MeasureText("WORLD", 96) + 4, 174, 24, WHITE);
    }


    /* --- itens do menu --- */
    float tItem   = m->temFonte ? 18.0f : 28.0f;
    float tCursor = m->temFonte ? 18.0f : 30.0f;
    int   espaco  = m->temFonte ? 48    : MENU_ESPACO;

    for (int i = 0; i < OPCAO_TOTAL; i++) {
        int y = MENU_Y_INICIO + i * espaco;
        int selecionado = (i == m->selecionado);

        if (m->temFonte) {
            DrawTextEx(m->fonte, LABELS[i],
                       (Vector2){MENU_X, y}, tItem, 1, WHITE);

            /* cursor piscante */
            if (selecionado) {
                int visivel = (int)(m->tempoCursor * 4) % 2;
                if (visivel)
                    DrawTextEx(m->fonte, ">",
                               (Vector2){CURSOR_X, y}, tCursor, 1, YELLOW);
            }
        } else {
            DrawText(LABELS[i], MENU_X, y, (int)tItem, WHITE);
            if (selecionado) {
                int visivel = (int)(m->tempoCursor * 4) % 2;
                if (visivel)
                    DrawText(">", CURSOR_X, y, (int)tCursor, YELLOW);
            }
        }
    }

    /* --- top scores (so aparece se PLACAR selecionado) --- */
    if (m->selecionado == OPCAO_PLACAR && p != NULL) {
        int px = LARGURA / 2 - 120;
        int py = MENU_Y_INICIO + OPCAO_TOTAL * espaco + 16;
        if (m->temFonte) {
            DrawTextEx(m->fonte, "TOP 5:", (Vector2){px, py}, 16, 1, YELLOW);
            for (int i = 0; i < TOP_SCORES; i++)
                DrawTextEx(m->fonte,
                           TextFormat("%d. %07d", i + 1, p->topScores[i]),
                           (Vector2){px, py + 24 + i * 24}, 14, 1, WHITE);
        } else {
            DrawText("TOP 5:", px, py, 22, YELLOW);
            for (int i = 0; i < TOP_SCORES; i++)
                DrawText(TextFormat("%d. %07d", i + 1, p->topScores[i]),
                         px, py + 28 + i * 26, 20, WHITE);
        }
    }

    /* --- rodape --- */
    const char *rodape = "(c) 2026 CESAR ESTUDIOS";
    if (m->temFonte) {
        Vector2 szRodape = MeasureTextEx(m->fonte, rodape, 12, 1);
        DrawTextEx(m->fonte, rodape,
                   (Vector2){LARGURA / 2 - szRodape.x / 2, ALTURA - 34},
                   12, 1, (Color){140, 140, 140, 255});
    } else {
        int lRodape = MeasureText(rodape, 20);
        DrawText(rodape, LARGURA / 2 - lRodape / 2, ALTURA - 36, 20,
                 (Color){140, 140, 140, 255});
    }

    /* icone de engrenagem simples */
    int lRodapeGear = MeasureText(rodape, 20);
    DrawCircleLines(LARGURA / 2 - lRodapeGear / 2 - 32, ALTURA - 22, 14,
                    (Color){140, 140, 140, 255});
    DrawText("C", LARGURA / 2 - lRodapeGear / 2 - 38, ALTURA - 30, 18,
             (Color){140, 140, 140, 255});
}
