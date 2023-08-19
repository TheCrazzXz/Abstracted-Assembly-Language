#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char parse_double_quote_escape(char* data)
{
    if(*data == 'n')
    {
        return '\n';
    }
    else if(*data == 't')
    {
        return '\t';
    }
    else if(*data == '\\')
    {
        return '\\';
    }
    else
    {
        return '\0';
    }
}

char* strdupc(char* str, char end, int enablequoting)
{
    size_t len = 0;
    char* alloc;
    int quote;

    quote = 0;

    for(;;)
    {
        ALLOC_NEW_ELEMENT_FOR_VECTOR(alloc, len, sizeof(char));

        if(*str == '"')
        {
            if(quote)
            {
                quote = 0;
            }
            else
            {
                quote = 1;
            }
        }

        if((*str == end || *str == '\0') && ((quote == 0) || !enablequoting))
        {
            alloc[len - 1] = '\0';
            return alloc;
        }
        alloc[len - 1] = *str;

        str++;
    }

    
}

char* strdupc_multi(char* str, char* ends, char* output_end, int enablequoting)
{
    size_t len = 0;
    size_t i;
    char* alloc;
    int quote;

    quote = 0;

    for(;;)
    {
        ALLOC_NEW_ELEMENT_FOR_VECTOR(alloc, len, sizeof(char));

        if(*str == '"')
        {
            if(quote)
            {
                quote = 0;
            }
            else
            {
                quote = 1;
            }
        }

        if(((quote == 0) || !enablequoting))
        {
            for(i = 0 ; i < strlen(ends) + 1; i++)
            {
                if(*str == ends[i])
                {
                    alloc[len - 1] = '\0';
                    if(output_end != NULL)
                    {
                        *output_end = ends[i];
                    }
                    
                    return alloc;
                }
            }
        }
        
        alloc[len - 1] = *str;

        str++;
    }
}

char* alloc_and_copy_string(char* str)
{
    char* new_str = malloc(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
} 