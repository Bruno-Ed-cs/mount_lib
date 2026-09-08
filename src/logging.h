#ifndef H_MOUNT_LOGGING
#define H_MOUNT_LOGGING
#include <stdio.h>

typedef enum {

    MNT_INFO = 0,
    MNT_DEBUG,
    MNT_ERROR, 
    MNT_FATAL

} Mnt_Log_Level;

void mnt_log_message(Mnt_Log_Level log_level, char* message, char* file, int line, ...);
void mnt_set_log_level(Mnt_Log_Level log_level);
void mnt_set_log_output(FILE* output);

#define mnt_log(log_level, message, ...) mnt_log_message((log_level), (message), __FILE__, __LINE__, ##__VA_ARGS__)

//H_MOUNT_LOGGING
#endif
