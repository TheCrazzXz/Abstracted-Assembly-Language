#include "compiler-logging.h"
#include <stdarg.h>
#include <stdio.h>
#include "color.h"
#include "compiler.h"

void printlnx(char c, char fill, size_t x)
{
    size_t i;
    for(i = 0 ; i < x ; i++)
    {
        printf("%c", fill);
    }
    printf("%c\n", c);
}

void aalc_error(const char* format, ...)
{
    va_list    args;

    va_start(args, format);
    
    fprintf(stderr, "%sCompiler error in file \"%s.aal\" at line %lu at character %lu : ", ANSI_COLOR_RED, aalc_state->current_source_file_name, aalc_state->currentline, currchr_idx);
    vfprintf(stderr, format, args);

    fprintf(stderr, "\n%s\n", currinst);
    printlnx('^', '-', currchr_idx);

    fprintf(stderr, "%s\n", ANSI_COLOR_RESET);

    va_end(args);
}