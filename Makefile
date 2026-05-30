ifeq ($(OS),Windows_NT)
W64     = C:/raylib/w64devkit/bin
CC      = $(W64)/gcc.exe
TARGET  = build/super-cesar-world.exe
RAYLIB  = C:/raylib/raylib/src
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -I$(RAYLIB)
LDFLAGS = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,console
export PATH := $(W64);$(PATH)
RUN     = $(TARGET)
else
CC      = gcc
TARGET  = build/super-cesar-world
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
RUN     = ./$(TARGET)
endif

SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c, build/%.o, $(SRC))
HEADERS = $(wildcard include/*.h)

.PHONY: all clean run

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: all
	$(RUN)

clean:
	rm -f build/*.o $(TARGET)
