#include "arenas.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

    byte* result = mnt_free_list_get_best_fit(&arena->reallocations, true_size);
    printf("Result: %d\n", result);

    if (result != NULL) {

        byte* allocated = result + sizeof(Mnt_Allocation_Header);
        Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)result;
        head->size = size;
        head->check = MNT_ALLOC_KEY ^ size;
        return allocated;
    }

    if ((arena->pos + true_size) >= (arena->buffer + arena->size)) {
        fprintf(stderr, "The current arena cannot allocate %lu bytes, only %lu remain\n", 
                true_size,
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
    mnt_free_list_reset(&arena->reallocations);

    return delta;
}

void* mnt_static_arena_realloc(void* mem_begin, size_t new_size, Mnt_Static_Arena* arena) {

    if ((byte*)mem_begin <= arena->buffer || (byte*)mem_begin >= arena->buffer + arena->size) {
        fprintf(stderr, "Error, invalid realocation: Pointer outside arenas domain\n");
        return NULL;
    }

    Mnt_Allocation_Header* head = mem_begin - sizeof(Mnt_Allocation_Header);

    if ((byte*)head <= arena->buffer || (byte*)head >= arena->buffer + arena->size) {
        fprintf(stderr, "Error, invalid realocation: Pointer does not belong to a alocation\n");
        return NULL;
    }

    bool valid = mnt_validate_header(*head);
    size_t full_size = new_size + sizeof(Mnt_Allocation_Header);

    byte* pos = mnt_free_list_get_best_fit(&arena->reallocations, full_size);

    mnt_free_list_add(&arena->reallocations, (byte*)head, head->size);

    if (pos == NULL) {
        pos = mnt_static_arena_alloc(new_size, arena);
    } else {
        Mnt_Allocation_Header* new_header = (Mnt_Allocation_Header*)(pos);
        new_header->size = new_size;
        new_header->check = new_size ^ MNT_ALLOC_KEY;
        pos = (byte*)&new_header[1];
    }


    memcpy(pos, mem_begin, head->size);

    return pos;

}

void  mnt_free_list_add(Mnt_Free_List* self, byte* pos, size_t size) {

    if (self->cur_entry == MNT_MAX_FREE_LIST) {
        self->list[self->cur_entry -1].pos = pos;
        self->list[self->cur_entry -1].size = size;

        return;
    }

    self->list[self->cur_entry].pos = pos;
    self->list[self->cur_entry].size = size;

    self->cur_entry++;

    return;
}

byte* mnt_free_list_get_best_fit(Mnt_Free_List* self, size_t size) {

    byte* result = NULL;
    size_t i_result = 0;

    for (size_t i = 0; i < MNT_MAX_FREE_LIST; i++) {
        if (self->list[i].size <= size) {
            printf("freelist[%d] = %d\n", i, self->list[i].size);
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

    for (size_t i = index; i < MNT_MAX_FREE_LIST; i++) {
        self->list[i] = self->list[i + 1];
    }

    self->cur_entry = self->cur_entry == 0 ? 0 : self->cur_entry--;
}

void  mnt_free_list_reset(Mnt_Free_List* self) {

    for (size_t i = 0; i < MNT_MAX_FREE_LIST; i++) {
        self->list[i].size = 0;
        self->list[i].pos = NULL;
    }
    self->cur_entry = 0;
}

