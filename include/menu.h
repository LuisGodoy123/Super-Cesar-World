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
    int selecionado;       /* item atual do cursor */
    float tempoCursor;     /* animacao do cursor */
    Texture2D fundo;
    int temFundo;
    Texture2D texNome;
    int temNome;
    Font fonte;
    int temFonte;
} Menu;

void IniciarMenu(Menu *m);
void LiberarMenu(Menu *m);
int  AtualizarMenu(Menu *m);   /* retorna OpcaoMenu selecionada, ou -1 */
void DesenharMenu(Menu *m, Placar *p);

#endif
