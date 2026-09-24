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
    byte data[];

} Mnt_Darray;

typedef struct {

    int index;
    Mnt_Array_Header* head;
    void* data;

} Mnt_Array_Iterator;

//dynamic array
Mnt_Darray* mnt_darray_make(size_t data_size, size_t n);
void _mnt_darray_grow(Mnt_Darray* self, size_t addition);
void _mnt_darray_reduce(Mnt_Darray* self, size_t subtraction);
void mnt_darray_free(Mnt_Darray* self);
void mnt_darray_reserve(Mnt_Darray* self, size_t size);
void mnt_darray_shrink(Mnt_Darray* self);

bool mnt_darray_append(Mnt_Darray* self, void* src);
bool mnt_darray_insert(Mnt_Darray* self, void* src, size_t index);
bool mnt_darray_pop(Mnt_Darray* self, void* dest);
bool mnt_darray_pop_front(Mnt_Darray* self, void* dest);
bool mnt_darray_remove(Mnt_Darray* self, size_t index);

void mnt_darray_get(Mnt_Darray* self, void* dest, size_t index);
void mnt_darray_set(Mnt_Darray* self, void* src, size_t index);

//static array
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

