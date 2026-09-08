#include "arenas.h"
#include "logging.h"
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

    //Verify current alingment
    

    //Verify if it all fits

    byte* result = mnt_free_list_get_best_fit(&arena->reallocations, true_size);
    // printf("Result: %d\n", result);

    if (result != NULL) {

        byte* aling_result = (byte*)mnt_resolve_alingment((uintptr_t)result);

        if ((size_t)(aling_result + true_size) <= mnt_free_list_find(&arena->reallocations, result)) {
            mnt_free_list_update(&arena->reallocations, result, true_size + (result - aling_result));

            Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)result;
            head->size = size;
            head->check = MNT_ALLOC_KEY ^ size;

            byte* allocated = result + sizeof(Mnt_Allocation_Header);

            return allocated;
        }
    }

    uintptr_t ptr = mnt_resolve_alingment((uintptr_t)arena->pos);
    if ((ptr + true_size) >= (uintptr_t)(arena->buffer + arena->size)) {
        mnt_log(MNT_ERROR, "The current arena cannot allocate %lu bytes from adress %08X, only %lu remain\n", 
                true_size,
                ptr,
                (arena->buffer + arena->size) - arena->pos);
        return NULL;
    }

    Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)ptr;
    head->size = size;
    head->check = MNT_ALLOC_KEY ^ size;

    byte* allocated = (byte*)(ptr + sizeof(Mnt_Allocation_Header));

    mnt_log(MNT_DEBUG, "Allocation head.size = %lu\n", mnt_get_header(allocated).size);

    arena->pos = (byte*)(ptr + true_size);

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
        mnt_log(MNT_ERROR, "Error, invalid realocation: Pointer outside arenas domain: Ox%04X\n", mem_begin);
        return NULL;
    }

    Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)((byte*)mem_begin - sizeof(Mnt_Allocation_Header));

    if ((byte*)head < arena->buffer || (byte*)head >= arena->buffer + arena->size) {
        mnt_log(MNT_ERROR, "Error, invalid realocation: Pointer does not belong to a alocation\n");
        return NULL;
    }

    if (mnt_validate_header(*head) == false) {
        mnt_log(MNT_ERROR, "Error, invalid header\n");
        return NULL;

    }
    mnt_log(MNT_DEBUG, "Rellocation head.size = %lu\n", mnt_get_header(mem_begin).size);
    mnt_log(MNT_DEBUG, "Rellocation head->size = %lu\n", head->size);

    size_t true_size = new_size + sizeof(Mnt_Allocation_Header);

    byte* pos = mnt_free_list_get_best_fit(&arena->reallocations, true_size);
    uintptr_t aling_result = mnt_resolve_alingment((uintptr_t)pos);

    if (pos != NULL && (size_t)(aling_result + true_size) <= mnt_free_list_find(&arena->reallocations, pos)) {

        mnt_free_list_update(&arena->reallocations, pos, true_size + ((uintptr_t) pos - aling_result));

        Mnt_Allocation_Header* new_header = (Mnt_Allocation_Header*)(aling_result);
        new_header->size = new_size;
        new_header->check = new_size ^ MNT_ALLOC_KEY;
        pos = (byte*)aling_result + sizeof(*new_header);

        memcpy(pos, mem_begin, head->size);
        memset(head, 0, head->size + sizeof(Mnt_Allocation_Header));
    }

    if (pos == NULL) {
        pos = mnt_static_arena_alloc(new_size, arena);

        if (pos != NULL) {
            mnt_free_list_add(&arena->reallocations, (byte*)head, head->size + sizeof(Mnt_Allocation_Header));
            memcpy(pos, mem_begin, head->size);
            memset(head, 0, head->size + sizeof(Mnt_Allocation_Header));
        }


    }


    return pos;

}


size_t mnt_free_list_find(Mnt_Free_List* self, byte* target) {

    for (int i = 0; i < self->end; i++) {
        if (target == self->list[i].pos) {
            return self->list[i].size;

            break;
        }
    }

    return 0;

}

void mnt_free_list_update(Mnt_Free_List* self, byte* target, size_t full_size) {

    for (int i = 0; i < self->end; i++) {
        if (target == self->list[i].pos) {
            self->list[i].pos += full_size;
            self->list[i].size -= full_size;
            if (self->list[i].size <= 0 || self->list[i].size <= sizeof(Mnt_Allocation_Header))
                mnt_free_list_remove(self, i);

            break;
        }
    }

}

void  mnt_free_list_add(Mnt_Free_List* self, byte* pos, size_t size) {

    if (self->end >= MNT_MAX_FREE_LIST) {
        return;
    }

    self->list[self->end].pos = pos;
    self->list[self->end].size = size;

    self->end = self->end >= MNT_MAX_FREE_LIST ? MNT_MAX_FREE_LIST -1 : self->end + 1;

    return;
}

byte* mnt_free_list_get_best_fit(Mnt_Free_List* self, size_t size) {

    byte* result = NULL;
    size_t i_result = 0;

    for (size_t i = 0; i < self->end; i++) {
        mnt_log(MNT_INFO, "freelist[%d] = %d, %X\n", i, self->list[i].size, self->list[i].pos);
        if (self->list[i].size >= size) {
            i_result = i;
            result = self->list[i].pos;
            break;
        }

    }

    return result;
}

void  mnt_free_list_remove(Mnt_Free_List* self, size_t index) {

    for (size_t i = index; i < self->end - 1; i++) {
        self->list[i] = self->list[i + 1];
    }
    self->end--;

}

void  mnt_free_list_reset(Mnt_Free_List* self) {

    for (size_t i = 0; i < MNT_MAX_FREE_LIST; i++) {
        self->list[i].size = 0;
        self->list[i].pos = NULL;
    }
    self->end = 0;
}

