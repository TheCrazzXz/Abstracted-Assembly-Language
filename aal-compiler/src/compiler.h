#ifndef __AAL_COMPILER_H__
#define __AAL_COMPILER_H__

#include <stdio.h>
#include "aal-data.h"
#include "aal-functions.h"
#include "stack.h"
#include "preprocessor.h"

#define compiler_set_register_dword(register_name, value) compiler_operate_register("mov", register_name, value, "dword")
#define compiler_set_register_byte(register_name, value) compiler_operate_register("mov", register_name, value, "byte")
#define current_function (aalc_state->functions[aalc_state->current_function_index])

#define currinst aalc_state->current_instruction
#define currchr aalc_state->current_instruction_char
#define currchr_idx ((unsigned long)currchr - (unsigned long)currinst)

typedef enum __compiler_value_type_e
{
    COMPILER_VALUE_TYPE_REGISTER,
    COMPILER_VALUE_TYPE_EXPRESSION
} compiler_value_type;

typedef struct __compiler_value_data_t
{
    compiler_value_type type;
    expr_data expr;
    size_t register_number;
    Bool register_byte;
} compiler_value_data;

typedef enum __compiler_array_access_type_e
{
    COMPILER_ACCESS_ARRAY_TYPE_NONE,
    COMPILER_ACCESS_ARRAY_TYPE_REGISTER,
    COMPILER_ACCESS_ARRAY_TYPE_INDEX,
} compiler_array_access_type;

typedef struct __compiler_array_access_data_t
{
    compiler_array_access_type type;
    size_t arr_index;
} compiler_array_access_data;



typedef struct __condition_stack_frame_t
{
    size_t number;
    Bool fail_defined;
    Bool pending_end;
} condition_stack_frame;

struct __aal_function_t;

typedef struct __compiler_state_t
{
    size_t asm_file_padding;

    /* list of all data that have to go in data segment */
    expr_data* data_seg_data;
    size_t data_seg_data_len;

    /* list of variables */
    aal_stack_frame stack_frame;

    /* list of functions */
    aal_function* functions;
    size_t functions_len;

    /* current function */
    ssize_t current_function_index;

    /* line */
    size_t currentline;
    char* current_instruction;
    char* current_instruction_char;

    /* registers */
    Bool register_usage[4]; /* 0 : a ; 1 : b ; 2 : c ; 3 : d */

    /* files */
    FILE* asm_file;
    FILE* source_file;
    char* current_source_file_name;

    /* macros */
    macro* macros;
    size_t macros_len;

    /* verbose */
    Bool verbose;
    
} compiler_state;

extern compiler_state* aalc_state;

int convert_to_assembly_source(FILE* source, FILE* asm_file, char*, Bool verbose, Bool main_file);
void compiler_append_code(const char* format, ...);


#endif