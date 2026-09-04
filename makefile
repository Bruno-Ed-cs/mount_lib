LIB_FILES = arrays.c
CC = gcc
RELEASE_FLAGS = -O2 -fPIC -shared
DEBUG_FLAGS = -ggdb

all: tests

tests: build
	$(CC) $(DEBUG_FLAGS) -o build/tests src/$(LIB_FILES) src/tests.c

release: build

objects: build $(LIB_FILES)

build:
	mkdir -p build/

clean:
	rm build/*
