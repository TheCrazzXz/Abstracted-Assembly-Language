#include "aal-variables.h"
#include "aal-data.h"
#include "compiler-operations.h"
#include "source-parser.h"
#include "compiler.h"
#include "aal-data.h"
#include "compiler-logging.h"
#include "expression-parser.h"
#include <stdlib.h>

aal_data_type dereference_variable(char** expression, char** str, size_t n, aal_operation op, Bool* set_in_place, aal_data_type dst_type)
{
    size_t len;
    char** otherstr;
    char* offset_str;
    char* varname;
    char* reg_name;
    compiler_value_data value_data;
    aal_var_ref var_ref;
    Bool address;
    Bool dereference;
    aal_data_type dereference_type;
    aal_stack_variable* stack_var;
    size_t reg_num;
    size_t reg_num2;
    *set_in_place = FALSE;
    
    address = FALSE;
    dereference = FALSE;

    dereference_type = switch_dereference_type(expression);

    if(**expression == '&')
    {
        address = TRUE;
        (*expression)++;
    }
    else if(dereference_type != AAL_DATA_TYPE_NONE)
    {
       dereference = TRUE;
    }

    varname = read_symbol_name(*expression);
    var_ref = aal_variable_get_reference(varname);

    if(var_ref.type == AAL_VAR_REF_TYPE_ERROR)
    {
        *str = NULL;
        return AAL_DATA_TYPE_ERROR;
    }
    *expression += strlen(varname);

    if(address)
    {
        if(var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER || var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME)
        {
            reg_num = find_usable_register_number();
            compiler_append_code("mov %s, ebp\n", reg_number_to_dword_reg(reg_num));
            compiler_append_code("%s %s, %lu\n", (var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER ? "add" : "sub"), reg_number_to_dword_reg(reg_num), get_stack_var_from_var_ref(var_ref)->offset);
            *str = strdup(reg_number_to_dword_reg(reg_num));
            return AAL_DATA_TYPE_DWORD;
        }   
    }
   

    if((**expression) == '<')
    {
        (*expression)++;
        value_data = source_parse_expression_core(expression, -1, FALSE, AAL_DATA_TYPE_DWORD);

        if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
        {
            *str = get_array_index_from_register_code(var_ref, value_data.register_number);
            (*expression) += 1;
        }
        else
        {
            if(value_data.expr.type == EXPR_DATA_TYPE_DATA_TYPE && value_data.expr.data_type == AAL_DATA_TYPE_DWORD)
            {
                *str = get_array_index_code(var_ref, *(dword_t*)value_data.expr.data);
            }
            else
            {
                aalc_error("Error : Array subscript not an integer");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        *str = get_variable_code(var_ref);
    }

    if(!address)
    {
        add_square_brackets(str);
    }

    if(dereference)
    {
        if(dst_type == AAL_DATA_TYPE_DWORD && op == AAL_OPERATION_NOT_SPECIFIC && dereference_type == AAL_DATA_TYPE_BYTE)
        {
            compiler_append_code("xor %s, %s\n", reg_number_to_dword_reg(n), reg_number_to_dword_reg(n));
        }
        
        reg_num = find_and_use_usable_register_number();
        //reg_num2 = find_and_use_usable_register_number();

        //reg_name = reg_number_to_reg(reg_num, dereference_type == AAL_DATA_TYPE_BYTE);
        compiler_append_code("mov %s, %s\n", reg_number_to_dword_reg(reg_num), *str);
        //compiler_append_code("mov %s, %s [%s]\n", reg_name, (dereference_type == AAL_DATA_TYPE_BYTE) ? "byte" : "dword", reg_number_to_dword_reg(reg_num));
        
        reg_name = strdup(reg_number_to_dword_reg(reg_num));
        add_square_brackets(&reg_name);

        printf("writing this : is deference type byte : %d\n", dereference_type == AAL_DATA_TYPE_BYTE);

        register_operation(op, reg_number_to_reg(n, dereference_type == AAL_DATA_TYPE_BYTE), reg_name, (dereference_type == AAL_DATA_TYPE_BYTE) ? "byte" : "dword");
        //*str = strdup(reg_name);

        //drop_register_usage(reg_num2);
        drop_register_usage(reg_num);

        *set_in_place = TRUE;

        return dereference_type;
    }

    if(dst_type == AAL_DATA_TYPE_DWORD && op == AAL_OPERATION_NOT_SPECIFIC && var_ref.data_type == AAL_DATA_TYPE_BYTE)
    {
        compiler_append_code("xor %s, %s\n", reg_number_to_dword_reg(n), reg_number_to_dword_reg(n));
    }

    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME || var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        stack_var = get_stack_var_from_var_ref(var_ref);
        return stack_var->type;
    }
    else
    {
        return aalc_state->data_seg_data[var_ref.index].data_type;
    }
}
void compiler_set_u32_variable(aal_var_ref var_ref, compiler_array_access_data array_access_data, compiler_value_data value_data)
{
    
    char* dst;
    char* src;
    dst = switch_compiler_access_array(var_ref, array_access_data);
    add_square_brackets(&dst);
    
    value_data.register_number = 0;
    src = switch_compiler_value(value_data, FALSE);

    compiler_set_register_dword(dst, src);
    compiler_append_code("\n");
    free(dst);
    free(src);
}

void compiler_set_u8_variable(aal_var_ref var_ref, compiler_array_access_data array_access_data, compiler_value_data value_data)
{
    char* dst;
    char* src;
    dst = switch_compiler_access_array(var_ref, array_access_data);
    add_square_brackets(&dst);

    value_data.register_number = 0;
    src = switch_compiler_value(value_data, TRUE);

    compiler_set_register_byte(dst, src);
    compiler_append_code("\n");
    free(dst);
    free(src);
}


void parse_variable_set(char** curr, aal_var_ref var_ref, char* varname, aal_data_type effective_type)
{
    compiler_value_data value_data;
    compiler_array_access_data access_array_data;
    size_t register_num;
    char* register_name;
    
    access_array_data.type = COMPILER_ACCESS_ARRAY_TYPE_NONE;

    if(**curr == '<')
    {
        (*curr)++;
        value_data = source_parse_expression(curr, AAL_DATA_TYPE_DWORD);
        if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
        {
            access_array_data.type = COMPILER_ACCESS_ARRAY_TYPE_REGISTER;
            compiler_append_code("push %s\n", reg_number_to_reg(value_data.register_number, value_data.register_byte));
        }
        else
        {
            if(value_data.expr.type == EXPR_DATA_TYPE_DATA_TYPE && value_data.expr.data_type == AAL_DATA_TYPE_DWORD)
            {
                access_array_data.type = COMPILER_ACCESS_ARRAY_TYPE_INDEX;
                access_array_data.arr_index = *(dword_t*)value_data.expr.data;
            }
            else
            {
                aalc_error("Bad array subscript, should be dword");
                exit(EXIT_FAILURE);
            }
            
        }
        char_expect((**curr), '>');
        (*curr)++;
    }

    /* <variable>+ or <variable>- */
    if((**curr) == '+' || (**curr) == '-')
    {
        /* use effective type */
        register_num = 0;
        register_name = reg_number_to_reg(register_num, effective_type == AAL_DATA_TYPE_BYTE);
        compiler_append_code("mov %s, [%s]\n", register_name, switch_compiler_access_array(var_ref, access_array_data));
        
        if(**curr == '+')
        {
            compiler_append_code("inc %s\n", register_name);
        }
        else if(**curr == '-')
        {
            compiler_append_code("dec %s\n", register_name);
        }
        

        value_data.type = COMPILER_VALUE_TYPE_REGISTER;
        value_data.register_byte = (effective_type == AAL_DATA_TYPE_BYTE);
        value_data.register_number = register_num;
    }
    else
    {
        char_expect((**curr), ' ');
        (*curr)++;
        char_expect((**curr), '=');
        (*curr)++;
        char_expect((**curr), ' ');
        (*curr)++;

        value_data = source_parse_expression(curr, effective_type);
    }

   
    /* set eax in use */
    if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
    {
        aalc_state->register_usage[value_data.register_number] = TRUE;
    }
    
    if(effective_type == AAL_DATA_TYPE_DWORD)
    {
        compiler_set_u32_variable(var_ref, access_array_data, value_data);
    }
    else if(effective_type == AAL_DATA_TYPE_BYTE)
    {
        compiler_set_u8_variable(var_ref, access_array_data, value_data);
    }

    if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
    {
        drop_register_usage(value_data.register_number);
    }
}

void parse_variable_declaring(aal_data_type data_type, char* current_instruction, char** curr)
{
    char* str2;
    size_t len;
    size_t offset;
    char* varname;

    
    (*curr)++;

    varname = strdupc((*curr), '<', 0);
    (*curr) += strlen(varname);

    if((*curr) < offsetptr_fd(current_instruction, strlen(current_instruction), char) && **curr == '<')
    {
        (*curr)++;
        str2 = strdupc((*curr), '>', 0);
        if((*curr)[strlen(str2)] != '>')
        {
            aalc_error("Expected '>' for the end of array declaring, not '%c'", (*curr)[strlen(str2)]);
            exit(EXIT_FAILURE);
        }
        len = read_uint(str2, &offset);
    }
    else
    {
        len = 1;
    }

    ALLOC_NEW_ELEMENT_FOR_VECTOR(aalc_state->stack_frame.stack_variables, aalc_state->stack_frame.stack_variables_len, sizeof(aal_stack_variable));
    aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].data_size = type2size(data_type) * len;
    aalc_state->stack_frame.stack_frame_size += aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].data_size;
    aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].name = varname;
    aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].type = data_type;
    if(aalc_state->stack_frame.stack_variables_len == 1)
    {
        aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].offset = aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].data_size;
    }
    else
    {
        aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].offset = aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 2].offset + aalc_state->stack_frame.stack_variables[aalc_state->stack_frame.stack_variables_len - 1].data_size;
    }
}