#include "arenas.h"
#include <stdio.h>
#include <stdint.h>

void* mnt_arena_realloc(void* mem_begin, size_t new_size, Mnt_Arena* arena) {

    void* allocated = NULL;

    switch (arena->type) {
        case MNT_PAGED_ARENA:
            //TODO: Implement the paged arena

        break;

        case MNT_STATIC_ARENA:

            allocated = mnt_static_arena_realloc(mem_begin, new_size, &arena->static_arena);

        break;

    }

    return allocated;
}

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

    mnt_free_list_reset(&arena.reallocations);

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
    size_t true_size = size + sizeof(Mnt_Allocation_Header);

    if ((arena->pos + true_size) >= (arena->buffer + arena->size)) {
        fprintf(stderr, "The current arena cannot allocate %lu bytes, only %lu remain\n", 
                size,
                arena->pos - arena->buffer);
        return NULL;
    }

    byte* allocated = arena->pos + sizeof(Mnt_Allocation_Header);
    Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)arena->pos;
    head->size = size;
    head->check = MNT_ALLOC_KEY ^ size;
    arena->pos += true_size;
    return allocated;
}

int mnt_static_arena_reset(Mnt_Static_Arena* arena) {
    byte* cur = arena->pos;
    int delta = (int)(cur - (byte*)arena->pos);
    arena->pos = arena->buffer;

    return delta;
}

void* mnt_static_arena_realloc(void* mem_begin, size_t new_size, Mnt_Static_Arena* arena) {

}

void  mnt_free_list_add(Mnt_Free_List* self, byte* pos, size_t size) {

    if (self->cur_entry == self->end) {
        self->list[self->cur_entry].pos = pos;
        self->list[self->cur_entry].size = size;

        return;
    }

    self->cur_entry++;
    self->list[self->cur_entry].pos = pos;
    self->list[self->cur_entry].size = size;

    return;
}

byte* mnt_free_list_get_best_fit(Mnt_Free_List* self, size_t size) {

    byte* result = NULL;
    size_t i_result = 0;

    for (size_t i = 0; i <= self->end; i++) {
        if (self->list[i].size >= size) {
            i_result = i;
            result = self->list[i].pos;
            break;
        }

    }

    if (result != NULL) {

        if (self->list[i_result].size == size) {
            mnt_free_list_remove(self, i_result);
        } else {
            self->list[i_result].pos += size;
            self->list[i_result].size -= size;
        }

    }

    return result;
}

void  mnt_free_list_remove(Mnt_Free_List* self, size_t index) {

    for (size_t i = index; i < self->end; i++) {
        self->list[i] = self->list[i + 1];
    }

    self->end = self->end == 0 ? 0 : self->end--;
    self->cur_entry = self->cur_entry == 0 ? 0 : self->cur_entry--;
}

void  mnt_free_list_reset(Mnt_Free_List* self) {

    for (size_t i = 0; i < MNT_MAX_FREE_LIST; i++) {
        self->list[i].size = 0;
        self->list[i].pos = NULL;
    }
    self->end = 0;
    self->cur_entry = 0;
}

