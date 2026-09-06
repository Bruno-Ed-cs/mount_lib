#ifndef H_MOUNT_UTILITIES
#define H_MOUNT_UTILITIES

#include <stdio.h>

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


//H_MOUNT_UTILITIES
#endif
