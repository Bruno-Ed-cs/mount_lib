#include "arenas.h"
#include <stdio.h>

void* mnt_arena_alloc(size_t size, Mnt_Arena* arena) {

    void* allocated = NULL;

    switch (arena->type) {
        case MNT_PAGED_ARENA:
            //TODO: Implement the paged arena

        break;

        case MNT_STATIC_ARENA:

            allocated = mnt_static_arena_alloc(size, &arena->static_arena);

        break;

    }

    return allocated;
}

int mnt_arena_free_all(Mnt_Arena* arena) {

    int allocated_bytes = 0;

    switch (arena->type) {
        case MNT_PAGED_ARENA:
            //TODO: Implement the paged arena

        break;

        case MNT_STATIC_ARENA:

            allocated_bytes = mnt_static_arena_reset(&arena->static_arena);

        break;

    }

    return allocated_bytes;
}

Mnt_Arena mnt_static_arena_make(size_t size, char* backing_buffer) {

    Mnt_Static_Arena arena;

    if (backing_buffer == NULL) {
        arena.buffer = malloc(size);
    } else {
        arena.buffer = backing_buffer;
    }

    for (size_t i = 0; i < size; i++) {
        arena.buffer[i] = 0;
    }
    arena.pos = arena.buffer;
    arena.size = size;

    return (Mnt_Arena) {
        .static_arena = arena,
        .type = MNT_STATIC_ARENA
    };
}

void mnt_arena_delete(Mnt_Arena arena) {

    switch (arena.type) {
        case MNT_PAGED_ARENA:
            //TODO: Implement the paged arena

        break;

        case MNT_STATIC_ARENA:

            free(arena.static_arena.buffer);

        break;

    }
}

void* mnt_static_arena_alloc(size_t size, Mnt_Static_Arena* arena) {

    if ((arena->pos + size) >= (arena->buffer + arena->size)) {
        fprintf(stderr, "The current arena cannot allocate %lu bytes, only %lu remain\n", 
                size,
                arena->pos - arena->buffer);
        return NULL;
    }

    byte* allocated = arena->pos;
    arena->pos += size;
    return allocated;
}

int mnt_static_arena_reset(Mnt_Static_Arena* arena) {
    byte* cur = arena->pos;
    int delta = (int)(cur - (byte*)arena->pos);
    arena->pos = arena->buffer;

    return delta;
}

