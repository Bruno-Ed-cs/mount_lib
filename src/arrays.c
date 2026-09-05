#include "arrays.h"
#include <stdlib.h>
#include <string.h>

void* mnt_array_make_canonical(size_t data_size, size_t n) {

    Mnt_Array_Header header = {
        .lenght = n,
        .data_size = data_size
    };

    Mnt_Array_Header* array = malloc(sizeof(Mnt_Array_Header) + (header.data_size * n));
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
    void* new_array = realloc(head, new_size * head->data_size + sizeof(Mnt_Array_Header));

    return new_array;

}

void* mnt_array_clone(void* array) {

    Mnt_Array_Header* header = mnt_array_header(array);
    void* new_array = mnt_array_make_canonical(header->data_size, header->lenght);

    size_t size = header->data_size * header->lenght + sizeof(Mnt_Array_Header);

    memcpy(mnt_array_header(new_array), header, size);

    return new_array;
}

void mnt_array_free(void* array) {

    Mnt_Array_Header* head = mnt_array_header(array);

    free(head);
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
