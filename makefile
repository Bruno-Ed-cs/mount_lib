CC = gcc
INCLUDE_PATH = -I./src/
RELEASE_FLAGS = -O2 -fPIC -std=c11
DEBUG_FLAGS = -O0 -ggdb

LIB_SOURCES = $(filter-out src/testing/tests.c, $(wildcard src/*.c))
OBJECTS = $(LIB_SOURCES:src/%.c=build/%.o)

all: tests

tests: build
	$(CC) $(INCLUDE_PATH) $(DEBUG_FLAGS) -o build/tests src/*.c src/testing/tests.c

release: build $(OBJECTS)
	mkdir -p build/headers
	cp -r src/*.h build/headers/
	ar rcs build/libmount.a $(OBJECTS)
	$(CC) $(INCLUDE_PATH) $(RELEASE_FLAGS) -shared -o build/libmount.so $(OBJECTS)

build/%.o: src/%.c | build
	$(CC) -c $(INCLUDE_PATH) $(RELEASE_FLAGS) -o $@ $<

build:
	mkdir -p build/

clean:
	rm -rf build
