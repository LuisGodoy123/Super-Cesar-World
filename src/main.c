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
	int   vidas     = j->vidas;
	int   pontos    = j->pontos;
	int   moedas    = j->moedas;
	int   devMode   = j->devMode;
	int   cafeAtivo = j->cafeAtivo;
	float timerCafe = j->timerCafe;

	IniciarJogador(j);

	j->vidas     = vidas;
	j->pontos    = pontos;
	j->moedas    = moedas;
	j->devMode   = devMode;
	j->cafeAtivo = cafeAtivo;
	j->timerCafe = timerCafe;
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
		float xs[] = { 300.0f, 620.0f, 950.0f, 1380.0f, 1760.0f,
		               2150.0f, 2530.0f, 2950.0f, 3400.0f };
		int n = sizeof(xs) / sizeof(xs[0]);
		for (int i = 0; i < n; i++)
			AdicionarInimigo(listaInimigos, CAMINHADOR, xs[i],
			                 y_superficie_chao(fase, xs[i], 32, 480.0f));
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
	InitAudioDevice();
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
	float introTimer = 0.0f;
	float timerFase = 300.0f;

	IniciarPlacar(&placar);
	CarregarPlacar(&placar);
	temFonteUI = carregar_fonte_ui(&fonteUI);

	Menu menu;
	IniciarMenu(&menu);

	const char *spritesJogador[] = {
		"assets/sprites/cesar.png",
		"assets/sprites/cesar2.png",
		"assets/sprites/cesar3.png",
	};

	jogador.numSprites = 0;
	jogador.temSprites = 0;
	int nSpritesJogador = sizeof(spritesJogador) / sizeof(spritesJogador[0]);
	for (int i = 0; i < nSpritesJogador; i++) {
		if (FileExists(spritesJogador[i])) {
			jogador.sprites[jogador.numSprites] = LoadTexture(spritesJogador[i]);
			SetTextureFilter(jogador.sprites[jogador.numSprites], TEXTURE_FILTER_POINT);
			jogador.numSprites++;
		}
	}
	if (jogador.numSprites > 0) jogador.temSprites = 1;

	#define NUM_FRAMES_MOEDA 10
	Texture2D texMoedas[NUM_FRAMES_MOEDA] = {0};
	int numFramesMoeda = 0;
	for (int i = 0; i < NUM_FRAMES_MOEDA; i++) {
		const char *caminho = TextFormat("assets/sprites/moedas/coin_frame_%02d.png", i);
		if (FileExists(caminho)) {
			texMoedas[numFramesMoeda] = LoadTexture(caminho);
			SetTextureFilter(texMoedas[numFramesMoeda], TEXTURE_FILTER_POINT);
			numFramesMoeda++;
		}
	}
	float tempoAnimMoeda = 0.0f;

	Texture2D texBloco = {0};
	if (FileExists("assets/sprites/blocos/blocoPoder.png")) {
		texBloco = LoadTexture("assets/sprites/blocos/blocoPoder.png");
		SetTextureFilter(texBloco, TEXTURE_FILTER_POINT);
	}

	Texture2D texCafe = {0};
	if (FileExists("assets/sprites/Cafe.png")) {
		texCafe = LoadTexture("assets/sprites/Cafe.png");
		SetTextureFilter(texCafe, TEXTURE_FILTER_POINT);
	}

	Texture2D texTijoloCinza = {0};
	if (FileExists("assets/sprites/blocos/tijolocinza.png")) {
		texTijoloCinza = LoadTexture("assets/sprites/blocos/tijolocinza.png");
		SetTextureFilter(texTijoloCinza, TEXTURE_FILTER_POINT);
	}

	Texture2D texTerra = {0};
	if (FileExists("assets/sprites/blocos/terra.png")) {
		texTerra = LoadTexture("assets/sprites/blocos/terra.png");
		SetTextureFilter(texTerra, TEXTURE_FILTER_POINT);
	}

	Texture2D texNuvem1 = {0};
	if (FileExists("assets/sprites/elementosDeCenario/nuvem1.png")) {
		texNuvem1 = LoadTexture("assets/sprites/elementosDeCenario/nuvem1.png");
		SetTextureFilter(texNuvem1, TEXTURE_FILTER_BILINEAR);
	}
	Texture2D texNuvem2 = {0};
	if (FileExists("assets/sprites/elementosDeCenario/nuvem2.png")) {
		texNuvem2 = LoadTexture("assets/sprites/elementosDeCenario/nuvem2.png");
		SetTextureFilter(texNuvem2, TEXTURE_FILTER_BILINEAR);
	}
	Texture2D texNuvem3 = {0};
	if (FileExists("assets/sprites/elementosDeCenario/nuvem3.png")) {
		texNuvem3 = LoadTexture("assets/sprites/elementosDeCenario/nuvem3.png");
		SetTextureFilter(texNuvem3, TEXTURE_FILTER_BILINEAR);
	}

	Texture2D texIni1 = {0};
	if (FileExists("assets/sprites/inimigo1andando.png")) {
		texIni1 = LoadTexture("assets/sprites/inimigo1andando.png");
		SetTextureFilter(texIni1, TEXTURE_FILTER_POINT);
	}

	Texture2D texIni2 = {0};
	if (FileExists("assets/sprites/inimigo1andando2.png")) {
		texIni2 = LoadTexture("assets/sprites/inimigo1andando2.png");
		SetTextureFilter(texIni2, TEXTURE_FILTER_POINT);
	}

	
	Texture2D texIniRebaixado = {0};
	if (FileExists("assets/sprites/inimigo1rebaixado.png")) {
		texIniRebaixado = LoadTexture("assets/sprites/inimigo1rebaixado.png");
		SetTextureFilter(texIniRebaixado, TEXTURE_FILTER_POINT);
	}

	Sound sndCoin = {0};
	if (FileExists("assets/sons/coin.wav"))  sndCoin  = LoadSound("assets/sons/coin.wav");
	Sound sndJump = {0};
	if (FileExists("assets/sons/jump.wav"))  sndJump  = LoadSound("assets/sons/jump.wav");
	Sound snd1up  = {0};
	if (FileExists("assets/sons/1up.wav"))   snd1up   = LoadSound("assets/sons/1up.wav");
	Sound sndKick = {0};
	if (FileExists("assets/sons/kick.wav"))  sndKick  = LoadSound("assets/sons/kick.wav");
	Sound sndPowerup = {0};
	if (FileExists("assets/sons/powerup.wav")) sndPowerup = LoadSound("assets/sons/powerup.wav");

	while (!WindowShouldClose()) {
		float frameTime = GetFrameTime();
		if (frameTime > 0.25f) frameTime = 0.25f;
		acumulador += frameTime;
		if (estado == JOGANDO) tempoAnimMoeda += frameTime;

		if (estado == MENU) {
			int opcao = AtualizarMenu(&menu);
			if (opcao == OPCAO_COMECAR ||
			    opcao == OPCAO_SLOT_A  ||
			    opcao == OPCAO_SLOT_B  ||
			    opcao == OPCAO_SLOT_C) {
				faseAtual = 1;
				scoreRegistrado = 0;
				timerFase = 300.0f;
				preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 0);
				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
				estado = JOGANDO;
				introTimer = 2.5f;
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

		if (estado == JOGANDO && introTimer <= 0.0f) {
			timerFase -= frameTime;
			if (timerFase < 0.0f) timerFase = 0.0f;
			if (timerFase == 0.0f) {
				estado = GAME_OVER;
				scoreRegistrado = 0;
			}
		}

		while (acumulador >= FIXED_DT) {
			if (estado == JOGANDO) {
				AtualizarJogador(&jogador, &fase, introTimer > 0.0f, sndJump, snd1up);
				AtualizarInimigos(listaInimigos, &jogador, &fase, FIXED_DT, sndKick);
				AtualizarMoedas(listaMoedas, &jogador, sndCoin);

				for (int l = 0; l < LINHAS; l++) {
					for (int c = 0; c < COLUNAS; c++) {
						Bloco *b = &fase.blocos[l][c];
						if (b->tipo != BLOCO_TIPO_POWERUP || b->estado != BLOCO_ESTADO_USADO || b->cafeColetado) continue;
						if (b->cafeOffset > -(float)TILE) {
							b->cafeOffset -= 1.5f;
							if (b->cafeOffset < -(float)TILE) b->cafeOffset = -(float)TILE;
						}
						float cx = (float)(c * TILE) + 4, cy = (float)(l * TILE) + b->cafeOffset, cs = (float)(TILE - 8);
						if (jogador.x + JOGADOR_HITBOX_OFFSET_X < cx + cs &&
							jogador.x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA > cx &&
							jogador.y < cy + cs && jogador.y + jogador.alturaAtual > cy) {
							b->cafeColetado = 1;
							jogador.cafeAtivo = 1;
							jogador.timerCafe = TEMPO_CAFE;
							PlaySound(sndPowerup);
						}
					}
				}

				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);

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
							timerFase = 300.0f;
							preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
							introTimer = 2.5f;
						} else if (!boss_ativo(listaInimigos)) {
							jogador.pontos += BONUS_COMPLETAR_FASE;
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
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
			float extraZoom = 1.0f;
			if (introTimer > 0.0f) {
				extraZoom = 1.0f + 1.5f * introTimer;
				introTimer -= frameTime;
				if (introTimer < 0.0f) introTimer = 0.0f;
			}

			float playerSX = (jogador.x - jogador.cameraX) * CAMERA_ZOOM + JOGADOR_LARGURA * CAMERA_ZOOM / 2.0f;
			float playerSY = (jogador.y - fase.cameraYOffset) * CAMERA_ZOOM + jogador.alturaAtual * CAMERA_ZOOM / 2.0f;
			Camera2D cam = { 0 };
			cam.offset   = (Vector2){ playerSX, playerSY };
			cam.target   = (Vector2){ playerSX, playerSY };
			cam.zoom     = extraZoom;

			BeginMode2D(cam);
			DesenharFase(&fase, texBloco, texTijoloCinza, texTerra, texNuvem1, texNuvem2, texNuvem3, texCafe);
			DesenharMoedas(listaMoedas, fase.cameraX, fase.cameraYOffset, texMoedas, numFramesMoeda, tempoAnimMoeda);
			DesenharInimigos(listaInimigos, fase.cameraX, fase.cameraYOffset, texIni1, texIni2, texIniRebaixado);
			DesenharJogador(&jogador, fase.cameraYOffset);
			EndMode2D();

			int frameAtual = (numFramesMoeda > 0) ? ((int)(tempoAnimMoeda * 10.0f) % numFramesMoeda) : 0;
			DesenharPlacar(&placar, &fonteUI, temFonteUI,
			               (numFramesMoeda > 0) ? texMoedas[frameAtual] : (Texture2D){0},
			               numFramesMoeda > 0);

			if (jogador.devMode)   DrawText("[DEV] x3", LARGURA_TELA - 110,  8, 20, RED);
			if (jogador.cafeAtivo) DrawText(TextFormat("[CAFE] %.1fs", jogador.timerCafe), LARGURA_TELA - 140, 32, 20, BROWN);

			if (faseAtual == 3 && boss_ativo(listaInimigos)) {
				DrawText("Derrote o boss para liberar a vitoria!", 340, 20, 24, GOLD);
			}
		} else if (estado == GAME_OVER) {
			ClearBackground(BLACK);
			Font fm = temFonteUI ? fonteUI : GetFontDefault();
			const char *s1 = "GAME OVER";
			const char *s2 = TextFormat("Pontuacao final: %d", jogador.pontos);
			const char *s3 = "Pressione ENTER para voltar ao menu";
			desenhar_texto_ui(&fonteUI, temFonteUI, s1, (LARGURA_TELA - (int)MeasureTextEx(fm, s1, 48, 1).x) / 2, 260, 48, RED);
			desenhar_texto_ui(&fonteUI, temFonteUI, s2, (LARGURA_TELA - (int)MeasureTextEx(fm, s2, 26, 1).x) / 2, 340, 26, WHITE);
			desenhar_texto_ui(&fonteUI, temFonteUI, s3, (LARGURA_TELA - (int)MeasureTextEx(fm, s3, 20, 1).x) / 2, 400, 20, LIGHTGRAY);
		} else {
			ClearBackground(DARKBLUE);
			Font fm = temFonteUI ? fonteUI : GetFontDefault();
			const char *s1 = "VITORIA!";
			const char *s2 = TextFormat("Pontuacao final: %d", jogador.pontos);
			const char *s3 = "Recorde salvo! Pressione ENTER para voltar ao menu";
			desenhar_texto_ui(&fonteUI, temFonteUI, s1, (LARGURA_TELA - (int)MeasureTextEx(fm, s1, 48, 1).x) / 2, 260, 48, YELLOW);
			desenhar_texto_ui(&fonteUI, temFonteUI, s2, (LARGURA_TELA - (int)MeasureTextEx(fm, s2, 26, 1).x) / 2, 340, 26, WHITE);
			desenhar_texto_ui(&fonteUI, temFonteUI, s3, (LARGURA_TELA - (int)MeasureTextEx(fm, s3, 20, 1).x) / 2, 400, 20, LIGHTGRAY);
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
	for (int i = 0; i < numFramesMoeda; i++) {
		if (texMoedas[i].id > 0) UnloadTexture(texMoedas[i]);
	}
	if (texCafe.id > 0) UnloadTexture(texCafe);
	if (texBloco.id > 0) UnloadTexture(texBloco);
	if (texTijoloCinza.id > 0) UnloadTexture(texTijoloCinza);
	if (texTerra.id > 0) UnloadTexture(texTerra);
	if (texNuvem1.id > 0) UnloadTexture(texNuvem1);
	if (texNuvem2.id > 0) UnloadTexture(texNuvem2);
	if (texNuvem3.id > 0) UnloadTexture(texNuvem3);
	if (texIni1.id > 0) UnloadTexture(texIni1);
	if (texIni2.id > 0) UnloadTexture(texIni2);
	if (texIniRebaixado.id > 0) UnloadTexture(texIniRebaixado);
	UnloadSound(sndCoin);
	UnloadSound(sndJump);
	UnloadSound(snd1up);
	UnloadSound(sndKick);
	UnloadSound(sndPowerup);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
