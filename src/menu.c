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

/* logo CESAR real: disco plano + oval do corpo + letra C como anel parcial */
static void desenhar_logo_cesar(int cx, int cy, int r, Color cFundo) {
    Color cD = {16, 12, 8, 255};

    /* 1. Disco plano na base — oval muito larga e achatada */
    DrawEllipse(cx, cy + r * 55 / 58, r,          r * 16 / 58, cD);

    /* 2. Corpo oval principal (externo) */
    DrawEllipse(cx, cy + r * 10 / 58, r * 54 / 58, r * 44 / 58, cD);

    /* 3. Buraco interno do corpo (cor do fundo) */
    DrawEllipse(cx, cy + r * 14 / 58, r * 32 / 58, r * 26 / 58, cFundo);

    /* 4. Letra "C" — anel parcial (abertura para a direita ~70 graus) */
    float outerC = (float)(r * 28 / 58);
    float innerC = (float)(r * 16 / 58);
    int   ccx    = cx;
    int   ccy    = cy - r * 16 / 58;
    DrawRing((Vector2){ccx, ccy}, innerC, outerC, 45.0f, 315.0f, 40, cD);

    /* 5. Sombra/espessura do disco (borda inferior da base) */
    DrawEllipse(cx, cy + r * 60 / 58, r * 95 / 100, r * 12 / 58,
                (Color){10, 8, 5, 255});
}

/* preenche uma area com padrao de tijolos (running bond) */
static void desenhar_tijolos(int x0, int y0, int larg, int alt,
                              Color cTij, Color cMort) {
    int bW = 68, bH = 22, mJ = 3; /* largura, altura, junta */
    DrawRectangle(x0, y0, larg, alt, cMort);
    for (int row = 0; row * bH < alt + bH; row++) {
        int offset = (row % 2 == 0) ? 0 : bW / 2;
        for (int col = -1; col * bW < larg + bW; col++) {
            int bx = x0 + col * bW + offset;
            int by = y0 + row * bH;
            int bw = bW - mJ;
            int bh = bH - mJ;
            /* clip ao limite da parede */
            if (bx < x0) { bw -= (x0 - bx); bx = x0; }
            if (bx + bw > x0 + larg) bw = x0 + larg - bx;
            if (by + bh > y0 + alt)  bh = y0 + alt  - by;
            if (bw > 0 && bh > 0)
                DrawRectangle(bx, by, bw, bh, cTij);
        }
    }
}

/* folha de palmeira: base no tronco, ponta em (px,py) com largura w */
static void folha_palma(int tx, int ty, int px, int py, int w, Color c) {
    /* desenha como dois triangulos sobrepostos para dar volume */
    DrawTriangle((Vector2){tx - w, ty},
                 (Vector2){tx + w, ty},
                 (Vector2){px,     py}, c);
}

static void desenhar_fundo_programatico(void) {
    /* ── paleta de cores ── */
    Color cEsc    = { 14,  10,   6, 255}; /* escuro geral            */
    Color cCeu    = { 12,  14,  22, 255}; /* ceu quase preto         */
    Color cTijE   = {205,  70,  22, 255}; /* tijolo fachada esquerda */
    Color cMortE  = {155,  45,  12, 255}; /* junta esquerda          */
    Color cTijLat = {155,  50,  14, 255}; /* tijolo lateral (escuro) */
    Color cMortLat= {112,  32,   8, 255}; /* junta lateral           */
    Color cTijD   = {190,  62,  18, 255}; /* tijolo fachada direita  */
    Color cMortD  = {142,  40,  10, 255}; /* junta direita           */
    Color cGnd    = {100,  95,  86, 255}; /* calcada                 */

    /* ── layout geral ──
       lateral esq: x=0..68
       fachada esq: x=68..735   gap: x=735..758   fachada dir: x=758..1280
       ceu: y=0..55   chao: y=692..720                                    */
    int xLat = 68;
    int xGap = 735, gapW = 23;
    int xDir = xGap + gapW;
    int ySky = 55, yGnd = 692;

    /* ══════════════════════════════════════
       1. CEU
    ══════════════════════════════════════ */
    DrawRectangle(0, 0, LARGURA, ySky, cCeu);

    /* ══════════════════════════════════════
       2. PAREDES
    ══════════════════════════════════════ */
    /* parede lateral esquerda — face lateral do predio, mais escura */
    desenhar_tijolos(0,    ySky, xLat,            yGnd - ySky, cTijLat, cMortLat);
    /* linha de aresta entre lateral e fachada */
    DrawRectangle(xLat - 3, ySky, 5, yGnd - ySky, (Color){90, 26, 6, 255});

    /* fachada esquerda */
    desenhar_tijolos(xLat, ySky, xGap - xLat,    yGnd - ySky, cTijE, cMortE);

    /* gap entre predios */
    DrawRectangle(xGap, ySky, gapW, yGnd - ySky, cEsc);

    /* fachada direita */
    desenhar_tijolos(xDir, ySky, LARGURA - xDir, yGnd - ySky, cTijD, cMortD);

    /* ══════════════════════════════════════
       3. FACHADA ESQUERDA — detalhes
    ══════════════════════════════════════ */

    /* --- ENTRADA/PORTA DUPLA ---
       Ocupa o canto direito da fachada esq, de y=278 ate o chao    */
    int pX = 508, pY = 278, pW = xGap - pX, pH = yGnd - pY;
    /* abertura */
    DrawRectangle(pX, pY, pW, pH, (Color){10, 8, 6, 255});
    /* batentes e verga */
    DrawRectangle(pX,       pY, 12, pH, (Color){28, 22, 14, 255});
    DrawRectangle(pX+pW-12, pY, 12, pH, (Color){28, 22, 14, 255});
    DrawRectangle(pX,       pY, pW, 12, (Color){28, 22, 14, 255});
    /* duas folhas da porta */
    int fW = (pW - 26) / 2;
    DrawRectangle(pX + 13,       pY + 13, fW, pH - 13, (Color){20, 16, 10, 255});
    DrawRectangle(pX + 13 + fW,  pY + 13, fW, pH - 13, (Color){20, 16, 10, 255});
    /* fresta central */
    DrawRectangle(pX + 13 + fW - 1, pY + 13, 3, pH - 13, (Color){6, 4, 2, 255});
    /* paineis decorativos nas folhas */
    DrawRectangle(pX + 18,      pY + 20, fW - 10, 80, (Color){25, 20, 13, 255});
    DrawRectangle(pX + 18,      pY+115, fW - 10, 80, (Color){25, 20, 13, 255});
    DrawRectangle(pX+13+fW+5,   pY + 20, fW - 10, 80, (Color){25, 20, 13, 255});
    DrawRectangle(pX+13+fW+5,   pY+115, fW - 10, 80, (Color){25, 20, 13, 255});
    /* macanetas */
    int mkY = pY + pH / 2;
    DrawRectangle(pX + 13 + fW - 18, mkY, 18, 7, (Color){52, 42, 28, 255});
    DrawRectangle(pX + 13 + fW +  2, mkY, 18, 7, (Color){52, 42, 28, 255});

    /* --- LOGO CESAR ESQUERDO ---
       Centralizado na area livre (xLat ate pX)                      */
    int logoExC = (xLat + pX) / 2;
    desenhar_logo_cesar(logoExC, 188, 58, cTijE);

    /* --- TEXTO C.E.S.A.R. --- */
    {
        int tw = MeasureText("C.E.S.A.R.", 24);
        DrawText("C.E.S.A.R.", logoExC - tw / 2, 270, 24, cEsc);
    }

    /* --- PALMEIRA ---
       Tronco centrado em tx=210 (dentro da area livre)              */
    int tx = 210, ty = 368;
    int trunkBot = yGnd - 98;

    /* folhas: da mais externa/escura ate a central/clara
       cada folha: (base_x, base_y) -> (ponta_x, ponta_y), largura  */
    folha_palma(tx, ty, tx - 205, ty + 220, 15, (Color){18,  78, 24, 255});
    folha_palma(tx, ty, tx - 172, ty + 178, 14, (Color){22,  90, 28, 255});
    folha_palma(tx, ty, tx - 135, ty + 135, 13, (Color){26, 102, 32, 255});
    folha_palma(tx, ty, tx -  95, ty +  92, 12, (Color){30, 112, 38, 255});
    folha_palma(tx, ty, tx -  52, ty +  50, 11, (Color){34, 122, 42, 255});
    folha_palma(tx, ty, tx -  16, ty -  62, 10, (Color){40, 135, 48, 255});
    folha_palma(tx, ty, tx +   2, ty -  78,  9, (Color){44, 142, 52, 255});
    folha_palma(tx, ty, tx +  20, ty -  60, 10, (Color){40, 135, 48, 255});
    folha_palma(tx, ty, tx +  58, ty +  46, 11, (Color){34, 122, 42, 255});
    folha_palma(tx, ty, tx +  98, ty +  88, 12, (Color){30, 112, 38, 255});
    folha_palma(tx, ty, tx + 138, ty + 130, 13, (Color){26, 102, 32, 255});
    folha_palma(tx, ty, tx + 174, ty + 174, 14, (Color){22,  90, 28, 255});
    folha_palma(tx, ty, tx + 208, ty + 215, 15, (Color){18,  78, 24, 255});
    /* camada traseira para volume */
    folha_palma(tx, ty, tx -  82, ty +  22, 10, (Color){38, 128, 46, 255});
    folha_palma(tx, ty, tx +  86, ty +  20, 10, (Color){38, 128, 46, 255});

    /* tronco */
    int tH = trunkBot - ty;
    DrawRectangle(tx - 8, ty, 16, tH, (Color){ 78, 46, 13, 255});
    DrawRectangle(tx - 4, ty + 8, 7, tH, (Color){ 98, 60, 19, 255});
    for (int a = 0; a < 14; a++)
        DrawRectangle(tx - 8, ty + 16 + a * 17, 16, 3, (Color){58, 33,  9, 255});

    /* vaso */
    DrawRectangle(tx - 62, trunkBot,      124, 88, (Color){44, 50, 64, 255});
    DrawRectangle(tx - 70, trunkBot,      140, 15, (Color){56, 62, 76, 255}); /* borda */
    DrawRectangle(tx - 54, trunkBot + 78, 108, 10, (Color){35, 40, 52, 255}); /* base */
    /* sombra lateral no vaso */
    DrawRectangle(tx + 38, trunkBot + 4, 20, 80, (Color){22, 26, 36, 80});

    /* ══════════════════════════════════════
       4. FACHADA DIREITA — detalhes
    ══════════════════════════════════════ */

    /* --- LOGO CESAR DIREITO --- */
    int logoDxC = xDir + (LARGURA - xDir) / 3;
    desenhar_logo_cesar(logoDxC, 188, 58, cTijD);

    /* --- TEXTOS --- */
    {
        int tw1 = MeasureText("c.e.s.a.r.", 24);
        int tw2 = MeasureText("school", 24);
        DrawText("c.e.s.a.r.", logoDxC - tw1 / 2, 268, 24, cEsc);
        DrawText("school",     logoDxC - tw2 / 2, 302, 24, cEsc);
    }

    /* --- VENEZIANA ---
       Localizada no lado direito da fachada dir, area central/baixa  */
    int vX = xDir + (LARGURA - xDir) * 57 / 100;
    int vY = 395, vW = 272, vH = 262;
    /* moldura */
    DrawRectangle(vX - 10, vY - 10, vW + 20, vH + 20, cEsc);
    /* fundo escuro dentro da moldura */
    DrawRectangle(vX, vY, vW, vH, (Color){18, 13, 8, 255});
    /* laminas horizontais com efeito 3D (brilho no topo, sombra embaixo) */
    int nSlats = 14, slatH = vH / nSlats;
    for (int s = 0; s < nSlats; s++) {
        int ly = vY + s * slatH + 2;
        int sh = slatH - 3;
        DrawRectangle(vX + 3, ly,      vW - 6, sh,  (Color){36, 28, 18, 255}); /* corpo */
        DrawRectangle(vX + 3, ly,      vW - 6,  4,  (Color){50, 40, 26, 255}); /* brilho */
        DrawRectangle(vX + 3, ly + sh - 2, vW - 6, 2, (Color){10, 7,  4, 255}); /* sombra */
    }
    /* divisores verticais */
    DrawRectangle(vX + vW * 1 / 3, vY, 5, vH, cEsc);
    DrawRectangle(vX + vW * 2 / 3, vY, 5, vH, cEsc);

    /* ══════════════════════════════════════
       5. CALCADA E SOMBRA
    ══════════════════════════════════════ */
    DrawRectangle(0, yGnd, LARGURA, ALTURA - yGnd, cGnd);
    for (int c = 0; c <= 22; c++)
        DrawLine(c * 62, yGnd, c * 62, ALTURA, (Color){78, 74, 66, 255});
    DrawLine(0, yGnd + 18, LARGURA, yGnd + 18, (Color){78, 74, 66, 255});

    /* sombra gradiente base dos predios */
    for (int i = 0; i < 22; i++)
        DrawRectangle(0, yGnd - 22 + i, LARGURA, 1,
                      (Color){0, 0, 0, (unsigned char)(i * 8)});
}

/* moldura de madeira ao redor da tela (estilo retro) */
static void desenhar_borda_madeira(void) {
    const int t = 36;
    Color cEscuro = (Color){58, 31, 10, 255};
    Color cMedio  = (Color){102, 60, 23, 255};
    Color cClaro  = (Color){160, 106, 56, 255};
    Color cSombra = (Color){36, 18, 6, 255};
    Color cPrego  = (Color){196, 178, 122, 255};

    /* faixa externa */
    DrawRectangle(0, 0, LARGURA, t, cEscuro);
    DrawRectangle(0, ALTURA - t, LARGURA, t, cEscuro);
    DrawRectangle(0, t, t, ALTURA - 2 * t, cEscuro);
    DrawRectangle(LARGURA - t, t, t, ALTURA - 2 * t, cEscuro);

    /* faixa interna para dar volume */
    DrawRectangle(6, 6, LARGURA - 12, t - 12, cMedio);
    DrawRectangle(6, ALTURA - t + 6, LARGURA - 12, t - 12, cMedio);
    DrawRectangle(6, t, t - 12, ALTURA - 2 * t, cMedio);
    DrawRectangle(LARGURA - t + 6, t, t - 12, ALTURA - 2 * t, cMedio);

    /* brilho/sombra chanfrados (topo/esquerda claros, base/direita escuros) */
    DrawLine(8, 8, LARGURA - 9, 8, cClaro);
    DrawLine(8, 8, 8, ALTURA - 9, cClaro);
    DrawLine(9, ALTURA - 9, LARGURA - 8, ALTURA - 9, cSombra);
    DrawLine(LARGURA - 9, 9, LARGURA - 9, ALTURA - 8, cSombra);

    /* veios horizontais na madeira */
    for (int x = 20; x < LARGURA - 20; x += 54) {
        DrawLine(x, 16, x + 26, 16, cClaro);
        DrawLine(x + 4, 23, x + 30, 23, cSombra);
        DrawLine(x, ALTURA - 17, x + 28, ALTURA - 17, cClaro);
        DrawLine(x + 3, ALTURA - 24, x + 31, ALTURA - 24, cSombra);
    }

    /* pregos */
    int px[] = { 22, LARGURA / 2, LARGURA - 22 };
    int pyTop = 18;
    int pyBot = ALTURA - 18;
    for (int i = 0; i < 3; i++) {
        DrawCircle(px[i], pyTop, 4, cPrego);
        DrawCircle(px[i], pyBot, 4, cPrego);
    }
    DrawCircle(18, ALTURA / 2, 4, cPrego);
    DrawCircle(LARGURA - 18, ALTURA / 2, 4, cPrego);
}

/* ------------------------------------------------------------------ */
/* IniciarMenu                                                          */
/* ------------------------------------------------------------------ */

void IniciarMenu(Menu *m) {
    m->selecionado = OPCAO_COMECAR;
    m->tempoCursor = 0.0f;
    m->temFundo    = 0;
    m->temFonte    = 0;

    const char *fundos[] = {
        "assets/sprites/Gemini_Generated_Image_ye86j2ye86j2ye86 (1).png",
        "assets/sprites/menu_bg.png"
    };

    for (int i = 0; i < 2; i++) {
        if (FileExists(fundos[i])) {
            m->fundo = LoadTexture(fundos[i]);
            m->temFundo = 1;
            break;
        }
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

    desenhar_borda_madeira();

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
