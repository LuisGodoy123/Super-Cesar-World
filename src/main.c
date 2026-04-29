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

static float y_superficie_chao(Fase *fase, float x, int altura, float fallback) {
	if (fase == NULL) return fallback;

	int col = (int)(x / TILE);
	if (col < 0) col = 0;
	if (col >= COLUNAS) col = COLUNAS - 1;

	for (int l = 0; l < LINHAS; l++) {
		if (fase->mapa[l][col] == BLOCO) {
			if (l == LINHAS - 1 || fase->mapa[l + 1][col] == BLOCO) {
				return (float)(l * TILE - altura);
			}
		}
	}

	return fallback;
}

static void carregar_inimigos_da_fase(No **listaInimigos, int faseAtual, Fase *fase) {
	if (faseAtual == 1) {
		float x1 =  384.0f;
		float x2 = 1184.0f;
		float x3 = 1984.0f;
		AdicionarInimigo(listaInimigos, CAMINHADOR, x1, y_superficie_chao(fase, x1, 32, 480.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, x2, y_superficie_chao(fase, x2, 32, 480.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, x3, y_superficie_chao(fase, x3, 32, 608.0f));
	} else if (faseAtual == 2) {
		float x1 =  350.0f;
		float x2 = 1150.0f;
		float x3 = 1700.0f;
		float x4 = 2200.0f;
		AdicionarInimigo(listaInimigos, CAMINHADOR, x1, y_superficie_chao(fase, x1, 32, 448.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, x2, y_superficie_chao(fase, x2, 32, 352.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x3, y_superficie_chao(fase, x3, 32, 416.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x4, y_superficie_chao(fase, x4, 32, 320.0f));
	} else if (faseAtual == 3) {
		float x1 =  380.0f;
		float x2 = 1100.0f;
		float x3 = 1680.0f;
		float x4 = 2360.0f;
		AdicionarInimigo(listaInimigos, CAMINHADOR, x1, y_superficie_chao(fase, x1, 32, 480.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x2, y_superficie_chao(fase, x2, 32, 352.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x3, y_superficie_chao(fase, x3, 32, 416.0f));
		AdicionarInimigo(listaInimigos, BOSS,       x4, y_superficie_chao(fase, x4, 64, 576.0f));
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
	carregar_inimigos_da_fase(listaInimigos, faseAtual, fase);
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

	Texture2D texMoeda = {0};
	if (FileExists("assets/sprites/moeda.png")) {
		texMoeda = LoadTexture("assets/sprites/moeda.png");
		SetTextureFilter(texMoeda, TEXTURE_FILTER_POINT);
	}

	Texture2D texBloco = {0};
	if (FileExists("assets/sprites/blocoPoder.png")) {
		texBloco = LoadTexture("assets/sprites/blocoPoder.png");
		SetTextureFilter(texBloco, TEXTURE_FILTER_POINT);
	}

	Texture2D texTijoloCinza = {0};
	if (FileExists("assets/sprites/tijolocinza.png")) {
		texTijoloCinza = LoadTexture("assets/sprites/tijolocinza.png");
		SetTextureFilter(texTijoloCinza, TEXTURE_FILTER_POINT);
	}

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
				AtualizarInimigos(listaInimigos, &jogador, &fase, FIXED_DT);
				AtualizarMoedas(listaMoedas, &jogador);

				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, faseAtual);

				if (jogador.estado == MORTO) {
					estado = GAME_OVER;
					scoreRegistrado = 0;
				} else {
					float fimDaFase = (float)((COLUNAS - 2) * TILE);
					float bordaDirJogador = jogador.x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA;

					if (bordaDirJogador >= fimDaFase) {
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
			DesenharFase(&fase, texBloco, texTijoloCinza);
			DesenharMoedas(listaMoedas, fase.cameraX, texMoeda);
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
	if (texMoeda.id > 0) UnloadTexture(texMoeda);
	if (texBloco.id > 0) UnloadTexture(texBloco);
	if (texTijoloCinza.id > 0) UnloadTexture(texTijoloCinza);
	CloseWindow();
	return 0;
}
