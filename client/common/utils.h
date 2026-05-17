#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>


#if defined(ARM9) 

#include <nds.h>

#elif defined(__3DS__)

#include <3ds.h>
#include <cstdarg>

#else 

#include <cstdarg>

#endif

void initConsole();

void waitForKey(int key);

static inline void consolef(const char* tmpl, ...) {
    va_list args;
    va_start(args, tmpl);

    va_list args_log;
    va_copy(args_log, args);

    #if defined(ARM9) || defined(__3DS__)
        va_list args_copy;
        va_copy(args_copy, args);
        viprintf(tmpl, args_copy);
        va_end(args_copy);
    #else
        vprintf(tmpl, args);
    #endif

    FILE* log = fopen("logs.txt", "a");
    if (log) {
        vfprintf(log, tmpl, args_log);
        fclose(log);
    }
    va_end(args_log);

    va_end(args);
}

static inline void dsExit(int code) {
    #if defined(ARM9) || defined(__3DS__)
        consolef("Press start to exit...\n");
        waitForKey(KEY_START);
    #endif
    exit(code);
}

#endif