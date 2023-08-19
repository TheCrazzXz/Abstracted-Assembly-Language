#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
#include <stdint.h>

#define index_is_overflowing_size(index, size) ((index + 1) > size)
#define offsetptr_fd(ptr, off, type) ((type*)((unsigned long)ptr + off))

typedef uint32_t dword_t;
typedef uint16_t word_t;
#define ALLOC_NEW_ELEMENT_FOR_VECTOR(data, len, size) {\
    (len)++; \
    if((len) == 1) \
    { \
        (data) = malloc((size)); \
        if(((data) == NULL)) \
        { \
            fprintf(stderr, "Error : Couldn't allocate memory"); \
            exit(EXIT_FAILURE); \
        } \
    } \
    else \
    { \
        (data) = realloc((data), (size) * (len)); \
        if(((data) == NULL)) \
        { \
            fprintf(stderr, "Error : Couldn't re-allocate memory"); \
            exit(EXIT_FAILURE); \
        } \
    } \
}

typedef enum __bool_e
{
    FALSE,
    TRUE
} Bool;


char parse_double_quote_escape(char* data);/* strdup until a character */
char* strdupc(char* str, char end, int enablequoting);
char* strdupc_multi(char* str, char* ends, char* output_end, int enablequoting);
char* alloc_and_copy_string(char* str);
#endif