#include "expression-parser.h"
#include "aal-data.h"
#include "aal-functions.h"
#include "memory.h"
#include "source-parser.h"
#include "compiler.h"
#include "compiler-operations.h"
#include <stdlib.h>
#include "compiler-logging.h"
#include "aal-variables.h"


expr_data_type str2comparaison_type(char** str)
{
    if(strncmp(*str, "==", 2) == 0)
    {
        *str += 2;
        return EXPR_DATA_TYPE_EQUAL_COMPARAISON;
    }
    else if(strncmp(*str, "!=", 2) == 0)
    {
        *str += 2;
        return EXPR_DATA_TYPE_DIFFERENT_COMPARAISON;
    }
    else if(strncmp(*str, ">=", 2) == 0)
    {
        *str += 2;
        return EXPR_DATA_TYPE_ABOVE_EQUAL_COMPARAISON;
    }
    else if(strncmp(*str, "<=", 2) == 0)
    {
        *str += 2;
        return EXPR_DATA_TYPE_LESS_EQUAL_COMPARAISON;
    }
    else if(**str == '<')
    {
        (*str)++;
        return EXPR_DATA_TYPE_LESS_COMPARAISON;
    }
    else if(**str == '>')
    {
        (*str)++;
        return EXPR_DATA_TYPE_ABOVE_COMPARAISON;
    }
    else
    {
        return EXPR_DATA_TYPE_NONE;
    }
    
}

compiler_value_data source_parse_expression_core(char** expression, ssize_t reg_number, Bool direct, aal_data_type dst_type)
{
    size_t expression_len;

    expr_data current_data;
    expr_data final_data;
    char* var_asm;
    char* symbol_name;
    aal_data_type type;
    expr_data_type expr_type;
    size_t len;
    size_t reg_temp_number;
    ssize_t final_sum = 0;
    ssize_t function_index;
    aal_operation op = AAL_OPERATION_NOT_SPECIFIC;
    aal_operation temp_op = AAL_OPERATION_NOT_SPECIFIC;
    size_t fp_save = ftell(aalc_state->asm_file);
    Bool only_numbers = TRUE;
    char* byte_reg;
    compiler_value_data value_data;
    Bool used_new_register = FALSE;
    Bool set_in_place;
    aal_data_type convert_type = AAL_DATA_TYPE_NONE;
    char* type_str;

    //expression_len = strlen(*expression);
    value_data.register_byte = FALSE;
    value_data.expr.type = EXPR_DATA_TYPE_NONE;

    printf("expression : %s\n", *expression);

    if(strncmp(*expression, "RET", 3) == 0)
    {
        value_data.type = COMPILER_VALUE_TYPE_REGISTER;
        value_data.register_number = 0;
        value_data.register_byte = 0;
        return value_data;
    }

    if(**expression != '[' && !direct)
    {
        value_data.expr = source_read_next_data(expression);
        if(value_data.expr.type == EXPR_DATA_TYPE_NONE)
        {
            aalc_error("Expression has error");
            exit(EXIT_FAILURE);
        }
        else
        {

            value_data.type = COMPILER_VALUE_TYPE_EXPRESSION;
            if(value_data.expr.type == EXPR_DATA_TYPE_STRING_LITERRAL)
            {
                ALLOC_NEW_ELEMENT_FOR_VECTOR(aalc_state->data_seg_data, aalc_state->data_seg_data_len, sizeof(expr_data));
                value_data.expr.name = data_seg_idx_to_name(aalc_state->data_seg_data_len - 1);
                aalc_state->data_seg_data[aalc_state->data_seg_data_len - 1] = value_data.expr;
                value_data.expr.type = EXPR_DATA_TYPE_DATA_VAR_NAME;
                value_data.expr.data = value_data.expr.name;
            }
            else if(value_data.expr.type == EXPR_DATA_TYPE_STRING_CHARACTER)
            {
                value_data.expr.type = EXPR_DATA_TYPE_DATA_TYPE;
                value_data.expr.data_type = AAL_DATA_TYPE_BYTE;
            }
            return value_data;
        }
    }

    if(!direct)
    {
        (*expression)++;
    }
    
    if(reg_number == -1)
    {
        reg_number = (ssize_t)find_and_use_usable_register_number();

        
        used_new_register = TRUE;
    }
    

    for(; ;)
    {
        
        current_data = source_read_next_data(expression);
        if(current_data.type != EXPR_DATA_TYPE_NONE)
        {
            if(current_data.type != EXPR_DATA_TYPE_DATA_TYPE || current_data.data_type != AAL_DATA_TYPE_DWORD)
            {
                only_numbers = FALSE;
            }

            if(current_data.type == EXPR_DATA_TYPE_DATA_TYPE && current_data.data_type == AAL_DATA_TYPE_DWORD)
            {
                var_asm = malloc(16);
                sprintf(var_asm, "%u", *(dword_t*)current_data.data);
                register_operation(op, reg_number_to_dword_reg(reg_number), var_asm, "dword");
                
                if(op == AAL_OPERATION_ADDITION)
                {
                    final_sum += *(dword_t*)current_data.data;
                }
                else if(op == AAL_OPERATION_SUBTRACTION)
                {
                    final_sum -= *(dword_t*)current_data.data;
                }
                else if(op == AAL_OPERATION_MULTIPLICATION)
                {
                    final_sum *= *(dword_t*)current_data.data;
                }
                else if(op == AAL_OPERATION_DIVISION)
                {
                    final_sum /= *(dword_t*)current_data.data;
                }
                if(op == AAL_OPERATION_NOT_SPECIFIC)
                {
                    final_sum = *(dword_t*)current_data.data;
                }
                
                op = AAL_OPERATION_NOT_SPECIFIC;
                free(var_asm);
            }
            
        }
        else
        {
            type = dereference_variable(expression, &var_asm, reg_number, op, &set_in_place, dst_type);
            
            
            if(var_asm != NULL)
            {
                if(type == AAL_DATA_TYPE_BYTE)
                {
                    value_data.register_byte = TRUE;
                }
                only_numbers = FALSE;
                /*if(type == AAL_DATA_TYPE_DWORD)
                {
                    compiler_set_register("eax", var_asm);
                }
                else if(type == AAL_DATA_TYPE_BYTE)
                {
                    compiler_set_register_byte("al", var_asm);
                }*/
                if(!set_in_place)
                {
                    if(type == AAL_DATA_TYPE_DWORD)
                    {
                        register_operation(op, reg_number_to_dword_reg(reg_number), var_asm, "dword");
                    }
                    else if(type == AAL_DATA_TYPE_BYTE)
                    {
                        
                        byte_reg = reg_number_to_byte_reg(reg_number, 0);
                        register_operation(op, byte_reg, var_asm, "byte");
                        
                        free(byte_reg);
                    }
                }
                convert_type = AAL_DATA_TYPE_NONE;
                free(var_asm);
            }
            else
            {
                symbol_name = read_symbol_name(*expression);
                    
                function_index = function_name_to_index(symbol_name);
                if(function_index != -1)
                {
                    *expression += strlen(aalc_state->functions[function_index].name);
                    
                    if(op == AAL_OPERATION_NOT_SPECIFIC)
                    {
                        drop_register_usage(reg_number);
                        parse_function_call(function_index, expression);
                        aalc_state->register_usage[reg_number] = TRUE;
                    }
                    else
                    {
                        reg_temp_number = find_and_use_usable_register_number();
                        compiler_append_code("mov %s, %s\n", reg_number_to_dword_reg(reg_temp_number), reg_number_to_dword_reg(reg_number));
                        parse_function_call(function_index, expression);
                        register_operation(op, reg_number_to_dword_reg(reg_temp_number), "eax", "dword");
                        compiler_append_code("mov %s, %s\n", reg_number_to_dword_reg(reg_number), reg_number_to_dword_reg(reg_temp_number));
                        drop_register_usage(reg_temp_number);
                    }
                    
                }
                else
                {
                    temp_op = character_to_operation(*expression);
                    if(temp_op != AAL_OPERATION_NOT_SPECIFIC)
                    {
                        
                        op = temp_op;
                    }
                    else
                    {
                        expr_type = str2comparaison_type(expression);
                        if(expr_type != EXPR_DATA_TYPE_NONE)
                        {
                            source_parse_expression_core(expression, -1, TRUE, dst_type);

                            // register_operation(AAL_OPERATION_SUBTRACTION, "eax", "ebx", "dword");
                            value_data.expr.type = expr_type;
                            continue;
                        }
                        else
                        {
                            if(**expression == ' ' || **expression == '\t')
                            {

                            }
                            
                            else if(**expression == ']')
                            {
                                /* TO DO : ADD OPTIMISATION WITH FINAL SUM IF ONLY NUMBERS */
                                
                                // printf("final sum = %ld", final_sum);
                                if(!direct)
                                {
                                    (*expression)++;
                                }
                                
                                break;
                            }
                            /* cast */
                            else if(**expression == '(')
                            {
                                type_str = strdupc(*expression, ')', 0);
                                *expression += strlen(type_str);
                                char_expect(**expression, ')');
                                type = str2type(type_str);
                                if(type == AAL_DATA_TYPE_ERROR)
                                {
                                    aalc_error("Unknown cast type : '%s'", type_str);
                                    exit(EXIT_FAILURE);
                                }
                                convert_type = type;
                            }
                            else if(**expression == '\0')
                            {
                                aalc_error("Unterminated expression");
                                exit(EXIT_FAILURE);
                            }
                            else
                            {
                                aalc_error("Unknown character for expression : '%c'", **expression);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                
                
                
                (*expression)++;
            }
        }
        
    }
    value_data.type = COMPILER_VALUE_TYPE_REGISTER;
    value_data.register_number = (size_t)reg_number;
    if(dst_type != AAL_DATA_TYPE_NONE)
    {
        value_data.register_byte = dst_type == AAL_DATA_TYPE_BYTE;
    }
    
    if(used_new_register)
    {
        drop_register_usage(reg_number);
    }
    
    return value_data;
}

compiler_value_data source_parse_expression(char **expression, aal_data_type dst_type)
{
    return source_parse_expression_core(expression, -1, FALSE, dst_type);
}