#include "raylib.h"
#include "fase.h"
#include "jogador.h"
#include "inimigo.h"
#include "moeda.h"
#include "chave.h"
#include "placar.h"
#include "menu.h"
#include <stddef.h>

#define LARGURA_TELA 1280
#define ALTURA_TELA   720

#define BONUS_COMPLETAR_FASE 500

typedef enum {
	MENU = 0,
	DIGITANDO_NICK,
	JOGANDO,
	GAME_OVER,
	VITORIA
} EstadoJogo;

void resetar_posicao_jogador_preservando_status(Jogador *j) {
	int   vidas     = j->vidas;
	int   pontos    = j->pontos;
	int   moedas    = j->moedas;
	int   cafeAtivo = j->cafeAtivo;
	float timerCafe = j->timerCafe;

	IniciarJogador(j);

	j->vidas     = vidas;
	j->pontos    = pontos;
	j->moedas    = moedas;
	j->cafeAtivo = cafeAtivo;
	j->timerCafe = timerCafe;
}

int boss_ativo(No *listaInimigos) {
	No *atual = listaInimigos;
	while (atual != NULL) {
		if (atual->dados.tipo == BOSS && atual->dados.ativo) return 1;
		atual = atual->proximo;
	}
	return 0;
}

int boss_vida(No *listaInimigos) {
	No *atual = listaInimigos;
	while (atual != NULL) {
		if (atual->dados.tipo == BOSS && atual->dados.ativo)
			return atual->dados.vida;
		atual = atual->proximo;
	}
	return 0;
}

float y_superficie_chao(Fase *fase, float x, int altura, float fallback) {
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

void carregar_inimigos_da_fase(No **listaInimigos, int faseAtual, Fase *fase) {
	if (faseAtual == 1) {
		AdicionarInimigo(listaInimigos, CAMINHADOR,  896.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 1184.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 1504.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 2560.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3392.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3584.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3360.0f, 352.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3680.0f, 352.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3520.0f, 224.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 4256.0f, 480.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 4704.0f, 416.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR, 5024.0f, 480.0f);
	} else if (faseAtual == 2) {
		float x1 =  350.0f;
		float x2 = 1150.0f;
		float x3 = 1700.0f;
		float x4 = 2200.0f;
		AdicionarInimigo(listaInimigos, CAMINHADOR, x1, y_superficie_chao(fase, x1, 32, 448.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, x2, y_superficie_chao(fase, x2, 32, 352.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x3, y_superficie_chao(fase, x3, 32, 416.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x4, y_superficie_chao(fase, x4, 32, 320.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 3296.0f, 336.0f);
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 3520.0f, 240.0f);
		AdicionarInimigo(listaInimigos, CAMINHADOR,  4352.0f, y_superficie_chao(fase, 4352.0f, 32, 640.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 4832.0f, y_superficie_chao(fase, 4832.0f, 48, 624.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, 5056.0f, y_superficie_chao(fase, 5056.0f, 48, 624.0f));
	} else if (faseAtual == 3) {
		float x1 =  380.0f;
		float x2 = 1100.0f;
		float x3 = 1680.0f;
		AdicionarInimigo(listaInimigos, CAMINHADOR, x1, y_superficie_chao(fase, x1, 32, 480.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x2, y_superficie_chao(fase, x2, 32, 352.0f));
		AdicionarInimigo(listaInimigos, PERSEGUIDOR, x3, y_superficie_chao(fase, x3, 32, 416.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, 2624.0f, y_superficie_chao(fase, 2624.0f, 32, 640.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, 2720.0f, y_superficie_chao(fase, 2720.0f, 32, 640.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, 2784.0f, y_superficie_chao(fase, 2784.0f, 32, 640.0f));
		AdicionarInimigo(listaInimigos, CAMINHADOR, 3072.0f, y_superficie_chao(fase, 3072.0f, 32, 640.0f));
		AdicionarInimigo(listaInimigos, BOSS, 3200.0f, 380.0f);
		(*listaInimigos)->dados.patrulhaMin  = 1408.0f;
		(*listaInimigos)->dados.patrulhaMax  = 4064.0f;
		(*listaInimigos)->dados.patrulhaYMin = 280.0f;
		(*listaInimigos)->dados.patrulhaYMax = 450.0f;
		(*listaInimigos)->dados.timerPouso   = 5.0f;
		(*listaInimigos)->dados.vy           = VEL_BOSS;
	}
}

void preparar_fase(Fase *fase,
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

int carregar_fonte_ui(Font *fonte) {
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

void desenhar_texto_hud(Font *fonte, int temFonte, const char *txt,
						  int x, int y, int tamanho, Color cor) {
	if (temFonte && fonte != NULL) {
		DrawTextEx(*fonte, txt, (Vector2){(float)x, (float)y}, (float)tamanho, 1, cor);
	} else {
		DrawText(txt, x, y, tamanho, cor);
	}
}

int main(void) {
	SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Super Cesar World");
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
	Chave chaveF1;
	Chave chaveF2;

	int faseAtual = 1;
	int scoreRegistrado = 0;
	float introTimer = 0.0f;
	float timerFase = 300.0f;
	char nickname[NICK_MAX + 1] = {0};
	int  nickLen = 0;

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

	Texture2D texIni1 = {0};
	if (FileExists("assets/sprites/inimigos/inimigo1andando.png")) {
		texIni1 = LoadTexture("assets/sprites/inimigos/inimigo1andando.png");
		SetTextureFilter(texIni1, TEXTURE_FILTER_POINT);
	}

	Texture2D texIni2 = {0};
	if (FileExists("assets/sprites/inimigos/inimigo1andando2.png")) {
		texIni2 = LoadTexture("assets/sprites/inimigos/inimigo1andando2.png");
		SetTextureFilter(texIni2, TEXTURE_FILTER_POINT);
	}


	Texture2D texIniRebaixado = {0};
	if (FileExists("assets/sprites/inimigos/inimigo1rebaixado.png")) {
		texIniRebaixado = LoadTexture("assets/sprites/inimigos/inimigo1rebaixado.png");
		SetTextureFilter(texIniRebaixado, TEXTURE_FILTER_POINT);
	}

	Texture2D texBoss[6] = {0};
	for (int i = 0; i < 6; i++) {
		const char *path = TextFormat("assets/sprites/inimigos/boss%d.png", i + 1);
		if (FileExists(path)) {
			texBoss[i] = LoadTexture(path);
			SetTextureFilter(texBoss[i], TEXTURE_FILTER_POINT);
		}
	}

	Texture2D texF2Ini1 = {0};
	if (FileExists("assets/sprites/inimigos/inimigo2andando.png")) {
		texF2Ini1 = LoadTexture("assets/sprites/inimigos/inimigo2andando.png");
		SetTextureFilter(texF2Ini1, TEXTURE_FILTER_POINT);
	}
	Texture2D texF2Ini2 = {0};
	if (FileExists("assets/sprites/inimigos/inimigo2andando2.png")) {
		texF2Ini2 = LoadTexture("assets/sprites/inimigos/inimigo2andando2.png");
		SetTextureFilter(texF2Ini2, TEXTURE_FILTER_POINT);
	}
	Texture2D texF2Ini3 = {0};
	if (FileExists("assets/sprites/inimigos/inimigo2andando3.png")) {
		texF2Ini3 = LoadTexture("assets/sprites/inimigos/inimigo2andando3.png");
		SetTextureFilter(texF2Ini3, TEXTURE_FILTER_POINT);
	}

	Texture2D texFundo2 = {0};
	if (FileExists("assets/sprites/sala_brum_jogo.png")) {
		texFundo2 = LoadTexture("assets/sprites/sala_brum_jogo.png");
		SetTextureFilter(texFundo2, TEXTURE_FILTER_BILINEAR);
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
	Sound sndDano = {0};
	if (FileExists("assets/sons/TomarDano.wav")) sndDano = LoadSound("assets/sons/TomarDano.wav");

	Music musicas[3] = {0};
	if (FileExists("assets/sons/SomPrimeiraFase.mp3")) musicas[0] = LoadMusicStream("assets/sons/SomPrimeiraFase.mp3");
	if (FileExists("assets/sons/SomSegundaFase.mp3"))  musicas[1] = LoadMusicStream("assets/sons/SomSegundaFase.mp3");
	if (FileExists("assets/sons/SomTerceiraFase.mp3")) musicas[2] = LoadMusicStream("assets/sons/SomTerceiraFase.mp3");
	for (int i = 0; i < 3; i++)
		if (musicas[i].ctxData != NULL) SetMusicVolume(musicas[i], 1.5f);
	int musicaAtual = 0;

	Music musicaMenu = {0};
	if (FileExists("assets/sons/SomTela Inicial.mp3")) musicaMenu = LoadMusicStream("assets/sons/SomTela Inicial.mp3");
	if (musicaMenu.ctxData != NULL) PlayMusicStream(musicaMenu);

	while (!WindowShouldClose()) {
		float frameTime = GetFrameTime();
		if (frameTime > 0.25f) frameTime = 0.25f;
		acumulador += frameTime;
		if (estado == JOGANDO) tempoAnimMoeda += frameTime;

		if (estado == MENU) {
			int opcao = AtualizarMenu(&menu);
			if (opcao == OPCAO_COMECAR) {
				estado  = DIGITANDO_NICK;
				nickLen = 0;
				nickname[0] = '\0';
			}
		} else if (estado == DIGITANDO_NICK) {
			int c = GetCharPressed();
			while (c > 0) {
				if (c >= 'a' && c <= 'z') c -= 32;
				int valido = (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
				if (valido && nickLen < NICK_MAX) {
					nickname[nickLen++] = (char)c;
					nickname[nickLen]   = '\0';
				}
				c = GetCharPressed();
			}
			if (IsKeyPressed(KEY_BACKSPACE) && nickLen > 0) {
				nickname[--nickLen] = '\0';
			}
			if (IsKeyPressed(KEY_ESCAPE)) {
				estado = MENU;
			}
			if (IsKeyPressed(KEY_ENTER) && nickLen > 0) {
				faseAtual = 1;
				scoreRegistrado = 0;
				timerFase = 300.0f;
				preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 0);
				IniciarChave(&chaveF1, 110 * TILE, 8 * TILE);
				IniciarChave(&chaveF2, 111 * TILE, 10 * TILE);
				AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
				estado = JOGANDO;
				introTimer = 2.5f;
				if (musicaMenu.ctxData != NULL) StopMusicStream(musicaMenu);
				if (musicas[0].ctxData != NULL) PlayMusicStream(musicas[0]);
				musicaAtual = 1;
			}
		} else if (estado == GAME_OVER) {
			if (!scoreRegistrado) {
				RegistrarPontuacaoFinal(&placar, jogador.pontos, nickname);
				SalvarPlacar(&placar);
				scoreRegistrado = 1;
			}

			if (IsKeyPressed(KEY_ENTER)) {
				estado = MENU;
				if (musicaMenu.ctxData != NULL) { StopMusicStream(musicaMenu); PlayMusicStream(musicaMenu); }
			}
		} else if (estado == VITORIA) {
			if (!scoreRegistrado) {
				RegistrarPontuacaoFinal(&placar, jogador.pontos, nickname);
				SalvarPlacar(&placar);
				scoreRegistrado = 1;
			}

			if (IsKeyPressed(KEY_ENTER)) {
				estado = MENU;
				if (musicaMenu.ctxData != NULL) { StopMusicStream(musicaMenu); PlayMusicStream(musicaMenu); }
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
				if (jogador.respawn) {
					jogador.respawn = 0;
					timerFase = 300.0f;
					preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
					if (faseAtual == 1) IniciarChave(&chaveF1, 110 * TILE, 8 * TILE);
					if (faseAtual == 2) IniciarChave(&chaveF2, 111 * TILE, 10 * TILE);
				}
				AtualizarInimigos(listaInimigos, &jogador, &fase, FIXED_DT, sndKick, sndDano);
				AtualizarMoedas(listaMoedas, &jogador, sndCoin);
				if (faseAtual == 1) AtualizarChave(&chaveF1, &jogador);
				if (faseAtual == 2) AtualizarChave(&chaveF2, &jogador);

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
							jogador.y < cy + cs && jogador.y + JOGADOR_ALTURA > cy) {
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
					if (musicaAtual > 0 && musicas[musicaAtual - 1].ctxData != NULL) StopMusicStream(musicas[musicaAtual - 1]);
					musicaAtual = 0;
				} else {
					int entrou_porta = 0;
					if (faseAtual == 1) {
						Rectangle playerHitbox = {
							jogador.x + JOGADOR_HITBOX_OFFSET_X, jogador.y,
							(float)JOGADOR_HITBOX_LARGURA, (float)JOGADOR_ALTURA
						};
						for (int l = 0; l < LINHAS && !entrou_porta; l++) {
							for (int c = 0; c < COLUNAS && !entrou_porta; c++) {
								if (fase.mapa[l][c] != PORTA) continue;
								Rectangle portaHitbox = {
									(float)(c * TILE), (float)(l * TILE),
									(float)TILE, (float)TILE
								};
								if (VerificarColisao(playerHitbox, portaHitbox) && chaveF1.coletada) {
									jogador.pontos += BONUS_COMPLETAR_FASE;
									faseAtual = 2;
									timerFase = 300.0f;
									preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
									IniciarChave(&chaveF2, 111 * TILE, 10 * TILE);
									AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
									introTimer = 2.5f;
									if (musicas[0].ctxData != NULL) StopMusicStream(musicas[0]);
									if (musicas[1].ctxData != NULL) PlayMusicStream(musicas[1]);
									musicaAtual = 2;
									entrou_porta = 1;
								}
							}
						}
					}
					if (faseAtual == 2) {
						Rectangle playerHitbox = {
							jogador.x + JOGADOR_HITBOX_OFFSET_X, jogador.y,
							(float)JOGADOR_HITBOX_LARGURA, (float)JOGADOR_ALTURA
						};
						for (int l = 0; l < LINHAS && !entrou_porta; l++) {
							for (int c = 0; c < COLUNAS && !entrou_porta; c++) {
								if (fase.mapa[l][c] != PORTA) continue;
								Rectangle portaHitbox = {
									(float)(c * TILE), (float)(l * TILE),
									(float)TILE, (float)TILE
								};
								if (VerificarColisao(playerHitbox, portaHitbox) && chaveF2.coletada) {
									jogador.pontos += BONUS_COMPLETAR_FASE;
									faseAtual = 3;
									timerFase = 300.0f;
									preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
									AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
									introTimer = 2.5f;
									if (musicas[1].ctxData != NULL) StopMusicStream(musicas[1]);
									if (musicas[2].ctxData != NULL) PlayMusicStream(musicas[2]);
									musicaAtual = 3;
									entrou_porta = 1;
								}
							}
						}
					}

					float fimDaFase = (float)((COLUNAS - 2) * TILE);
					float bordaDirJogador = jogador.x + JOGADOR_HITBOX_OFFSET_X + JOGADOR_HITBOX_LARGURA;

					if (!entrou_porta && bordaDirJogador >= fimDaFase) {
						if (faseAtual < 3 && (faseAtual != 1 || chaveF1.coletada) && (faseAtual != 2 || chaveF2.coletada)) {
							jogador.pontos += BONUS_COMPLETAR_FASE;
							if (musicaAtual > 0 && musicas[musicaAtual - 1].ctxData != NULL) StopMusicStream(musicas[musicaAtual - 1]);
							faseAtual++;
							timerFase = 300.0f;
							preparar_fase(&fase, &jogador, &listaInimigos, &listaMoedas, faseAtual, 1);
							if (faseAtual == 2) IniciarChave(&chaveF2, 111 * TILE, 10 * TILE);
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
							introTimer = 2.5f;
							if (musicas[faseAtual - 1].ctxData != NULL) PlayMusicStream(musicas[faseAtual - 1]);
							musicaAtual = faseAtual;
						} else if (faseAtual == 3 && !boss_ativo(listaInimigos)) {
							jogador.pontos += BONUS_COMPLETAR_FASE;
							AtualizarPlacar(&placar, jogador.pontos, jogador.vidas, jogador.moedas, faseAtual, (int)timerFase);
							estado = VITORIA;
							scoreRegistrado = 0;
							if (musicaAtual > 0 && musicas[musicaAtual - 1].ctxData != NULL) StopMusicStream(musicas[musicaAtual - 1]);
							musicaAtual = 0;
						}
					}
				}
			}

			acumulador -= FIXED_DT;
		}

		if (estado == MENU && musicaMenu.ctxData != NULL)
			UpdateMusicStream(musicaMenu);
		if (estado == JOGANDO && musicaAtual > 0 && musicas[musicaAtual - 1].ctxData != NULL)
			UpdateMusicStream(musicas[musicaAtual - 1]);

		BeginDrawing();

		if (estado == MENU) {
			DesenharMenu(&menu, &placar);
		} else if (estado == DIGITANDO_NICK) {
			DesenharMenu(&menu, NULL);
			DesenharInputNick(&menu, nickname);
		} else if (estado == JOGANDO) {
			float extraZoom = 1.0f;
			if (introTimer > 0.0f) {
				extraZoom = 1.0f + 1.5f * introTimer;
				introTimer -= frameTime;
				if (introTimer < 0.0f) introTimer = 0.0f;
			}

			float playerSX = (jogador.x - jogador.cameraX) * CAMERA_ZOOM + JOGADOR_LARGURA * CAMERA_ZOOM / 2.0f;
			float playerSY = (jogador.y - fase.cameraYOffset) * CAMERA_ZOOM + JOGADOR_ALTURA * CAMERA_ZOOM / 2.0f;
			Camera2D cam = { 0 };
			cam.offset   = (Vector2){ playerSX, playerSY };
			cam.target   = (Vector2){ playerSX, playerSY };
			cam.zoom     = extraZoom;

			BeginMode2D(cam);
			DesenharFase(&fase, texBloco, texTijoloCinza, texTerra, texCafe, texFundo2);
			DesenharMoedas(listaMoedas, fase.cameraX, fase.cameraYOffset, texMoedas, numFramesMoeda, tempoAnimMoeda);
			if (faseAtual == 1) DesenharChave(chaveF1, fase.cameraX, fase.cameraYOffset);
			if (faseAtual == 2) DesenharChave(chaveF2, fase.cameraX, fase.cameraYOffset);
			DesenharInimigos(listaInimigos, fase.cameraX, fase.cameraYOffset, texIni1, texIni2, texIniRebaixado, texF2Ini1, texF2Ini2, texF2Ini3, texBoss);
			DesenharJogador(&jogador, fase.cameraYOffset);
			EndMode2D();

			int frameAtual = (numFramesMoeda > 0) ? ((int)(tempoAnimMoeda * 10.0f) % numFramesMoeda) : 0;
			DesenharPlacar(&placar, &fonteUI, temFonteUI,
			               (numFramesMoeda > 0) ? texMoedas[frameAtual] : (Texture2D){0},
			               numFramesMoeda > 0);

			if (faseAtual == 3 && jogador.x > 105 * 32.0f)
				DesenharBarraVidaBoss(boss_vida(listaInimigos), 8);

			if (jogador.cafeAtivo) DrawText(TextFormat("[CAFE] %.1fs", jogador.timerCafe), LARGURA_TELA - 140, 32, 20, BROWN);

			if (faseAtual == 3 && !boss_ativo(listaInimigos) && jogador.x > 105 * 32.0f) {
				const char *msg = "Boss derrotado! Va ate o fim da fase!";
				int msgW = MeasureText(msg, 24);
				DrawText(msg, (LARGURA_TELA - msgW) / 2, 20, 24, GREEN);
			}
		} else if (estado == GAME_OVER) {
			ClearBackground(BLACK);
			Font fm = temFonteUI ? fonteUI : GetFontDefault();
			const char *s1 = "GAME OVER";
			const char *s2 = TextFormat("Pontuacao final: %d", jogador.pontos);
			const char *s3 = "Pressione ENTER para voltar ao menu";
			desenhar_texto_hud(&fonteUI, temFonteUI, s1, (LARGURA_TELA - (int)MeasureTextEx(fm, s1, 48, 1).x) / 2, 260, 48, RED);
			desenhar_texto_hud(&fonteUI, temFonteUI, s2, (LARGURA_TELA - (int)MeasureTextEx(fm, s2, 26, 1).x) / 2, 340, 26, WHITE);
			desenhar_texto_hud(&fonteUI, temFonteUI, s3, (LARGURA_TELA - (int)MeasureTextEx(fm, s3, 20, 1).x) / 2, 400, 20, LIGHTGRAY);
		} else {
			ClearBackground(DARKBLUE);
			Font fm = temFonteUI ? fonteUI : GetFontDefault();
			const char *s1 = "VITORIA!";
			const char *s2 = TextFormat("Pontuacao final: %d", jogador.pontos);
			const char *s3 = "Recorde salvo! Pressione ENTER para voltar ao menu";
			desenhar_texto_hud(&fonteUI, temFonteUI, s1, (LARGURA_TELA - (int)MeasureTextEx(fm, s1, 48, 1).x) / 2, 260, 48, YELLOW);
			desenhar_texto_hud(&fonteUI, temFonteUI, s2, (LARGURA_TELA - (int)MeasureTextEx(fm, s2, 26, 1).x) / 2, 340, 26, WHITE);
			desenhar_texto_hud(&fonteUI, temFonteUI, s3, (LARGURA_TELA - (int)MeasureTextEx(fm, s3, 20, 1).x) / 2, 400, 20, LIGHTGRAY);
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
	if (texIni1.id > 0) UnloadTexture(texIni1);
	if (texIni2.id > 0) UnloadTexture(texIni2);
	if (texIniRebaixado.id > 0) UnloadTexture(texIniRebaixado);
	if (texF2Ini1.id > 0) UnloadTexture(texF2Ini1);
	if (texF2Ini2.id > 0) UnloadTexture(texF2Ini2);
	if (texF2Ini3.id > 0) UnloadTexture(texF2Ini3);
	for (int i = 0; i < 6; i++)
		if (texBoss[i].id > 0) UnloadTexture(texBoss[i]);
	if (texFundo2.id > 0) UnloadTexture(texFundo2);
	UnloadSound(sndCoin);
	UnloadSound(sndJump);
	UnloadSound(snd1up);
	UnloadSound(sndKick);
	UnloadSound(sndPowerup);
	UnloadSound(sndDano);
	for (int i = 0; i < 3; i++)
		if (musicas[i].ctxData != NULL) UnloadMusicStream(musicas[i]);
	if (musicaMenu.ctxData != NULL) UnloadMusicStream(musicaMenu);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
