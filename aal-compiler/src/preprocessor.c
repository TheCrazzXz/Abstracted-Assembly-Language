#include "preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler-logging.h"
#include "memory.h"
#include "source-parser.h"
#include "compiler.h"

#define aal_source_file_extension ".aal"
#define asm_source_file_extension ".asm"

ssize_t macro_index(char* name)
{
    ssize_t i;
    for(i = 0 ; i < aalc_state->macros_len ; i++)
    {
        if(strcmp(name, aalc_state->macros[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void compile_aal_file(char* input_file_name, char** source_name, char** asm_file_name, Bool verbose, Bool main_file)
{
    size_t source_len_before_extension = strlen(input_file_name) - strlen(aal_source_file_extension);
    FILE* source;
    FILE* asm_file;
    
    if(strcmp(offsetptr_fd(input_file_name, source_len_before_extension, char), aal_source_file_extension) != 0)
    {
        aalc_error("File %s is not a file with .aal extension", input_file_name);
        exit(EXIT_FAILURE);
    }

    source = fopen(input_file_name, "r");
    if(!source)
    {
        aalc_error("Could not open %s source file to read", input_file_name);
        exit(EXIT_FAILURE);
    }

    *source_name = malloc(source_len_before_extension + 1);
    strncpy(*source_name, input_file_name, source_len_before_extension);

    *asm_file_name = malloc(source_len_before_extension + strlen(asm_source_file_extension) + 1);
    strcpy(*asm_file_name, *source_name);
    strcat(*asm_file_name, asm_source_file_extension);
    
    asm_file = fopen(*asm_file_name, "w+");
    if(!asm_file)
    {
        aalc_error("Could not open %s file to write", *asm_file_name);
        exit(EXIT_FAILURE);
    }

    convert_to_assembly_source(source, asm_file, *source_name, verbose, main_file);

    fclose(asm_file);
    fclose(source);
}

void preprocess_code(FILE* source, FILE* asm_file, Bool verbose)
{
    //char* currinst;
    //char* currchr;
    char* filename;
    char* source_name;
    char* asm_file_name;
    char* source_save;
    char* macro_name;
    Bool comment = FALSE;
    FILE* fp;
    int source_ignore_whitespaces_ret;
    size_t len;
    dword_t uint;

    if(verbose)
    {
        printf("PREPROCESSING CODE...\n");
    }
    while(1)
    {
        source_ignore_whitespaces_ret = source_ignore_whitespaces(source);
        
        /* if code is finished, stop */
        if(source_ignore_whitespaces_ret == 1)
        {
            break;
        }

        if(getc(source) == '@')
        {
            currinst = source_read_instruction(source);
            currchr = currinst;

            aalc_state->currentline++;

            if(compiler_ignore_comments(currinst, &comment) == TRUE)
            {
                continue;
            }

            printf("preprocessor instruction : %s\n", currinst);
            if(strncmp(currinst, "include_file", 12) == 0)
            {
                currchr += 12;
                char_expect(*currchr, ' ');
                currchr++;

                if(*currchr != '"')
                {
                    aalc_error("Error : include_file takes one argument as string litteral");
                    exit(EXIT_FAILURE);
                }
                filename = read_double_quote(currchr);
                printf("including : %s\n", filename);
                
                source_save = aalc_state->current_source_file_name;
                compile_aal_file(filename, &source_name, &asm_file_name, verbose, FALSE);
                
                aalc_state->source_file = source;
                aalc_state->current_source_file_name = source_save;
                aalc_state->asm_file = asm_file;
                aalc_state->currentline = 0;

                compiler_append_code("%s \"%s\"\n", "%include", asm_file_name);
            }
            else if(strncmp(currinst, "set", 3) == 0)
            {
                currchr += 3;
                char_expect(*currchr, ' ');
                currchr++;
                
                macro_name = read_symbol_name(currchr);
                printf("macro name : %s\n", macro_name);
                currchr += strlen(macro_name);

                char_expect(*currchr, ' ');
                currchr++;

                uint = read_uint(currchr, &len);
                currchr += len;

                ALLOC_NEW_ELEMENT_FOR_VECTOR(aalc_state->macros, aalc_state->macros_len, sizeof(macro));
                aalc_state->macros[aalc_state->macros_len - 1].name = macro_name;
                aalc_state->macros[aalc_state->macros_len - 1].value = uint;

            }
            else
            {
                aalc_error("PREPROCESSING : UNKNOWN INSTRUCTION");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fseek(source, -1, SEEK_CUR);
            return;
        }
    }
}