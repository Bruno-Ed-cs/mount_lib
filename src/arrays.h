#ifndef H_MOUNT_ARRAYS
#define H_MOUNT_ARRAYS

#ifndef MNT_DONT_INCLUDE

#include "arenas.h"
#include "utilities.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
    byte* data;

} Mnt_Darray;

typedef struct {

    int index;
    Mnt_Array_Header* head;
    void* data;

} Mnt_Array_Iterator;

//dynamic array
#define MNT_DARRAY_DEFAULT_CAPACITY 50

#define mnt_darray_make(type, init_capacity) _mnt_darray_make(sizeof(type), (init_capacity))
Mnt_Darray* _mnt_darray_make(size_t data_size, size_t init_capacity);
Mnt_Darray* mnt_darray_clone(Mnt_Darray* self);

void _mnt_darray_grow(Mnt_Darray* self, size_t addition);
void _mnt_darray_reduce(Mnt_Darray* self, size_t subtraction);
void mnt_darray_free(Mnt_Darray* self);
void mnt_darray_reserve(Mnt_Darray* self, size_t size);
void mnt_darray_shrink(Mnt_Darray* self);

void mnt_darray_append(Mnt_Darray* self, void* src);
void mnt_darray_insert(Mnt_Darray* self, void* src, size_t index);
void mnt_darray_pop(Mnt_Darray* self, void* dest);
void mnt_darray_pop_front(Mnt_Darray* self, void* dest);
void mnt_darray_remove(Mnt_Darray* self, size_t index);

void mnt_darray_get(Mnt_Darray* self, void* dest, size_t index);
void mnt_darray_set(Mnt_Darray* self, void* src, size_t index);

//static array

#define mnt_array_make(type, size, arena) _mnt_array_make(sizeof(type), (size), (arena))
#define mnt_array_len(array) mnt_array_header((array))->lenght
void* _mnt_array_make(size_t data_size, size_t n, Mnt_Arena* arena);

Mnt_Array_Header* mnt_array_header(void* array);
void* mnt_array_realloc(void* array, size_t new_size);
void* mnt_array_clone(void* array);
void mnt_array_free(void* array);

//if the arena is null the array is allocated with malloc

Mnt_Array_Iterator mnt_array_get_iterator(void* array);
bool mnt_array_iterate(Mnt_Array_Iterator* iter);


//H_MOUNT_ARRAYS
#endif

