#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "../arrays.h"
#include "../arenas.h"
#include "../logging.h"

bool create_array(Mnt_Arena temp_arena) {

    float* temps = mnt_array_make(float, 10, NULL);
    float* other_temps = mnt_array_make(float, 10, &temp_arena);

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

    float* temps = mnt_array_make(float, 10, NULL);
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
    float* temps = mnt_array_make(float, 10, NULL);
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

    float* temps = mnt_array_make(float, 10, NULL);
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

typedef Mnt_Darray Mnt_Darray_float;

bool dynamic_array() {

    //append tests
    Mnt_Darray_float* temps = mnt_darray_make(float, 0);

    float unit[] = {443.2, 10.0, 84.3, 99.2};

    for (int i = 0; i < sizeof(unit)/sizeof(*unit); i++) {

        mnt_darray_append(temps, &unit[i]);
    }


    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        mnt_log(MNT_INFO, "%.2f, ", holder);

        if (unit[i] != holder) {
            mnt_log(MNT_ERROR, "The data does not match in the array");
            return false;

        }

    }

    //copy tests
    Mnt_Darray_float* temps_clone = mnt_darray_clone(temps);

    if (temps_clone->data == temps->data) {

        mnt_log(MNT_ERROR, "Failed copy");
        return false;
    }

    for (int i = 0; i < temps_clone->lenght; i++) {

        float original; mnt_darray_get(temps, &original, i);
        float clone; mnt_darray_get(temps, &clone, i);

        if (clone != original) {
            return false;
        }

        mnt_log(MNT_INFO, "original[%d] = %.2f, clone[%d] = %.2f", i, original, i, clone);

    }

    //pop tests
    float expected[] = {443.2, 10.0, 84.3};
    float poped = 69;
    float comp = 99.2;
    mnt_darray_pop(temps, &poped);

    if (poped != comp) {
        mnt_log(MNT_ERROR, "invalid pop,\nexpected = 99.2\ngot = %.2f", poped);

        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected[i] != holder) {

            mnt_log(MNT_ERROR, "invalid comparisson");

            return false;

        }

    }

    //insert tests
    float insert_val = 55.5;
    mnt_darray_insert(temps, &insert_val, 1);

    float expected_insert[] = {443.2, 55.5, 10.0, 84.3};

    if (temps->lenght != sizeof(expected_insert)/sizeof(*expected_insert)) {
        mnt_log(MNT_ERROR, "invalid lenght after insert,\nexpected = %d\ngot = %d",
                (int)(sizeof(expected_insert)/sizeof(*expected_insert)), (int)temps->lenght);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_insert[i] != holder) {
            mnt_log(MNT_ERROR, "invalid insert at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_insert[i], holder);
            return false;
        }
    }

    //insert at end tests
    float insert_end_val = 7.7;
    mnt_darray_insert(temps, &insert_end_val, temps->lenght);

    float expected_insert_end[] = {443.2, 55.5, 10.0, 84.3, 7.7};

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_insert_end[i] != holder) {
            mnt_log(MNT_ERROR, "invalid insert at end, index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_insert_end[i], holder);
            return false;
        }
    }

    //set tests
    float set_val = 3.3;
    mnt_darray_set(temps, &set_val, 2);

    float expected_set[] = {443.2, 55.5, 3.3, 84.3, 7.7};

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_set[i] != holder) {
            mnt_log(MNT_ERROR, "invalid set at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_set[i], holder);
            return false;
        }
    }

    //pop_front tests
    float poped_front = 69;
    comp = 443.2;
    mnt_darray_pop_front(temps, &poped_front);

    if (poped_front != comp) {
        mnt_log(MNT_ERROR, "invalid pop_front,\nexpected = 443.20\ngot = %.2f", poped_front);
        return false;
    }

    float expected_pop_front[] = {55.5, 3.3, 84.3, 7.7};

    if (temps->lenght != sizeof(expected_pop_front)/sizeof(*expected_pop_front)) {
        mnt_log(MNT_ERROR, "invalid lenght after pop_front,\nexpected = %d\ngot = %d",
                (int)(sizeof(expected_pop_front)/sizeof(*expected_pop_front)), (int)temps->lenght);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_pop_front[i] != holder) {
            mnt_log(MNT_ERROR, "invalid pop_front, index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_pop_front[i], holder);
            return false;
        }
    }

    //remove tests
    mnt_darray_remove(temps, 1);

    float expected_remove[] = {55.5, 84.3, 7.7};

    if (temps->lenght != sizeof(expected_remove)/sizeof(*expected_remove)) {
        mnt_log(MNT_ERROR, "invalid lenght after remove,\nexpected = %d\ngot = %d",
                (int)(sizeof(expected_remove)/sizeof(*expected_remove)), (int)temps->lenght);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_remove[i] != holder) {
            mnt_log(MNT_ERROR, "invalid remove at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_remove[i], holder);
            return false;
        }
    }

    //reserve tests
    size_t cap_before = temps->capacity;
    mnt_darray_reserve(temps, 128);

    if (temps->capacity < 128) {
        mnt_log(MNT_ERROR, "reserve failed to grow capacity,\nexpected >= 128\ngot = %d",
                (int)temps->capacity);
        return false;
    }

    if (temps->lenght != sizeof(expected_remove)/sizeof(*expected_remove)) {
        mnt_log(MNT_ERROR, "reserve altered lenght,\nexpected = %d\ngot = %d",
                (int)(sizeof(expected_remove)/sizeof(*expected_remove)), (int)temps->lenght);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_remove[i] != holder) {
            mnt_log(MNT_ERROR, "reserve corrupted data at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_remove[i], holder);
            return false;
        }
    }

    mnt_log(MNT_INFO, "capacity before reserve = %d, after = %d",
            (int)cap_before, (int)temps->capacity);

    //shrink tests
    mnt_darray_shrink(temps);

    if (temps->capacity != temps->lenght) {
        mnt_log(MNT_ERROR, "shrink did not match capacity to lenght,\nlenght = %d\ncapacity = %d",
                (int)temps->lenght, (int)temps->capacity);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_remove[i] != holder) {
            mnt_log(MNT_ERROR, "shrink corrupted data at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_remove[i], holder);
            return false;
        }
    }

    //append after shrink tests
    float post_shrink = 1.1;
    mnt_darray_append(temps, &post_shrink);

    float expected_post_shrink[] = {55.5, 84.3, 7.7, 1.1};

    if (temps->lenght != sizeof(expected_post_shrink)/sizeof(*expected_post_shrink)) {
        mnt_log(MNT_ERROR, "invalid lenght after post shrink append,\nexpected = %d\ngot = %d",
                (int)(sizeof(expected_post_shrink)/sizeof(*expected_post_shrink)), (int)temps->lenght);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_post_shrink[i] != holder) {
            mnt_log(MNT_ERROR, "invalid post shrink append at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_post_shrink[i], holder);
            return false;
        }
    }

    //grow tests
    size_t grow_cap = temps->capacity;
    _mnt_darray_grow(temps, 32);

    if (temps->capacity < grow_cap + 32) {
        mnt_log(MNT_ERROR, "grow failed,\nexpected >= %d\ngot = %d",
                (int)(grow_cap + 32), (int)temps->capacity);
        return false;
    }

    for (int i = 0; i < temps->lenght; i++) {

        float holder; mnt_darray_get(temps, &holder, i);

        if (expected_post_shrink[i] != holder) {
            mnt_log(MNT_ERROR, "grow corrupted data at index %d,\nexpected = %.2f\ngot = %.2f",
                    i, expected_post_shrink[i], holder);
            return false;
        }
    }

    mnt_darray_free(temps);
    mnt_darray_free(temps_clone);

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

    Mnt_Arena arena = mnt_static_arena_make(mnt_mb(1), NULL);

    fprintf(log, "\nTests Mount lib:\n%s\n"
            "---------------------------------------------------------------------\n",
            ctime(&now));

    fprintf(log, "Create array test:\n\t%s\n", passed(create_array(arena)));

    fprintf(log, "Modify array test:\n\t%s\n", passed(modify_array()));

    fprintf(log, "Clone array test:\n\t%s\n", passed(clone_array()));

    fprintf(log, "Iterate array test:\n\t%s\n", passed(iterate_array()));

    fprintf(log, "Static Arenas test:\n\t%s\n", passed(static_arena_tests()));

    fprintf(log, "Dynamic arrays test:\n\t%s\n", passed(dynamic_array()));

    return 0;

}

