#include "log.h"

#include <SDL2/SDL_error.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <SDL2/SDL.h>

#ifdef _WIN32
    #include <windows.h>
#endif

void log_f(const string fmt, const string file, int line, ...)
{
#if (DEBUG == 1 && WIN32)
    static boolean alloc = FALSE;

    if (!alloc)
    {
        AllocConsole();

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);

        alloc = TRUE;
    }
#endif

    time_t timer = time(NULL);
    struct tm* tm_info = localtime(&timer);
    
    char buffer[128];
    strftime(buffer, 26, "%H:%M:%S", tm_info);
	printf("[%s %s:%d] ", buffer, file, line);

    // print actual info
    va_list args;
    va_start(args, line);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

void log_msgboxf(const string fmt, const string file, int line, ...)
{
    char buffer[1024];

    // print actual info
    va_list args;
    va_start(args, line);
    vsnprintf(buffer, 1024, fmt, args);
    va_end(args);

    char finBuffer[2048];
    snprintf(finBuffer, 2048, "File: %s                                           \nLine: %d\n\n%s", file, line, buffer);

    if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Runtime Error! (Please contact developers)", finBuffer, NULL) < 0)
    {
        ILOG("Failed to show error: %s", SDL_GetError());
        va_list args;
        va_start(args, line);
        vprintf(fmt, args);
        va_end(args);
        putchar('\n');
    }
    exit(1);
}
