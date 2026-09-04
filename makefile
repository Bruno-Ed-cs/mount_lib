LIB_FILES = arrays.c
OBJECT_FILES = $(LIB_FILES:.c=.o)
CC = gcc
RELEASE_FLAGS = -O2 -fPIC -std=c11 -shared
DEBUG_FLAGS = -ggdb

all: tests

tests: build 
	$(CC) $(DEBUG_FLAGS) -o build/tests src/$(LIB_FILES) src/tests.c

release: build build/$(OBJECT_FILES)
	mkdir -p build/headers
	cp -r src/*.h build/headers/
	ar rcs build/mount.a build/$(OBJECT_FILES)

build/$(OBJECT_FILES): build 
	$(foreach source, $(LIB_FILES), $(CC) -c $(RELEASE_FLAGS) -o build/$(source:.c=.o) src/$(source))

build:
	mkdir -p build/

clean:
	rm build/*
