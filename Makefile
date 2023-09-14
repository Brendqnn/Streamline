CFLAGS = -Wall -g -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lavformat -lavcodec -lavutil -lswscale
RUN_COMMAND = bin/debug/main.exe

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c, bin/debug/%.o, $(SOURCES))
EXECUTABLE = bin/debug/main.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

bin/debug/%.o: src/%.c
	gcc -c -std=c11 $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

run: $(EXECUTABLE)
	$(RUN_COMMAND)
