#ifndef H_MOUNT_ARRAYS
#define H_MOUNT_ARRAYS

#include <stddef.h>
#include <stdbool.h>
#include "arenas.h"
#include "utilities.h"

typedef struct {

    size_t data_size;
    size_t lenght;

} Mnt_Array_Header;

typedef struct {

    int index;
    Mnt_Array_Header* head;
    void* data;

} Mnt_Array_Iterator;

Mnt_Array_Header* mnt_array_header(void* array);
void* mnt_array_make_canonical(size_t data_size, size_t n);
void* mnt_array_realloc(void* array, size_t new_size);
void* mnt_array_clone(void* array);
void mnt_array_free(void* array);

Mnt_Array_Iterator mnt_array_get_iterator(void* array);
bool mnt_array_iterate(Mnt_Array_Iterator* iter);

#define mnt_array_make(type, size) mnt_array_make_canonical(sizeof(type), size)
#define mnt_array_len(array) mnt_array_header((array))->lenght
#define mnt_array_data_size(array) mnt_array_header((array))->data_size

//H_MOUNT_ARRAYS
#endif

#ifdef MOUNT_ARRAYS_IMPLEMENTATION

//DMOUNT_ARRAYS_IMPLEMENTATION
#endif
