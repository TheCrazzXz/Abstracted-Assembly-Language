/*
    Abstracted Assembly Language Compiler for LINUX x86 (32 bits).

    Written bt TheCrazzXz / PainHandler.
    Started : 3 August 2023.

    Usage : aalc <source.aal> -o <output executable>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler-logging.h"
#include "compiler.h"
#include "memory.h"
#include "preprocessor.h"

typedef struct __parsed_main_args_t
{
    char* source;
    char* output;
    Bool verbose;
} parsed_main_args;



void print_usage(char** argv)
{
    printf("Usage : %s (args...)\n\
    args :\n\
    \t-o <output> : sets the output executable to <output>\n\
    \t<source file> : sets a source file to <source file>\n\
    \t-v : enable verbose\n", argv[0]);
}

parsed_main_args parse_main_args(int argc, char** argv)
{
    parsed_main_args pma;
    memset(&pma, 0, sizeof(parsed_main_args));
    size_t i;
    for(i = 0 ; i < argc ; i++)
    {
        if(strcmp(argv[i], "-o") == 0)
        {
            i++;
            if(index_is_overflowing_size(i, argc))
            {
                aalc_error("No argument after -o");
                exit(EXIT_FAILURE);
            }
            pma.output = argv[i];
        }
        else if(strcmp(argv[i], "-v") == 0)
        {
            pma.verbose = TRUE;
        }
        else if(strcmp(argv[i], "-h") == 0)
        {
            print_usage(argv);
            exit(EXIT_SUCCESS);
        }
        else
        {
            pma.source = argv[i];
        }
    }
    return pma;
}

int main(int argc, char** argv)
{
    parsed_main_args pma;
    char* asm_file_name;
    char* source_name;
    size_t source_len_before_extension;
    FILE* asm_file;
    FILE* source;
    char *assemble_command;
    
    pma = parse_main_args(argc, argv);

    if(pma.source == NULL || pma.output == NULL)
    {
        exit(EXIT_SUCCESS);
    }

    compile_aal_file(pma.source, &source_name, &asm_file_name, pma.verbose, TRUE);
    if(aalc_state->macros != NULL)
    {
        free(aalc_state->macros);
    }
    
    assemble_command = malloc(strlen("nasm -f elf32 -o .o  ; ld -m elf_i386 -o  .o") + strlen(source_name) + strlen(asm_file_name) + strlen(pma.output) + strlen(source_name) + 1);

    sprintf(assemble_command, "nasm -f elf32 -o %s.o %s ; ld -m elf_i386 -o %s %s.o", source_name, asm_file_name, pma.output, source_name);
    printf("command : %s\n", assemble_command);
    system(assemble_command);

    free(assemble_command);
    free(asm_file_name);
    free(source_name);

}
