#include "logging.h"
#include <stdarg.h>
#include <unistd.h>

Mnt_Log_Level min_logging = MNT_INFO;
FILE* log_output = NULL;

void mnt_log_message(Mnt_Log_Level log_level, char* message, char* file, int line, ...) {

    if (log_output == NULL) {
        log_output = stdout;
    }

    if (log_level >= min_logging) {

        switch (log_level) {

            case MNT_INFO:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;97;160;255m");

                fprintf(log_output, "INFO");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_DEBUG:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;204;255;44m");

                fprintf(log_output, "DEBUG");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_ERROR:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;255;0;90m");

                fprintf(log_output, "ERROR");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

            case MNT_FATAL:

                fprintf(log_output, "[");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[38;2;255;0;0m");

                fprintf(log_output, "FATAL");

                if(isatty(fileno(log_output)))
                    fprintf(log_output, "\033[0m");

                fprintf(log_output, "] ---- %s: line(%d)\n", file, line);

            break;

        }

        va_list args;
        va_start(args, line);
        
        vfprintf(log_output, message, args);
        fprintf(log_output, "\n");

        va_end(args);

    }

}

void mnt_set_log_level(Mnt_Log_Level log_level) {

    min_logging = log_level;

}

void mnt_set_log_output(FILE* output) {

    log_output = output;

}

