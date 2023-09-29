## DEBUG 
DEBUG_FLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0)
## RELEASE
RELEASE_FLAGS = -O3 $(shell pkg-config --cflags gtk+-3.0)

LDFLAGS = $(shell pkg-config --libs gtk+-3.0) -lavformat -lavcodec -lavutil -lswscale

DEBUG_RUN_COMMAND = bin/debug/main
RELEASE_RUN_COMMAND = bin/release/main

SOURCES = $(wildcard src/*.c)
DEBUG_OBJECTS = $(patsubst src/%.c, bin/debug/%.o, $(SOURCES))
RELEASE_OBJECTS = $(patsubst src/%.c, bin/release/%.o, $(SOURCES))
DEBUG_EXECUTABLE = bin/debug/main
RELEASE_EXECUTABLE = bin/release/main

all: debug release

debug: $(DEBUG_EXECUTABLE)

$(DEBUG_EXECUTABLE): $(DEBUG_OBJECTS)
	gcc $(DEBUG_OBJECTS) -o $(DEBUG_EXECUTABLE) $(LDFLAGS)

bin/debug/%.o: src/%.c
	gcc -c -std=c11 $(DEBUG_FLAGS) $< -o $@

release: $(RELEASE_EXECUTABLE)

$(RELEASE_EXECUTABLE): $(RELEASE_OBJECTS)
	gcc $(RELEASE_OBJECTS) -o $(RELEASE_EXECUTABLE) $(LDFLAGS)

bin/release/%.o: src/%.c
	gcc -c -std=c11 $(RELEASE_FLAGS) $< -o $@

clean:
	rm -f $(DEBUG_EXECUTABLE) $(DEBUG_OBJECTS) $(RELEASE_EXECUTABLE) $(RELEASE_OBJECTS)

run-debug: $(DEBUG_EXECUTABLE)
	$(DEBUG_RUN_COMMAND)

run-release: $(RELEASE_EXECUTABLE)
	$(RELEASE_RUN_COMMAND)
