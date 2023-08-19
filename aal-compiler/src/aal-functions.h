#ifndef __AAL_FUNCTIONS_H__
#define __AAL_FUNCTIONS_H__

#include <stdlib.h>
#include "memory.h"
#define compiler_end_stackframe() compiler_append_code("leave\n");

struct __aal_stack_variable_t;

typedef struct __aal_function_t
{
    char* name; /* name of the function */
    struct __aal_stack_variable_t* parameters; /* parameters of the function */
    size_t paremeters_len; /* number of parameters for the function */
    size_t parameters_size; /* size of all parameters */
    Bool defined;
} aal_function;

ssize_t function_name_to_index(char* name);
void compiler_define_function(size_t function_index);
void compiler_declare_function(char* name, struct __aal_stack_variable_t* parameters, size_t parameters_len);
void parse_function_call(size_t index, char** curr);

void aalc_start_stackframe();
void compiler_set_stackframe(size_t size);

#endif