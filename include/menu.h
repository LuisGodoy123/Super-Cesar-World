#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "placar.h"

typedef enum {
    OPCAO_COMECAR = 0,
    OPCAO_PLACAR,
    OPCAO_TOTAL
} OpcaoMenu;

typedef struct {
    int selecionado;
    float tempoCursor;
    Texture2D fundo;
    int temFundo;
    Texture2D texNome;
    int temNome;
    Font fonte;
    int temFonte;
} Menu;

void IniciarMenu(Menu *m);
void LiberarMenu(Menu *m);
int  AtualizarMenu(Menu *m);
void DesenharMenu(Menu *m, Placar *p);

#endif
