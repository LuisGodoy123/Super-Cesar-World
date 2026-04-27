W64     = C:/raylib/w64devkit/bin
CC      = $(W64)/gcc.exe
TARGET  = build/super-cesar-world.exe
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c, build/%.o, $(SRC))

RAYLIB  = C:/raylib/raylib/src

HEADERS = $(wildcard include/*.h)
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -I$(RAYLIB)
LDFLAGS = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,console

export PATH := $(W64);$(PATH)

.PHONY: all clean run

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: all
	$(TARGET)

clean:
	rm -f build/*.o $(TARGET)
