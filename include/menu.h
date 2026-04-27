#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "placar.h"

typedef enum {
    OPCAO_SLOT_A = 0,
    OPCAO_SLOT_B,
    OPCAO_SLOT_C,
    OPCAO_COMECAR,
    OPCAO_PLACAR,
    OPCAO_OPCOES,
    OPCAO_TOTAL
} OpcaoMenu;

typedef struct {
    int selecionado;       /* item atual do cursor */
    float tempoCursor;     /* animacao do cursor */
    Texture2D fundo;
    int temFundo;
    Font fonte;
    int temFonte;
} Menu;

void IniciarMenu(Menu *m);
void LiberarMenu(Menu *m);
int  AtualizarMenu(Menu *m);   /* retorna OpcaoMenu selecionada, ou -1 */
void DesenharMenu(Menu *m, Placar *p);

#endif
