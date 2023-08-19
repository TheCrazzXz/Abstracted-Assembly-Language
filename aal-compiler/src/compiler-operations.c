#include "compiler-operations.h"
#include "aal-data.h"
#include "compiler-logging.h"
#include "compiler.h"
#include "stack.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

char* reg_number_to_dword_reg(size_t n)
{
    if(n == 0)
    {
        return "eax";
    }
    else if(n == 1)
    {
        return "ebx";
    }
    else if(n == 2)
    {
        return "ecx";
    }
    else if(n == 3)
    {
        return "edx";
    }
    else
    {
        return NULL;
    }
}

char* reg_number_to_byte_reg(size_t n, char low_or_high)
{
    char* reg;
    char* dword_reg;
    reg = malloc(3);
    dword_reg = reg_number_to_dword_reg(n);
    reg[0] = dword_reg[1];
    reg[1] = low_or_high ? 'h' : 'l';
    reg[2] = '\0';
    
    return reg;
}

char* reg_number_to_reg(size_t n, Bool byte_register)
{
    if(byte_register)
    {
        return reg_number_to_byte_reg(n, 0);
    }
    else
    {
        return reg_number_to_dword_reg(n);
    }
}

char switch_sign(aal_var_ref var_ref)
{
    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        return '+';
    }
    else
    {
        return '-';
    }
}

aal_stack_variable* get_stack_var_from_var_ref(aal_var_ref var_ref)
{
    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME)
    {
        return &aalc_state->stack_frame.stack_variables[var_ref.index];
    }
    else if(var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        return &aalc_state->functions[aalc_state->current_function_index].parameters[var_ref.index];
    }
    else
    {
        fprintf(stderr, "Error : Can't get stack var from var ref\n");
        exit(EXIT_FAILURE);
        return 0;
    }
}

expr_data* get_data_var_from_var_ref(aal_var_ref var_ref)
{
    if(var_ref.type != AAL_VAR_REF_TYPE_DATA_LABEL)
    {
        fprintf(stderr, "Error : Can't get data var from var ref\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    return &aalc_state->data_seg_data[var_ref.index];
}

char* get_array_index_from_register_code(aal_var_ref var_ref, size_t register_index)
{
    char* str;
    char* register_name = reg_number_to_dword_reg(register_index);
    aal_stack_variable* stack_var;
    if(register_name == NULL)
    {
        fprintf(stderr, "Compilator error : Bad register\n");
        exit(EXIT_FAILURE);
    }
    
    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME || var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        str = malloc(strlen("ebp+*-") + 16 * 2 + strlen(register_name) + 1);
        stack_var = get_stack_var_from_var_ref(var_ref);
        sprintf(str, "ebp+%s*%lu%c%lu", register_name, type2size(stack_var->type), switch_sign(var_ref), stack_var->offset);
    }
    else if(var_ref.type == AAL_VAR_REF_TYPE_DATA_LABEL)
    {
        str = malloc(strlen(get_data_var_from_var_ref(var_ref)->name) + strlen("+*") + strlen(register_name) + 16 + 1);
        sprintf(str, "%s+%s*%lu", get_data_var_from_var_ref(var_ref)->name, register_name, type2size(aalc_state->data_seg_data[var_ref.index].data_type));
    }

    return str;
}
char* get_array_index_code(aal_var_ref var_ref, size_t index)
{
    char* str;
    aal_stack_variable* stack_var;
    
    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME || var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        str = malloc(strlen("ebp+") + 16 + 1);
        stack_var = get_stack_var_from_var_ref(var_ref);
        sprintf(str, "ebp%c%lu", switch_sign(var_ref), stack_var->offset - index * type2size(stack_var->type));
    }
    else if(var_ref.type == AAL_VAR_REF_TYPE_DATA_LABEL)
    {
        str = malloc(strlen("+") + strlen(get_data_var_from_var_ref(var_ref)->name) + 16 + 1);
        sprintf(str, "%s+%lu", get_data_var_from_var_ref(var_ref)->name, index * type2size(aalc_state->data_seg_data[var_ref.index].data_type));
    }
        
    return str;
}
char* get_variable_code(aal_var_ref var_ref)
{
    char* str;
    aal_stack_variable* stack_var;
    if(var_ref.type == AAL_VAR_REF_TYPE_STACK_FRAME || var_ref.type == AAL_VAR_REF_TYPE_STACK_PARAMETER)
    {
        str = malloc(strlen("ebp+") + 16 + 1);
        stack_var = get_stack_var_from_var_ref(var_ref);
        sprintf(str, "ebp%c%lu",switch_sign(var_ref), stack_var->offset);
    }
    else if(var_ref.type == AAL_VAR_REF_TYPE_DATA_LABEL)
    {
        str = malloc(strlen(get_data_var_from_var_ref(var_ref)->name) + 1);
        sprintf(str, "%s", get_data_var_from_var_ref(var_ref)->name);
    }

    return str;
}

void add_square_brackets(char** str)
{
    char* old_str = *str;
    size_t len;
    *str = malloc(strlen(old_str) + 3);
    strcpy(offsetptr_fd(*str, 1, char), old_str);
    (*str)[0] = '[';

    len = strlen(*str);
    (*str)[len] = ']';
    (*str)[len + 1] = '\0';
    free(old_str);
}

aal_operation character_to_operation(char* data)
{
    if(*data == '+')
    {
        return AAL_OPERATION_ADDITION;
    }
    else if(*data == '-')
    {
        return AAL_OPERATION_SUBTRACTION;
    }
    else if(*data == '*')
    {
        return AAL_OPERATION_MULTIPLICATION;
    }
    else if(*data == '/')
    {
        return AAL_OPERATION_DIVISION;
    }
    else if(*data == '%')
    {
        return AAL_OPERATION_MODULO;
    }
    else if(*data == '~')
    {
        return AAL_OPERATION_BITWISE_AND;
    }
    else if(*data == '^')
    {
        return AAL_OPERATION_BITWISE_XOR;
    }
    else if(*data == '|')
    {
        return AAL_OPERATION_BITWISE_OR;
    }
    else
    {
        return AAL_OPERATION_NOT_SPECIFIC;
    }
}

void register_operation(aal_operation operation, char* register_name, char* var_asm, char* type)
{
    size_t otherreg;
    size_t num;
    double logarithm;
    size_t power;
    Bool pow_of_2 = FALSE;
    if(operation == AAL_OPERATION_ADDITION)
    {
        compiler_operate_register("add", register_name, var_asm, type);
    }

    else if(operation == AAL_OPERATION_SUBTRACTION)
    {
        compiler_operate_register("sub", register_name, var_asm, type);
    }

    else if(operation == AAL_OPERATION_MULTIPLICATION || operation == AAL_OPERATION_DIVISION || operation == AAL_OPERATION_MODULO)
    {
        if(is_digit(var_asm[0]))
        {
            num = atol(var_asm);
            logarithm = log2((double)num);
            power = (size_t)logarithm;
            pow_of_2 = (num != 0) && ((double)power == logarithm);
        }
        
        if(!pow_of_2 || operation == AAL_OPERATION_MODULO)
        {
            otherreg = find_and_use_usable_register_number();
            register_name = reg_number_to_dword_reg(otherreg);
            compiler_operate_register("mov", register_name, var_asm, type);
        }
        
        if(operation == AAL_OPERATION_MULTIPLICATION)
        {
            /* a * 2**n = a << n */
            if(pow_of_2)
            {
                compiler_append_code("shl %s, %lu\n", register_name, power);
            }
            else
            {
                compiler_append_code("mul %s\n", register_name);
            }
            
        }
        else if(operation == AAL_OPERATION_DIVISION)
        {
            /* a * 2**n = a >> n */
            if(pow_of_2)
            {
                compiler_append_code("shr %s, %lu\n", register_name, power);
            }
            else
            {
                if(aalc_state->register_usage[3] == TRUE)
                {
                    aalc_error("Cannot compute division because EDX is used");
                    exit(EXIT_FAILURE);
                }
                compiler_append_code("xor edx, edx\n");
                compiler_append_code("div %s\n", register_name);
            }
            
            
        }
        else if(operation == AAL_OPERATION_MODULO)
        {
            if(aalc_state->register_usage[3] == TRUE)
            {
                aalc_error("Cannot compute modulus because EDX is used");
                exit(EXIT_FAILURE);
            } 
            compiler_append_code("xor edx, edx\n");
            compiler_append_code("div %s\n", register_name);
            compiler_set_register_dword("eax", "edx");
        }
        if(!pow_of_2 || operation == AAL_OPERATION_MODULO)
        {
            drop_register_usage(otherreg);
        }
        
    }
    else if(operation == AAL_OPERATION_BITWISE_AND)
    {
        compiler_operate_register("and", register_name, var_asm, type);
    }
    else if(operation == AAL_OPERATION_BITWISE_XOR)
    {
        compiler_operate_register("xor", register_name, var_asm, type);
    }
    else if(operation == AAL_OPERATION_BITWISE_OR)
    {
        compiler_operate_register("or", register_name, var_asm, type);
    }
    else if(operation == AAL_OPERATION_NOT_SPECIFIC)
    {
        if(strcmp(var_asm, "0") == 0)
        {
            compiler_operate_register("xor", register_name, register_name, type);
        }
        else
        {
            compiler_operate_register("mov", register_name, var_asm, type);
        }
        
    }
    
}

char* get_je_jne_instruction(Bool activated)
{
    if(activated)
    {
        return "je";
    }
    else
    {
        return "jne";
    }
}

char* get_ja_jb_instruction(Bool activated, Bool type, Bool equal)
{
    char str[5];
    if(type) /* ja*/
    {
        if(activated)
        {
            strcpy(str, "ja");
        }
        else
        {
            strcpy(str, "jbe");
        }
    }
    else /* jb */
    {
        if(activated)
        {
            strcpy(str, "jb");
        }
        else
        {
            strcpy(str, "jae");
        }
    }
    if(equal) /* e */
    {
        if(activated)
        {
            str[strlen(str)] = 'e';
        }
        else /* not e */
        {
            str[strlen(str) - 1] = '\0';
        }
    }
    return strdup(str);
}

char* get_unsigned_conditionnal_jump_instruction(expr_data_type type, Bool activated)
{
    if(type == EXPR_DATA_TYPE_EQUAL_COMPARAISON)
    {
        return get_je_jne_instruction(activated);
    }
    else if(type == EXPR_DATA_TYPE_DIFFERENT_COMPARAISON)
    {
        return get_je_jne_instruction(!activated);
    }
    else if(type == EXPR_DATA_TYPE_ABOVE_COMPARAISON)
    {
        return get_ja_jb_instruction(activated, 1, FALSE);
    }
    else if(type == EXPR_DATA_TYPE_LESS_COMPARAISON)
    {
        return get_ja_jb_instruction(activated, 0, FALSE);
    }
    else if(type == EXPR_DATA_TYPE_ABOVE_EQUAL_COMPARAISON)
    {
        return get_ja_jb_instruction(activated, 1, TRUE);
    }
    else if(type == EXPR_DATA_TYPE_LESS_EQUAL_COMPARAISON)
    {
        return get_ja_jb_instruction(activated, 0, TRUE);
    }
    else
    {
        return get_je_jne_instruction(activated);
    }
    return NULL;
}

/* 
    Jump when true if activated, jump when false if not activated
*/
void compiler_append_cond(compiler_value_data value_data, char* dest, Bool activated)
{
    if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
    {
        if(value_data.expr.type >= EXPR_DATA_TYPE_EQUAL_COMPARAISON && value_data.expr.type <= EXPR_DATA_TYPE_EQUAL_COMPARAISON+5)
        {
            
            compiler_append_code("cmp %s, %s\n", reg_number_to_reg(0, value_data.register_byte), reg_number_to_reg(1, value_data.register_byte));
            compiler_append_code("%s %s\n\n", get_unsigned_conditionnal_jump_instruction(value_data.expr.type, activated), dest);
        }
        else
        {
            compiler_append_code("cmp %s, 0\n", reg_number_to_reg(0, value_data.register_byte));
            compiler_append_code("%s %s\n\n", get_unsigned_conditionnal_jump_instruction(value_data.expr.type, !activated), dest);
        }
    }
    
    else
    {
        compiler_append_code("mov eax, %s\n", switch_compiler_value(value_data, FALSE));
        compiler_append_code("cmp eax, 0\n");
        compiler_append_code("%s %s\n\n", get_unsigned_conditionnal_jump_instruction(value_data.expr.type, !activated), dest);
    }
}

void end_condition(stack* cond_stack)
{
    if(is_stack_empty(*cond_stack))
    {
        aalc_error("Can't end condition because there's no condition to end");
        exit(EXIT_FAILURE);
    }
    if(aalc_state->asm_file_padding > 0)
    {
        aalc_state->asm_file_padding--;
    }
    
    if(!((stack_top(*cond_stack, condition_stack_frame))->fail_defined))
    {
        compiler_append_code("%s_fail_condition%lu:\n", current_function.name, (stack_top(*cond_stack, condition_stack_frame))->number);
    }
    else
    {
        if(aalc_state->asm_file_padding > 0)
        {
            aalc_state->asm_file_padding--;
        }
    }
    compiler_append_code("%s_end_condition%lu:\n", current_function.name, (stack_top(*cond_stack, condition_stack_frame))->number);
    (stack_top(*cond_stack, condition_stack_frame))->fail_defined = FALSE;
    stack_pop(cond_stack);
}

char* switch_compiler_access_array(aal_var_ref var_ref, compiler_array_access_data array_access_data)
{
    char* str;
    char* reg_name;
    size_t reg_num;
    if(array_access_data.type == COMPILER_ACCESS_ARRAY_TYPE_REGISTER)
    {
        compiler_append_code("pop ebx\n");
        str = get_array_index_from_register_code(var_ref, 1);
    }
    else if(array_access_data.type == COMPILER_ACCESS_ARRAY_TYPE_INDEX)
    {
        str = get_array_index_code(var_ref, array_access_data.arr_index);
    }
    else
    {
        str = get_variable_code(var_ref);
    }

    if(var_ref.dereference_type != AAL_DATA_TYPE_NONE)
    {
        reg_num = find_usable_register_number();
        reg_name = reg_number_to_dword_reg(reg_num);
        
        if(var_ref.dereference_type == AAL_DATA_TYPE_DWORD || var_ref.dereference_type == AAL_DATA_TYPE_BYTE)
        {
            compiler_append_code("mov %s, [%s]\n", reg_name, str);
            return strdup(reg_name);
        }
        else
        {
            aalc_error("Cannot derefence pointer");
            exit(EXIT_FAILURE);
        }
    }

    return str;
}

char* switch_compiler_value(compiler_value_data value_data, Bool force_byte)
{
    char* ret;
    if(value_data.type == COMPILER_VALUE_TYPE_REGISTER)
    {
        ret = strdup(reg_number_to_reg(value_data.register_number, value_data.register_byte || force_byte));
    }
    else if(value_data.type == COMPILER_VALUE_TYPE_EXPRESSION)
    {
        if(value_data.expr.type == EXPR_DATA_TYPE_DATA_TYPE)
        {
            if(value_data.expr.data_type == AAL_DATA_TYPE_DWORD)
            {
                ret = malloc(16);
                sprintf(ret, "%u", *(dword_t*)value_data.expr.data);
            }
            else if(value_data.expr.data_type == AAL_DATA_TYPE_BYTE)
            {
                ret = malloc(4);
                sprintf(ret, "%u", *(dword_t*)value_data.expr.data);
            }
            else
            {
                aalc_error("Cannot affect this value to the variable");
                exit(EXIT_FAILURE);
            }
            
        }
        else if(value_data.expr.type == EXPR_DATA_TYPE_DATA_VAR_NAME)
        {
            ret = malloc(strlen(value_data.expr.data));
            strcpy(ret, value_data.expr.data);
        }
        else
        {
            aalc_error("Cannot affect this value to the variable");
            exit(EXIT_FAILURE);
        }
    }
    return ret;
}

aal_data_type switch_effective_type(aal_data_type dereference_type, aal_var_ref var_ref)
{
    if(dereference_type == AAL_DATA_TYPE_NONE)
    {
        return var_ref.data_type;
    }
    else
    {
        return dereference_type;
    }
}