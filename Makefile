# Ajuste este caminho para onde o raylib está instalado
RAYLIB_PATH ?= C:/raylib/raylib

CC      = gcc
TARGET  = build/super-cesar-world.exe
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c, build/%.o, $(SRC))

CFLAGS  = -Wall -Wextra -std=c11 \
          -Iinclude \
          -I$(RAYLIB_PATH)/src

LDFLAGS = -L$(RAYLIB_PATH)/src \
          -lraylib -lopengl32 -lgdi32 -lwinmm

.PHONY: all clean run

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: all
	./$(TARGET)

clean:
	rm -f build/*.o $(TARGET)
