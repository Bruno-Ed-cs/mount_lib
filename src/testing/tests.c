#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "../arrays.h"
#include "../arenas.h"
#include "../logging.h"

bool create_array() {

    float* temps = mnt_array_make(float, 10);

    if (temps == NULL)
        return false;

    for (size_t i = 0; i < mnt_array_len(temps); i++) {
        if (temps[i] != 0) {

            return false;
        }
    }

    mnt_array_free(temps);

    return true;
}

bool modify_array() {

    float* temps = mnt_array_make(float, 10);
    float target_temps[] = {29.3, 11, 43, 29, 69, 100, 385.2, 10, 88, 2};

    for (size_t i = 0; i < mnt_array_len(temps); i++) {
        temps[i] = target_temps[i];
    }

    for (size_t i = 0; i < mnt_array_len(temps); i++) {
        if (temps[i] != target_temps[i]) {
            return false;
        }
    }

    return true;
}

bool clone_array() {
    float* temps = mnt_array_make(float, 10);
    float target_temps[] = {29.3, 11, 43, 29, 69, 100, 385.2, 10, 88, 2};

    for (size_t i = 0; i < mnt_array_len(temps); i++) {
        temps[i] = target_temps[i];
    }

    float* temp_clone = mnt_array_clone(temps);

    assert(mnt_array_len(temp_clone) == mnt_array_len(temps) && "The clone does not have the same length");

    for (size_t i = 0; i < mnt_array_len(temp_clone); i++) {
        if (temp_clone[i] != temps[i])
            return false;
    }

    return true;
}

bool iterate_array() {

    float* temps = mnt_array_make(float, 10);
    float target_temps[] = {29.3, 11, 43, 29, 69, 100, 385.2, 10, 88, 2};

    for (size_t i = 0; i < mnt_array_len(temps); i++) {
        temps[i] = target_temps[i];
    }

    Mnt_Array_Iterator it = mnt_array_get_iterator(temps);
    mnt_log(MNT_INFO, "Array iterator output:\n\n");
    mnt_log(MNT_INFO, "iterator index = %d\n", it.index);
    while (mnt_array_iterate(&it)) {
        mnt_log(MNT_INFO, "iterator index = %d\n", it.index);
        float* temp = it.data;
        mnt_log(MNT_INFO, "index: %f , data: %f\n", temps[it.index], *temp);

        if (*temp != temps[it.index])
            return false;

    }

    return true;

}

bool static_arena_tests() {
    byte buffer[128];

    Mnt_Arena stack_arena = mnt_static_arena_make(128, buffer);
    Mnt_Arena heap_arena = mnt_static_arena_make(128, NULL);

    int* num_heap = mnt_arena_alloc(sizeof(int), &heap_arena);
    int* num_stack = mnt_arena_alloc(sizeof(int), &stack_arena);

    mnt_util_print_mem(heap_arena.static_arena.buffer, heap_arena.static_arena.size);
    mnt_util_print_mem(stack_arena.static_arena.buffer, stack_arena.static_arena.size);

    if (*num_heap != 0) {
        mnt_log(MNT_INFO, "num 1 = %d\n", *num_heap);
        mnt_log(MNT_INFO, "num 1 = %d\n", *num_heap);
        return false;
    }

    if (*num_stack != 0) {
        mnt_log(MNT_INFO, "num 2 = %d\n", *num_heap);
        mnt_log(MNT_INFO, "num 2 = %d\n", *num_stack);
        return false;
    }

    char* name = mnt_arena_alloc(22, &heap_arena);
    if ((uintptr_t)name != mnt_resolve_alingment((uintptr_t)name)){
        mnt_log(MNT_ERROR, "Memory not alingned " __FILE__ ": %d\n", __LINE__);
        return 1;
    }
    mnt_log(MNT_INFO, "name header {\nsize = %lu\n}\n", mnt_get_header(name).size);
    memcpy(name, "Bruno", 6);

    mnt_util_print_mem(heap_arena.static_arena.buffer, heap_arena.static_arena.size);
    mnt_log(MNT_INFO, "%d num_heap\n", *num_heap);
    mnt_log(MNT_INFO, "%s name\n", name);

    if (strcmp(name, "Bruno") != 0) {
        return false;
    }

    Mnt_Allocation_Header* head = (Mnt_Allocation_Header*)(name - sizeof(Mnt_Allocation_Header));
    Mnt_Allocation_Header* false_head = (Mnt_Allocation_Header*)(name);
    if (!mnt_validate_header(*head)){
        mnt_log(MNT_INFO, "The header is invalid\n");
        return false;
    }

    if (mnt_validate_header(*false_head)) {
        mnt_log(MNT_INFO, "The false head is valid\n");
        return false;
    }

    mnt_arena_free_all(&heap_arena);

    name = mnt_arena_alloc(22, &heap_arena);
    if ((uintptr_t)name != mnt_resolve_alingment((uintptr_t)name)){
        mnt_log(MNT_ERROR, "Memory not alingned " __FILE__ ": %d\n", __LINE__);
        return 1;
    }
    mnt_log(MNT_INFO, "name header {\nsize = %lu\n}\n", mnt_get_header(name).size);
    memcpy(name, "Bruno", 6);

    char* full_name = mnt_arena_realloc(name, 28, &heap_arena);
    full_name = mnt_arena_realloc(full_name, 28, &heap_arena);
    //
    // ids[0] = 1; ids[1] = 0xFF;
    //
    //
    // printf("Ids = %d, %d\n", ids[0], ids[1]);
    // printf("realloc name: %s\n", full_name);
    //
    // if (ids[0] != 1 || ids[1] != 0xFF)
    //     return false;
    //
    for (size_t i = 0; i < 10; i++) {
        int* nums = mnt_arena_alloc(sizeof(int) * 2, &heap_arena);
        if ((uintptr_t)nums != mnt_resolve_alingment((uintptr_t)nums)){
            mnt_log(MNT_ERROR, "Memory not alingned " __FILE__ ": %d\n", __LINE__);
            return 1;
        }

        if (nums != NULL) {
            nums[0] = 0xFFFFFFFF;
            nums[1] = 0xFFFFFFFF;
        }
    }

    mnt_util_print_mem(heap_arena.static_arena.buffer, heap_arena.static_arena.size);

    mnt_arena_delete(heap_arena);
    return true;
}

inline static char* passed(bool test_result) {
    if (test_result)
        return "PASSED";
    else
        return "FAILED";

}

int main(int argc, char** argv) {

    char* log_path = "/tmp/mount_log.txt";

    if (argc > 1) {

        log_path = argv[1];
    }

    FILE* log = fopen(log_path, "a");
    if (log == NULL) {
        perror("Could not open the log file");
        return 1;
    }

    time_t now = time(NULL);

    fprintf(log, "\nTests Mount lib:\n%s\n"
            "---------------------------------------------------------------------\n",
            ctime(&now));

    fprintf(log, "Create array test:\n\t%s\n", passed(create_array()));

    fprintf(log, "Modify array test:\n\t%s\n", passed(modify_array()));

    fprintf(log, "Clone array test:\n\t%s\n", passed(clone_array()));

    fprintf(log, "Iterate array test:\n\t%s\n", passed(iterate_array()));

    fprintf(log, "Create arena test:\n\t%s\n", passed(static_arena_tests()));

    return 0;

}

