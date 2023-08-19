#include "aal-functions.h"

#include "aal-data.h"
#include "compiler.h"
#include "memory.h"
#include <stdlib.h>
#include "compiler-logging.h"
#include <string.h>
#include "source-parser.h"

void compiler_add_function(aal_function function)
{
    ALLOC_NEW_ELEMENT_FOR_VECTOR(aalc_state->functions, aalc_state->functions_len, sizeof(aal_function));
    aalc_state->functions[aalc_state->functions_len - 1] = function;
}

aal_function create_compute_function(char* name, struct __aal_stack_variable_t* parameters, size_t parameters_len)
{
    aal_function function;
    function.parameters_size = 0;
    size_t i;
    for(i = 0 ; i < parameters_len ; i++)
    {
        printf("parameter %s (type=%d)\n", parameters[i].name, parameters[i].type);
        if(i == 0)
        {
            /* pass the ebp save and eip save */
            parameters[i].offset = 8;
        }
        else
        {
            parameters[i].offset = parameters[i - 1].offset + parameters[i - 1].data_size;
        }
        function.parameters_size += parameters[i].data_size;
    }
    function.parameters = parameters;
    function.paremeters_len = parameters_len;
    function.name = strdup(name);
    function.defined = FALSE;

    return function;
}

ssize_t function_name_to_index(char* name)
{
    size_t i;

    for(i = 0 ; i < aalc_state->functions_len ; i++)
    {
        if(strcmp(name, aalc_state->functions[i].name) == 0)
        {
            return (ssize_t)i;
        }
    }
    return -1;
}

void aalc_start_stackframe()
{
    aalc_state->stack_frame.stack_frame_size = 0;
    aalc_state->stack_frame.submitted_stack_frame = FALSE;
    aalc_state->stack_frame.defining_stack_frame = FALSE;
}

void compiler_set_stackframe(size_t size)
{
    char* size_str = malloc(16);

    sprintf(size_str, "%lu", size);
    
    
    compiler_operate_register("sub", "esp", size_str, "dword");
    compiler_append_code("\n");
    free(size_str);
    aalc_state->stack_frame.submitted_stack_frame = TRUE;
    aalc_state->stack_frame.defining_stack_frame = FALSE;
}


void compiler_define_function(size_t function_index)
{
    aal_function* function = &aalc_state->functions[function_index];
    char* label_code = malloc(1 + strlen(function->name) + 1 + 1);

    sprintf(label_code, "%s:\n", function->name);

    aalc_state->asm_file_padding = 0;

    aalc_state->current_function_index = function_index;
    compiler_append_code(label_code);
    free(label_code);

    aalc_state->asm_file_padding = 1;
    function->defined = TRUE;

    if(aalc_state->stack_frame.stack_variables != NULL)
    {
        free(aalc_state->stack_frame.stack_variables);
        memset(&aalc_state->stack_frame, 0, sizeof(aal_stack_frame));
    }
    compiler_append_code("push ebp\n");
    compiler_append_code("mov ebp, esp\n\n");
    aalc_start_stackframe();
}

void compiler_declare_function(char* name, struct __aal_stack_variable_t* parameters, size_t parameters_len)
{
    aal_function function;
    size_t index;

    index = function_name_to_index(name);
    if(index != -1)
    {
        aalc_error("Function '%s' was already declared before, can't re-declare it");
        exit(EXIT_FAILURE);
    }
    function = create_compute_function(name, parameters, parameters_len);
    
    compiler_add_function(function);

    
}

void parse_function_call(size_t index, char** curr)
{
    char_expect(**curr, '(');
    (*curr)++;
    parse_push_arguments(curr, (size_t)index);
    
    compiler_append_code("call %s\n", aalc_state->functions[index].name);
    if(aalc_state->functions[index].parameters_size > 0)
    {
        compiler_append_code("add esp, %lu\n\n", aalc_state->functions[index].parameters_size);
    }
                  
}