#ifndef H_MOUNT_ARRAYS
#define H_MOUNT_ARRAYS

#include <stddef.h>
#include <stdbool.h>

typedef struct {

    size_t data_size;
    size_t capacity;
    size_t lenght;


} DArrayHeader;

typedef char byte;

void* util_make_darray(size_t data_size);
DArrayHeader* util_header_darray(void* darray);
void util_append_darray(void* darray, void* data);
void util_remove_darray(void* darray, size_t index);
void util_grow_darray(void* darray, size_t increment);
void util_srink_darray(void* darray, size_t decrement);
void util_free_darray(void* darray);

typedef struct {

    DArrayHeader* head;
    int index;

} DArrayIterator;

DArrayIterator util_get_it_darray(void* darray);
bool util_it_darray(DArrayIterator* iter);


#define util_len(darray) util_header_darray((darray))->lenght
#define util_cap(darray) util_header_darray((darray))->capacity
#define util_data_size(darray) util_header_darray((darray))->data_size

#define util_print(statement, darray) \
    do {\
        for (int i = 0; i < util_len((darray)); i++)\
        printf((statement), i, (darray)[i]);\
        printf("\n");\
    } while(0)\

//H_MOUNT_ARRAYS
#endif

#ifdef MOUNT_ARRAYS_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

void* util_make_darray(size_t data_size) {

    DArrayHeader header = {
        .capacity = 10,
        .lenght = 0,
        .data_size = data_size

    };

    DArrayHeader* array = malloc(sizeof(DArrayHeader) + (header.data_size * header.capacity));
    array[0] = header;

    return (void*)((byte*)array + sizeof(DArrayHeader));

}

inline DArrayHeader* util_header_darray(void* darray) {
    DArrayHeader* header = (DArrayHeader*)((byte*)darray - sizeof(DArrayHeader));
    return header;
}


void util_append_darray(void* darray, void* data) {

    DArrayHeader* head = util_header_darray(darray);

    if (head->lenght >= head->capacity) {

        util_grow_darray(darray, head->capacity / 2 + 1);
    }

    byte* target = (byte*)darray + (head->lenght * head->data_size);
    memcpy(target, data, head->data_size);
    head->lenght++;
    return;
}

void util_remove_darray(void* darray, size_t index) {

    DArrayHeader* head = util_header_darray(darray);

    bool ran = false;

    for (int i = index; i < head->lenght; i++) {

        byte* dest = (byte*)darray + (i * head->data_size);
        byte* target = (byte*)darray + ((i + 1) * head->data_size);
        memcpy(dest, target, head->data_size);

        ran = true;
    }

    if(ran) head->lenght--;

    return;
}

void util_insert_darray(void* darray, size_t index, void* data, size_t size) {

    DArrayHeader* head = util_header_darray(darray);

    bool ran = false;

    for (int i = index; i < head->lenght; i++) {

        byte* dest = (byte*)darray + (i * head->data_size);
        byte* target = (byte*)darray + ((i + 1) * head->data_size);
        memcpy(dest, target, head->data_size);

        ran = true;
    }

    if(ran) head->lenght--;

    return;
}

void util_grow_darray(void* darray, size_t increment) {

    DArrayHeader* head = util_header_darray(darray);

    darray = realloc(head, (head->capacity + increment) * head->data_size + sizeof(DArrayHeader));
    head->capacity += increment;

}

void util_srink_darray(void* darray, size_t decrement) {

    DArrayHeader* head = util_header_darray(darray);

    if (decrement > head->capacity)
        decrement = head->capacity;

    darray = realloc(head, (head->capacity - decrement) * head->data_size + sizeof(DArrayHeader));
    head->capacity -= decrement;

    if(head->capacity < head->lenght)
        head->lenght = head->capacity;
}

void util_free_darray(void* darray) {

    DArrayHeader* head = util_header_darray(darray);

    free(head);
}

DArrayIterator util_get_it_darray(void* darray) {

    DArrayIterator iter = {
        .head = util_header_darray(darray),
        .index = -1};

    return iter;
}

bool util_it_darray(DArrayIterator* iter) {

    if (iter->index >= (int)iter->head->lenght -1) return false;

    iter->index++;
    return true;

}


//DMOUNT_ARRAYS_IMPLEMENTATION
#endif
