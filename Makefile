CC      = C:/raylib/w64devkit/bin/gcc.exe
TARGET  = build/super-cesar-world.exe
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c, build/%.o, $(SRC))

RAYLIB  = C:/raylib/raylib/src

CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -I$(RAYLIB)
LDFLAGS = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,console

.PHONY: all clean run

all: build $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: all
	cmd /c start "" "$(TARGET)"

clean:
	rm -f build/*.o $(TARGET)
