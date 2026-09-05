#ifndef H_MOUNT_ARENAS
#define H_MOUNT_ARENAS

#include <stdlib.h>
#include <stdbool.h>
#include "utilities.h"

#define MNT_MAX_FREE_LIST 10
#define MNT_ALLOC_KEY 0x690731F3

typedef struct {
    size_t check;
    size_t size;

} Mnt_Allocation_Header;

inline static bool mnt_validate_header(Mnt_Allocation_Header head) {
    size_t validator = head.check ^ head.size;
    if (validator == MNT_ALLOC_KEY) 
        return true;
    else
        return false;

}

typedef struct {

    size_t end;
    size_t cur_entry;

    struct {
        byte* pos;
        size_t size;
    } list[MNT_MAX_FREE_LIST];

} Mnt_Free_List;

void  mnt_free_list_add(Mnt_Free_List* self, byte* pos, size_t size);
byte* mnt_free_list_get_next(Mnt_Free_List* self, size_t size);

void  mnt_free_list_remove(Mnt_Free_List* self, size_t index);
void  mnt_free_list_reset(Mnt_Free_List* self);

typedef struct {
    byte* buffer;
    byte* pos;
    size_t size;
    Mnt_Free_List reallocations;

} Mnt_Static_Arena;

typedef struct {


} Mnt_Paged_Arena;

typedef struct {

    enum {
        MNT_STATIC_ARENA,
        MNT_PAGED_ARENA

    } type;

    union {
        Mnt_Static_Arena static_arena;
        Mnt_Paged_Arena paged_arena;
    };


} Mnt_Arena;

Mnt_Arena mnt_static_arena_make(size_t size, char* backing_buffer);
void* mnt_static_arena_alloc(size_t size, Mnt_Static_Arena* arena);
int mnt_static_arena_reset(Mnt_Static_Arena* arena);
void* mnt_static_arena_realloc(void* mem_begin, size_t new_size, Mnt_Static_Arena* arena);

void mnt_arena_delete(Mnt_Arena arena);
void* mnt_arena_alloc(size_t size, Mnt_Arena* arena);
void* mnt_arena_realloc(void* mem_begin, size_t new_size, Mnt_Arena* arena);
int mnt_arena_free(void* mem_block, Mnt_Arena* arena);

//H_MOUNT_ARENAS
#endif 
