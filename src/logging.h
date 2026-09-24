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
