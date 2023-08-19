#include "aal-data.h"
#include "compiler-logging.h"
#include "compiler.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

char* data_seg_idx_to_name(size_t index)
{
    char* name;

    name = malloc(strlen("data") + 16 + 1);

    sprintf(name, "data%lu", index);

    return name;
}

aal_var_ref aal_variable_get_reference(char* name)
{
    aal_var_ref var_ref;
    size_t i;

    
    for(i = 0 ; i < aalc_state->stack_frame.stack_variables_len ; i++)
    {
        if(strcmp(name, aalc_state->stack_frame.stack_variables[i].name) == 0)
        {
            var_ref.type = AAL_VAR_REF_TYPE_STACK_FRAME;
            var_ref.data_type = aalc_state->stack_frame.stack_variables[i].type;
            var_ref.index = i;
            return var_ref;
        }
    }
    if(aalc_state->current_function_index != -1)
    {
        for(i = 0 ; i < aalc_state->functions[aalc_state->current_function_index].paremeters_len ; i++)
        {
            if(strcmp(name, aalc_state->functions[aalc_state->current_function_index].parameters[i].name) == 0)
            {
                var_ref.type = AAL_VAR_REF_TYPE_STACK_PARAMETER;
                var_ref.data_type = aalc_state->functions[aalc_state->current_function_index].parameters[i].type;
                var_ref.index = i;
                return var_ref;
            }
        }
    }
    
    for(i = 0 ; i < aalc_state->data_seg_data_len ; i++)
    {
        if(strcmp(name, aalc_state->data_seg_data[i].name) == 0)
        {
            var_ref.type = AAL_VAR_REF_TYPE_STACK_FRAME;
            var_ref.data_type = aalc_state->data_seg_data[i].data_type;
            var_ref.index = i;
            return var_ref;
        }
    }
    var_ref.type = AAL_VAR_REF_TYPE_ERROR;
    var_ref.index = 0;
    return var_ref;
}


size_t type2size(aal_data_type type)
{
    if(type == AAL_DATA_TYPE_DWORD)
    {
        return 4;
    }
    else if(type == AAL_DATA_TYPE_BYTE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
aal_data_type str2type(char* type)
{
    if(strcmp(type, "dword") == 0 || strcmp(type, "pointer") == 0)
    {
        return AAL_DATA_TYPE_DWORD;
    }
    else if(strcmp(type, "byte") == 0)
    {
        return AAL_DATA_TYPE_BYTE;
    }
    else
    {
        return AAL_DATA_TYPE_ERROR;
    }
}

char* str_as_asm(char* str)
{
    char* out;
    char escape[10];
    size_t len = 0;
    size_t len_save;

    printf("str : %s\n", str);

    while(1)
    {
        if(*str < 33 && *str > 0 && *str != ' ')
        {
            sprintf(escape, "\", 0x%x, \"", (unsigned char)*str);
            
            ALLOC_NEW_ELEMENT_FOR_VECTOR(out, len, sizeof(char));
            out[len - 1] = '\0';
            len += 9;
            out = realloc(out, len);
            if(!out)
            {
                fprintf(stderr, "Error : reallocation error\n");
                exit(EXIT_FAILURE);
            }
            strcat(out, escape);
            len--;
        }
        else if(*str == 0)
        {
            ALLOC_NEW_ELEMENT_FOR_VECTOR(out, len, sizeof(char));
            out[len - 1] = '\0';
            break;
        }
        else
        {
            ALLOC_NEW_ELEMENT_FOR_VECTOR(out, len, sizeof(char));
            out[len - 1] = *str;
        }
        str++;
    }
    return out;
}

void compute_data_segment()
{
    size_t i;
    char* name;
    if(i > 0)
    {
        fputs("section .data\n", aalc_state->asm_file);
        printf("In segment .data\n");
    }

    for(i = 0 ; i < aalc_state->data_seg_data_len ; i++)
    {
        name = data_seg_idx_to_name(i);
        if(aalc_state->data_seg_data[i].type == EXPR_DATA_TYPE_STRING_LITERRAL)
        {
            fprintf(aalc_state->asm_file, "%s db \"%s\", 0x00\n", name, str_as_asm((char*)aalc_state->data_seg_data[i].data));
            printf("\tset string data for %s : \"%s\"\n", name, (char*)aalc_state->data_seg_data[i].data);
        }
        free(name);
    }
}

size_t find_usable_register_number()
{
    size_t i;
    for(i = 0 ; i < 4 ; i++)
    {
        if(aalc_state->register_usage[i] == FALSE)
        {
            return i;
        }
    }
    aalc_error("Expression is not usable because there's not enough registers to use it :(");
    exit(EXIT_FAILURE);
    return 0;
}


size_t find_and_use_usable_register_number()
{
    size_t i;
    i = find_usable_register_number();
    aalc_state->register_usage[i] = TRUE;
    return i;
}

