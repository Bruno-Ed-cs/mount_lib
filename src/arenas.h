#ifndef H_MOUNT_ARENAS
#define H_MOUNT_ARENAS

#include <stdlib.h>
#include "utilities.h"

typedef struct {
    byte* buffer;
    byte* pos;
    size_t size;

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
void mnt_static_arena_delete(Mnt_Static_Arena arena);
void* mnt_static_arena_alloc(size_t size, Mnt_Static_Arena* arena);
int mnt_static_arena_reset(Mnt_Static_Arena* arena);

void* mnt_arena_alloc(size_t size, Mnt_Arena* arena);
int mnt_arena_free(void* mem_block, Mnt_Arena* arena);

//H_MOUNT_ARENAS
#endif 
