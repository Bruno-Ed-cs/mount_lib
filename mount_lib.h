#ifndef H_MOUNT_LIB
#define H_MOUNT_LIB

#define MNT_DONT_INCLUDE

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef H_MOUNT_UTILITIES
#define H_MOUNT_UTILITIES

#ifndef MNT_DONT_INCLUDE

#include <stdio.h>
#include <stdint.h>

#endif

typedef uint8_t byte;

inline static void mnt_util_print_mem(byte* begin, unsigned long size) {

    printf("Memory block at Ox%p of length: %lu:\n", begin, size);
    puts("--------------------------------------------------------------------------");
    for (size_t i = 0; i < size; i++) {
        if (i % 10 == 0) {
            puts("\n");
        }

        printf("Ox%02X ", (unsigned char)begin[i]);
    }
    puts("\n");
    puts("--------------------------------------------------------------------------\n");

};

inline static size_t mnt_mb(size_t amount) {
    return (1024 * 2) * amount;
}

inline static size_t mnt_kb(size_t amount) {
    return 1024 * amount;
}

inline static size_t mnt_gb(size_t amount) {
    return (1024 * 3) * amount;
}

inline static uintptr_t mnt_resolve_alingment(uintptr_t ptr) {


    const uintptr_t aling = 2 * sizeof(void*);
    uintptr_t chunk_alingment = ptr % aling;

    if (chunk_alingment != 0) {
        ptr = (ptr / aling + 1) * aling;
    }

    return ptr;

    //Get the next align adress


}


//H_MOUNT_UTILITIES
#endif

#ifndef H_MOUNT_ARENAS
#define H_MOUNT_ARENAS

#ifndef MNT_DONT_INCLUDE

#include "utilities.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif

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

inline static Mnt_Allocation_Header mnt_get_header(void* ptr) {
    Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)((byte*)ptr - sizeof(Mnt_Allocation_Header));

    return *head;
}

typedef struct {

    size_t end;

    struct {
        byte* pos;
        size_t size;
    } list[MNT_MAX_FREE_LIST];

} Mnt_Free_List;

size_t mnt_free_list_find(Mnt_Free_List* self, byte* target);
void  mnt_free_list_add(Mnt_Free_List* self, byte* pos, size_t size);
byte* mnt_free_list_get_best_fit(Mnt_Free_List* self, size_t size);

void  mnt_free_list_remove(Mnt_Free_List* self, size_t index);
void  mnt_free_list_reset(Mnt_Free_List* self);
void mnt_free_list_update(Mnt_Free_List* self, byte* target, size_t full_size);

typedef struct {
    byte* buffer;
    byte* pos;
    size_t size;
    Mnt_Free_List free_list;

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

//static arena functions
Mnt_Arena mnt_static_arena_make(size_t size, char* backing_buffer);
void* mnt_static_arena_alloc(size_t size, Mnt_Static_Arena* arena);
int mnt_static_arena_reset(Mnt_Static_Arena* arena);
void* mnt_static_arena_realloc(void* mem_begin, size_t new_size, Mnt_Static_Arena* arena);

//Arena interfaces
void mnt_arena_delete(Mnt_Arena arena);
int mnt_arena_free_all(Mnt_Arena* arena);
void* mnt_arena_alloc(size_t size, Mnt_Arena* arena);
void* mnt_arena_realloc(void* mem_begin, size_t new_size, Mnt_Arena* arena);

//H_MOUNT_ARENAS
#endif 


#ifndef H_MOUNT_ARRAYS
#define H_MOUNT_ARRAYS

#ifndef MNT_DONT_INCLUDE

#include "arenas.h"
#include "utilities.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#endif

typedef struct {

    size_t data_size;
    size_t lenght;
    Mnt_Arena* arena;
    byte data[];

} Mnt_Array_Header;

typedef struct {

    size_t data_size;
    size_t lenght;
    size_t capacity;
    Mnt_Arena* arena;
    byte data[];

} Mnt_Darray_Header;

typedef struct {

    int index;
    Mnt_Array_Header* head;
    void* data;

} Mnt_Array_Iterator;

void* _mnt_array_make_implementation(size_t data_size, size_t n, Mnt_Arena* arena);

Mnt_Array_Header* mnt_array_header(void* array);
void* mnt_array_realloc(void* array, size_t new_size);
void* mnt_array_clone(void* array);
void mnt_array_free(void* array);

//if the arena is null the array is allocated with malloc
#define mnt_array_make(type, size, arena) _mnt_array_make_implementation(sizeof(type), (size), (arena))
#define mnt_array_len(array) mnt_array_header((array))->lenght

Mnt_Array_Iterator mnt_array_get_iterator(void* array);
bool mnt_array_iterate(Mnt_Array_Iterator* iter);


//H_MOUNT_ARRAYS
#endif


#ifndef H_MOUNT_LOGGING
#define H_MOUNT_LOGGING

#ifndef MNT_DONT_INCLUDE

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#endif

typedef enum {

    MNT_INFO = 0,
    MNT_DEBUG,
    MNT_ERROR, 
    MNT_FATAL,
    MNT_NONE

} Mnt_Log_Level;

void mnt_log_message(Mnt_Log_Level log_level, char* message, char* file, int line, ...);
void mnt_log_set_level(Mnt_Log_Level log_level);
void mnt_log_set_output(FILE* output);

#define mnt_log(log_level, message, ...) mnt_log_message((log_level), (message), __FILE__, __LINE__, ##__VA_ARGS__)

//H_MOUNT_LOGGING
#endif

#ifdef MOUNT_LIB_IMPLEMENTATION

//logging.c
// #include "logging.h"

Mnt_Log_Level min_logging = MNT_INFO;
FILE* log_output = NULL;

void mnt_log_message(Mnt_Log_Level log_level, char* message, char* file, int line, ...) {

    if (log_output == NULL) {
        log_output = stdout;
    }

    if (log_level >= min_logging) {

        switch (log_level) {

            case MNT_INFO:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;97;160;255m");

                fprintf(log_output, "INFO");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_DEBUG:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;204;255;44m");

                fprintf(log_output, "DEBUG");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_ERROR:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;255;0;90m");

                fprintf(log_output, "ERROR");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_FATAL:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;255;0;0m");

                fprintf(log_output, "FATAL");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

        }

        va_list args;
        va_start(args, line);
        
        vfprintf(log_output, message, args);
        fprintf(log_output, "\n");

        va_end(args);

    }

}

void mnt_set_log_level(Mnt_Log_Level log_level) {

    min_logging = log_level;

}

void mnt_set_log_output(FILE* output) {

    log_output = output;

}

// arrays.c
// #include "arrays.h"
// #include "arenas.h"
// #include "logging.h"

void* _mnt_array_make_implementation(size_t data_size, size_t n, Mnt_Arena* arena) {

    Mnt_Array_Header header = {
        .lenght = n,
        .data_size = data_size,
        .arena = arena
        
    };

    Mnt_Array_Header* array = NULL;

    if (arena) {

        array = mnt_arena_alloc(sizeof(Mnt_Array_Header) + (header.data_size * n), arena);

    } else {

        array = malloc(sizeof(Mnt_Array_Header) + (header.data_size * n));
    }

    if (!array) {

        mnt_log(MNT_ERROR, "Failure allocating array");
        return array;
    }

    array[0] = header;
    byte* first_element = (void*)((byte*)array + sizeof(Mnt_Array_Header));

    for (size_t i = 0; i < n * data_size; i++) {
        first_element[i] = 0;
    }

    return (void*)first_element;

}

inline Mnt_Array_Header* mnt_array_header(void* array) {
    Mnt_Array_Header* header = (Mnt_Array_Header*)((byte*)array - sizeof(Mnt_Array_Header));
    return header;
}

void* mnt_array_realloc(void* array, size_t new_size) {

    Mnt_Array_Header* head = mnt_array_header(array);
    void* new_array = mnt_arena_realloc(head, new_size * head->data_size + sizeof(Mnt_Array_Header), head->arena);

    return new_array;

}

void* mnt_array_clone(void* array) {

    Mnt_Array_Header* header = mnt_array_header(array);
    void* new_array = _mnt_array_make_implementation(header->data_size, header->lenght, header->arena);

    size_t size = header->data_size * header->lenght + sizeof(Mnt_Array_Header);

    memcpy(mnt_array_header(new_array), header, size);

    return new_array;
}

void mnt_array_free(void* array) {

    Mnt_Array_Header* head = mnt_array_header(array);

    if (head->arena == NULL){
        free(head);
    }
}

Mnt_Array_Iterator mnt_array_get_iterator(void* array) {

    Mnt_Array_Header* header = mnt_array_header(array);

    return (Mnt_Array_Iterator){
        .head = header,
        .index = 0,
        .data = NULL
    };

}

bool mnt_array_iterate(Mnt_Array_Iterator* iter) {

    if (iter->index >= iter->head->lenght)
        return false;

    if (iter->data == NULL) {
        iter->data = &iter->head[1];

    } else {

        iter->index++;
        iter->data = ((byte*)iter->head + sizeof(Mnt_Array_Header)) + (iter->head->data_size * iter->index);
    }

    return true;

}

// void util_append_darray(void* darray, void* data) {
//
//     DArrayHeader* head = util_header_darray(darray);
//
//     if (head->lenght >= head->capacity) {
//
//         util_grow_darray(darray, head->capacity / 2 + 1);
//     }
//
//     byte* target = (byte*)darray + (head->lenght * head->data_size);
//     memcpy(target, data, head->data_size);
//     head->lenght++;
//     return;
// }
//
// void util_remove_darray(void* darray, size_t index) {
//
//     DArrayHeader* head = util_header_darray(darray);
//
//     bool ran = false;
//
//     for (int i = index; i < head->lenght; i++) {
//
//         byte* dest = (byte*)darray + (i * head->data_size);
//         byte* target = (byte*)darray + ((i + 1) * head->data_size);
//         memcpy(dest, target, head->data_size);
//
//         ran = true;
//     }
//
//     if(ran) head->lenght--;
//
//     return;
// }
//
// void util_insert_darray(void* darray, size_t index, void* data, size_t size) {
//
//     DArrayHeader* head = util_header_darray(darray);
//
//     bool ran = false;
//
//     for (int i = index; i < head->lenght; i++) {
//
//         byte* dest = (byte*)darray + (i * head->data_size);
//         byte* target = (byte*)darray + ((i + 1) * head->data_size);
//         memcpy(dest, target, head->data_size);
//
//         ran = true;
//     }
//
//     if(ran) head->lenght--;
//
//     return;
// }
//
//
// void util_srink_darray(void* darray, size_t decrement) {
//
//     DArrayHeader* head = util_header_darray(darray);
//
//     if (decrement > head->capacity)
//         decrement = head->capacity;
//
//     darray = realloc(head, (head->capacity - decrement) * head->data_size + sizeof(DArrayHeader));
//     head->capacity -= decrement;
//
//     if(head->capacity < head->lenght)
//         head->lenght = head->capacity;
// }
//
//
// DArrayIterator util_get_it_darray(void* darray) {
//
//     DArrayIterator iter = {
//         .head = util_header_darray(darray),
//         .index = -1};
//
//     return iter;
// }
//
// bool util_it_darray(DArrayIterator* iter) {
//
//     if (iter->index >= (int)iter->head->lenght -1) return false;
//
//     iter->index++;
//     return true;
//
// }
//
//

// arenas.c
// #include "arenas.h"
// #include "logging.h"

void* mnt_arena_realloc(void* mem_begin, size_t new_size, Mnt_Arena* arena) {

    void* allocated = NULL;

    if (arena == NULL) {
        mnt_log(MNT_ERROR, "No arena provided");
        return NULL;

    }

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

    if (arena == NULL) {
        mnt_log(MNT_ERROR, "No arena provided");
        return NULL;
    }

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

    if (arena == NULL) {
        mnt_log(MNT_ERROR, "No arena provided");
        return 0;
    }

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

    mnt_free_list_reset(&arena.free_list);

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

    byte* result = mnt_free_list_get_best_fit(&arena->free_list, true_size);
    // printf("Result: %d\n", result);

    if (result != NULL) {

        byte* aling_result = (byte*)mnt_resolve_alingment((uintptr_t)result);

        if ((size_t)(aling_result + true_size) <= mnt_free_list_find(&arena->free_list, result)) {
            mnt_free_list_update(&arena->free_list, result, true_size + (result - aling_result));

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
    mnt_free_list_reset(&arena->free_list);

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

    byte* pos = mnt_free_list_get_best_fit(&arena->free_list, true_size);
    uintptr_t aling_result = mnt_resolve_alingment((uintptr_t)pos);

    if (pos != NULL && (size_t)(aling_result + true_size) <= mnt_free_list_find(&arena->free_list, pos)) {

        mnt_free_list_update(&arena->free_list, pos, true_size + ((uintptr_t) pos - aling_result));

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
            mnt_free_list_add(&arena->free_list, (byte*)head, head->size + sizeof(Mnt_Allocation_Header));
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


//MOUNT_LIB_IMPLEMENTATION
#endif

//H_MOUNT_LIB
#endif
