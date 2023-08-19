#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

#include <stdio.h>
#include "memory.h"

typedef struct preprocessor_macro_t
{
    char* name;
    dword_t value;
} macro;

ssize_t macro_index(char* name);

void compile_aal_file(char* input_file_name, char** source_name, char** asm_file_name, Bool verbose, Bool main_file);
void preprocess_code(FILE* source, FILE* asm_file, Bool verbose);

#endif