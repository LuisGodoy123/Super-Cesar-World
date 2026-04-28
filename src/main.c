#include "raylib.h"
#include "fase.h"
#include "jogador.h"
#include "inimigo.h"
#include "moeda.h"
#include "placar.h"
#include "menu.h"
#include <stddef.h>

#define LARGURA_TELA 1280
#define ALTURA_TELA   720

#define BONUS_COMPLETAR_FASE 500

typedef enum {
	MENU = 0,
	JOGANDO,
	GAME_OVER,
	VITORIA
} EstadoJogo;

// Utilitarios internos

static void resetar_posicao_jogador_preservando_status(Jogador *j) {
	int vidas = j->vidas;
	int pontos = j->pontos;

	IniciarJogador(j);

	j->vidas = vidas;
	j->pontos = pontos;
}

static int boss_ativo(No *listaInimigos) {
	No *atual = listaInimigos;
	while (atual != NULL) {
		if (atual->dados.tipo == BOSS && atual->dados.ativo) return 1;
		atual = atual->proximo;
	}
	return 0;
}

static void carregar_inimigos_da_fase(No **listaInimigos, int faseAtual) {
	if (faseAtual == 1) {
		AdicionarInimigo(listaInimigos, CAMINHADOR,  420.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR,  980.0f, 320.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 1520.0f, 512.0f);
	} else if (faseAtual == 2) {
		AdicionarInimigo(listaInimigos, CAMINHADOR,  350.0f, 448.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 1150.0f, 352.0f);
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 1700.0f, 416.0f);
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 2200.0f, 320.0f);
	} else if (faseAtual == 3) {
		AdicionarInimigo(listaInimigos, CAMINHADOR,  380.0f, 480.0f);
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 1100.0f, 352.0f);
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 1680.0f, 416.0f);
		AdicionarInimigo(listaInimigos, BOSS,       2360.0f, 576.0f);
	}
}

static void preparar_fase(Fase *fase,
						  Jogador *jogador,
						  No **listaInimigos,
						  NoMoeda **listaMoedas,
						  int faseAtual,
						  int preservarStatusJogador) {
	if (*listaInimigos != NULL) {
		LiberarInimigos(*listaInimigos);
		*listaInimigos = NULL;
	}

	if (*listaMoedas != NULL) {
		LiberarMoedas(*listaMoedas);
		*listaMoedas = NULL;
	}

	CarregarFase(fase, faseAtual);

	if (preservarStatusJogador) resetar_posicao_jogador_preservando_status(jogador);
	else                        IniciarJogador(jogador);

	CarregarMoedasDaFase(listaMoedas, fase);
	carregar_inimigos_da_fase(listaInimigos, faseAtual);
}

static int carregar_fonte_ui(Font *fonte) {
	const char *caminhos_fonte[] = {
		"assets/fontes/SuperMarioWorld.ttf",
		"assets/fontes/PressStart2P-Regular.ttf",
		"assets/fontes/SuperMario256.ttf",
	};
	int n = sizeof(caminhos_fonte) / sizeof(caminhos_fonte[0]);
	for (int i = 0; i < n; i++) {
		if (FileExists(caminhos_fonte[i])) {
			*fonte = LoadFontEx(caminhos_fonte[i], 128, NULL, 0);
			SetTextureFilter(fonte->texture, TEXTURE_FILTER_POINT);
			return 1;
		}
	}
	return 0;
}

static void desenhar_texto_ui(Font *fonte, int temFonte, const char *txt,
						  int x, int y, int tamanho, Color cor) {
	if (temFonte && fonte != NULL) {
		DrawTextEx(*fonte, txt, (Vector2){(float)x, (float)y}, (float)tamanho, 1, cor);
	} else {
		DrawText(txt, x, y, tamanho, cor);
	}
}

// main

int main(void) {
	InitWindow(LARGURA_TELA, ALTURA_TELA, "Super Cesar World");
	SetTargetFPS(60);

	const float FIXED_DT = 1.0f / 60.0f;
	float acumulador = 0.0f;

	EstadoJogo estado = MENU;

	Fase fase;
	Jogador jogador;
	Placar placar;
	Font fonteUI;
	int temFonteUI = 0;

	No *listaInimigos = NULL;
	NoMoeda *listaMoedas = NULL;

	int faseAtual = 1;
	int scoreRegistrado = 0;

	IniciarPlacar(&placar);
	CarregarPlacar(&placar);
	temFonteUI = carregar_fonte_ui(&fonteUI);

	Menu menu;
	IniciarMenu(&menu);

	const char *spritesJogador[] = {
		"assets/sprites/personagem1.png",
		"assets/sprites/personagem2.png",
		"assets/sprites/personagem3.png",
	};

	jogador.numSprites = 0;
	jogador.temSprites = 0;
	for (int i = 0; i < 3; i++) {
		if (FileExists(spritesJogador[i])) {
			jogador.sprites[jogador.numSprites] = LoadTexture(spritesJogador[i]);
			SetTextureFilter(jogador.sprites[jogador.numSprites], TEXTURE_FILTER_POINT);
			jogador.numSprites++;
		}
	}
	if (jogador.numSprites > 0) jogador.temSprites = 1;

	while (!WindowShouldClose()) {
		float frameTime = GetFrameTime();
		if (frameTime > 0.25f) frameTime = 0.25f;
		acumulador += frameTime;

		if (estado == MENU) {
			int opcao = AtualizarMenu(&menu);
			if (opcao == OPCAO_COMECAR ||
			    opcao == OPCAO_SLOT_A  ||
			    opcao == OPCAO_SLOT_B  ||
			    opcao == OPCAO_SLOT_C) {
				faseAtual = 1;
				scoreRegistrado = 0;
				preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 0);
				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, faseAtual);
				estado = JOGANDO;
			}
		} else if (estado == GAME_OVER) {
			if (!scoreRegistrado) {
				RegistrarPontuacaoFinal(&placar, jogador.pontos);
				SalvarPlacar(&placar);
				scoreRegistrado = 1;
			}

			if (IsKeyPressed(KEY_ENTER)) {
				estado = MENU;
			}
		} else if (estado == VITORIA) {
			if (!scoreRegistrado) {
				RegistrarPontuacaoFinal(&placar, jogador.pontos);
				SalvarPlacar(&placar);
				scoreRegistrado = 1;
			}

			if (IsKeyPressed(KEY_ENTER)) {
				estado = MENU;
			}
		}

		while (acumulador >= FIXED_DT) {
			if (estado == JOGANDO) {
				AtualizarJogador(&jogador, &fase);
				AtualizarInimigos(listaInimigos, &jogador, FIXED_DT);
				AtualizarMoedas(listaMoedas, &jogador);

				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, faseAtual);

				if (jogador.estado == MORTO) {
					estado = GAME_OVER;
					scoreRegistrado = 0;
				} else {
					float fimDaFase = (float)((COLUNAS - 2) * TILE);

					if (jogador.x >= fimDaFase) {
						if (faseAtual < 3) {
							jogador.pontos += BONUS_COMPLETAR_FASE;
							faseAtual++;
							preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, faseAtual);
						} else if (!boss_ativo(listaInimigos)) {
							jogador.pontos += BONUS_COMPLETAR_FASE;
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, faseAtual);
							estado = VITORIA;
							scoreRegistrado = 0;
						}
					}
				}
			}

			acumulador -= FIXED_DT;
		}

		BeginDrawing();

		if (estado == MENU) {
			DesenharMenu(&menu, &placar);
		} else if (estado == JOGANDO) {
			DesenharFase(&fase);
			DesenharMoedas(listaMoedas, fase.cameraX);
			DesenharInimigos(listaInimigos, fase.cameraX);
			DesenharJogador(&jogador);
			DesenharPlacar(&placar, &fonteUI, temFonteUI);

			if (faseAtual == 3 && boss_ativo(listaInimigos)) {
				DrawText("Derrote o boss para liberar a vitoria!", 340, 20, 24, GOLD);
			}
		} else if (estado == GAME_OVER) {
			ClearBackground(BLACK);

			desenhar_texto_ui(&fonteUI, temFonteUI, "GAME OVER", 500, 180, 72, RED);
			desenhar_texto_ui(&fonteUI, temFonteUI,
						  TextFormat("Pontuacao final: %d", jogador.pontos), 470, 300, 36, WHITE);
			desenhar_texto_ui(&fonteUI, temFonteUI,
						  "Pressione ENTER para voltar ao menu", 365, 380, 30, LIGHTGRAY);
		} else {
			ClearBackground(DARKBLUE);

			desenhar_texto_ui(&fonteUI, temFonteUI, "VITORIA!", 520, 180, 72, YELLOW);
			desenhar_texto_ui(&fonteUI, temFonteUI,
						  TextFormat("Pontuacao final: %d", jogador.pontos), 470, 300, 36, WHITE);
			desenhar_texto_ui(&fonteUI, temFonteUI,
						  "Recorde salvo! Pressione ENTER para voltar ao menu", 280, 380, 30, LIGHTGRAY);
		}

		EndDrawing();
	}

	LiberarInimigos(listaInimigos);
	LiberarMoedas(listaMoedas);
	LiberarMenu(&menu);
	if (temFonteUI) UnloadFont(fonteUI);
	if (jogador.temSprites) {
		for (int i = 0; i < jogador.numSprites; i++) {
			UnloadTexture(jogador.sprites[i]);
		}
	}
	CloseWindow();
	return 0;
}
