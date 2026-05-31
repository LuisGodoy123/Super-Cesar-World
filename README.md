# SUPER CESAR WORLD

**Projeto da disciplina: Programacao Imperativa e Funcional**
**Semestre:** 2025.2

---

## Sobre o Jogo

Super Cesar World e um jogo de plataforma inspirado no classico Super Mario, onde o jogador controla Cesar atravessando tres fases distintas repletas de inimigos e obstaculos. Com fisica de plataforma, pathfinding inteligente e um sistema de pontuacao, o objetivo e chegar ao laboratorio secreto, derrotar o boss final e escapar.

---

## Caracteristicas

**3 Fases Unicas:**
- Fase 1: Interior do Mercado
- Fase 2: Rua com Predios
- Fase 3: Laboratorio Secreto

**3 Tipos de Inimigos:**
- Caminhador - Anda de um lado ao outro, resiste a 2 pisadas
- Perseguidor - Rastreia o jogador com pathfinding inteligente, detecta bordas e morre em 1 pisada
- Boss - Patrulha a area, sobe e desce enquanto se move, e necessario 8 pisadas para derrotar

**Sistema de Itens:**
- Moedas - Espalhadas pelas fases, valem 100 pontos cada
- Blocos de Power-up - Ao serem golpeados por baixo, soltam um cafe que concede boost de velocidade
- Chaves - Necessarias para abrir a porta e avancar de fase (Fases 1 e 2)

**Power-up de Cafe:** Dobra a velocidade do jogador por 5 segundos

**Timer por Fase:** 300 segundos (5 minutos) para completar cada fase

**Sistema de Ranking:** Top 5 melhores pontuacoes salvas

---

## Conceitos de Programacao Aplicados

- Structs - Jogador, Inimigo, Moeda, Bloco, Fase, Chave, Placar
- Ponteiros - Manipulacao de listas encadeadas de inimigos e moedas
- Alocacao Dinamica - malloc() e free() para gerenciar inimigos e moedas em tempo de execucao
- Listas Encadeadas - Inimigos (No) e moedas (NoMoeda) como listas ligadas
- Arquivos - Leitura e escrita do ranking em data/placar.dat
- Matrizes - Mapas de tiles 130x23 (colunas x linhas), cada tile com 32x32 pixels
- Funcoes - Modularizacao completa do codigo em modulos separados

---

## Requisitos

- [Raylib](https://www.raylib.com/) 6.0+
- Compilador C (gcc)
- Make
- Git

---

## Instalacao das Dependencias

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential git libgl1-mesa-dev libx11-dev
git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git /tmp/raylib
cd /tmp/raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
sudo ldconfig
cd -
```

#### macOS

Instale o [Homebrew](https://brew.sh/) e depois:
```bash
brew install raylib gcc git make
```

> **Nota:** Caso o Homebrew instale uma versao anterior a 6.0, compile do fonte seguindo o mesmo procedimento da Opcao 2 do Linux (substitua `sudo ldconfig` por nada — no macOS nao e necessario `ldconfig`).

#### Windows

1. Baixe o [w64devkit](https://github.com/skeeto/w64devkit/releases) e extraia em `C:\raylib\w64devkit\`
2. Baixe o [codigo-fonte da Raylib 6.0](https://github.com/raysan5/raylib/releases/tag/6.0) e extraia em `C:\raylib\raylib\`

> **Atencao:** `make setup` apenas compila a Raylib. Certifique-se de ter baixado e extraido o codigo-fonte da Raylib 6.0 no caminho correto antes de rodar esse comando.

Em seguida, compile a Raylib uma unica vez com o mesmo gcc que sera usado no jogo:
```bash
make setup
```

Se instalou em caminhos diferentes, passe as variaveis:
```bash
make setup W64=C:/seu/w64devkit/bin RAYLIB=C:/seu/raylib/src
```

> **Importante:** nao use outro gcc (MinGW antigo, MSYS2, etc). O Makefile usa o gcc do w64devkit diretamente para garantir compatibilidade com a Raylib.

---

## Instalacao e Execucao

### Passo 1: Clonar o Repositorio
```bash
git clone https://github.com/LuisGodoy123/Super-Cesar-World.git
cd Super-Cesar-World
```

### Passo 2: Compilar a Raylib (apenas Windows, apenas uma vez)

```bash
make setup
# ou, se instalou em outro caminho:
make setup RAYLIB=C:/caminho/para/raylib/src
```

### Passo 3: Compilar o Jogo

```bash
make
# ou, se instalou em caminhos diferentes (Windows):
make W64=C:/seu/w64devkit/bin RAYLIB=C:/seu/raylib/src
```

### Passo 4: Executar

> **Atencao:** Execute sempre a partir da raiz do projeto (onde estao as pastas `assets/` e `data/`). Rodar o executavel de outro diretorio fara o jogo iniciar sem sprites, sons e sem salvar o placar.

```bash
make run
# ou diretamente apos compilar:
./build/super-cesar-world        # Linux/macOS
.\build\super-cesar-world.exe    # Windows
```

### Limpeza dos Arquivos de Build
```bash
make clean
```

---

## Comandos do Makefile
```bash
make setup    # Compila a Raylib (Windows, apenas uma vez)
make          # Compila o jogo
make run      # Compila e executa
make clean    # Remove arquivos compilados
```

---

## Controles

| Tecla                  | Acao                  |
|------------------------|-----------------------|
| Seta Esquerda / A      | Mover para a esquerda |
| Seta Direita / D       | Mover para a direita  |
| Espaco / Seta Cima / W | Pular                 |

---


## Sistema de Pontuacao

| Acao                    | Pontos |
|-------------------------|--------|
| Pular em Caminhador     | 200    |
| Pular em Perseguidor    | 300    |
| Derrotar o Boss         | 1000   |
| Coletar moeda           | 100    |
| Golpear bloco de poder  | 200    |
| Completar uma fase      | 500    |

---

## Sistema de Ranking

- Top 5 melhores pontuacoes salvas ao fim de cada partida
- Arquivo salvo em: `data/placar.dat`
- Pontuacoes visiveis no menu principal

---

## Screencast

https://github.com/user-attachments/assets/0d6ac7f2-f0f4-44bb-bd3d-0fb81da45ac2
