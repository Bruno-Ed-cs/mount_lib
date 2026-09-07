#ifndef H_MOUNT_UTILITIES
#define H_MOUNT_UTILITIES

#include <stdio.h>
#include <stdint.h>

typedef unsigned char byte;

inline static void mnt_util_print_mem(byte* begin, unsigned long size) {

    printf("Memory block at Ox%08X of length: %lu:\n", begin, size);
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
