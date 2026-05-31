ifeq ($(OS),Windows_NT)
W64     ?= C:/raylib/w64devkit/bin
CC       = $(W64)/gcc.exe
TARGET   = build/super-cesar-world.exe
RAYLIB  ?= C:/raylib/raylib/src
CFLAGS   = -Wall -Wextra -std=c11 -Iinclude -I$(RAYLIB)
LDFLAGS  = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,console
RUN      = $(TARGET)
export PATH := $(W64);$(PATH)
setup:
	$(MAKE) -C $(RAYLIB) clean CC=$(CC)
	$(MAKE) -C $(RAYLIB) PLATFORM=PLATFORM_DESKTOP CC=$(CC)
else
CC      = gcc
TARGET  = build/super-cesar-world
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude
RUN     = ./$(TARGET)
UNAME   := $(shell uname -s)
ifeq ($(UNAME),Darwin)
LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
else
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif
setup:
	@echo "No Linux/macOS, instale a Raylib pelo gerenciador de pacotes."
endif

SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c, build/%.o, $(SRC))
HEADERS = $(wildcard include/*.h)

.PHONY: all clean run setup

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: all
ifeq ($(OS),Windows_NT)
	$(RUN)
else
	cd $(dir $(abspath $(lastword $(MAKEFILE_LIST)))) && ./$(TARGET)
endif

clean:
	rm -f build/*.o $(TARGET)
