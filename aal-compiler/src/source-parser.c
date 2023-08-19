#include "source-parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "aal-data.h"
#include "compiler-logging.h"
#include "compiler.h"
#include "memory.h"

#include <unistd.h>
#include <sys/types.h>
#include "compiler-operations.h"
#include "expression-parser.h"
#include "preprocessor.h"

char* source_read_instruction(FILE* source_file)
{
    char* instruction;
    char c;
    size_t len;

    len = 0;

    while(1)
    {
        c = fgetc(source_file);
        if(c == ';' || c == '\n')
        {
            break;
        }
        else if(c == '}' || c == '{')
        {
            ALLOC_NEW_ELEMENT_FOR_VECTOR(instruction, len, sizeof(char));
            instruction[len - 1] = c;
            break;
        }
        ALLOC_NEW_ELEMENT_FOR_VECTOR(instruction, len, sizeof(char));
        instruction[len - 1] = c;
    }
    ALLOC_NEW_ELEMENT_FOR_VECTOR(instruction, len, sizeof(char));
    instruction[len - 1] = '\x00';
    return instruction;
}

void char_expect(char c, char expect)
{
    if(c != expect)
    {
        aalc_error("Expected '%c' here, not '%c'", expect, c);
        exit(EXIT_FAILURE);
    }
}

int source_ignore_whitespaces(FILE* source_file)
{
    char c;
    size_t i;
    size_t fp_save;

    for(i = 0 ; ; i++)
    {
        fp_save = ftell(source_file);
        c = getc(source_file);
        
        
        
        if(feof(source_file))
        {
            return 1;
        }
        else if(c == '\n')
        {
            aalc_state->currentline++;
        }
        
        if(c != ' ' && c != '\t' && c != '\n')
        {
            fseek(source_file, fp_save, SEEK_SET);
            return 0;
        }
    }
}

Bool compiler_ignore_comments(char* current_instruction, Bool* comment)
{
    
    if(current_instruction[0] == '/')
    {
        if(current_instruction[1] == '/')
        {
            if(current_instruction[2] == '/')
            {
                if(*comment)
                {
                    *comment = FALSE;
                }
                else
                {
                    *comment = TRUE;
                }
            }
            compiler_append_code("; %s\n", current_instruction);
            return TRUE;
        }
    }
    if(*comment)
    {
        compiler_append_code("; comment: %s\n", current_instruction);
        return TRUE;
    }
    return FALSE;
}

char* read_double_quote(char* data)
{
    size_t size;
    char* original_data;
    size = 0;

    char* str;

    if(*data != '"')
    {
        return NULL;
    }
    data++;
    for(; ;)
    {
        ALLOC_NEW_ELEMENT_FOR_VECTOR(str, size, sizeof(char));

        if(*data == '"')
        {
            str[size - 1] = '\0';
            break;
        }
        else if(*data == '\\')
        {
            data++;
            
            str[size - 1] = parse_double_quote_escape(data);
            if(str[size - 1] == '\0')
            {
                aalc_error("Unknown escape character string quote");
                exit(EXIT_FAILURE);
            }
        }
        else if(*data == '\0')
        {
            aalc_error("Unterminated string quote");
            exit(EXIT_FAILURE);
        }
        else
        {
            str[size - 1] = *data;
        }
        

        data++;
    }

    return str;
}
char read_quote(char* data)
{
    size_t size;
    char* original_data;
    size = 0;

    original_data = data;

    if(*data != '\'')
    {
        return '\x00';
    }
    data++;
    for(; ;)
    {
        if(*data == '\'')
        {
            break;
        }
        else if(*data == '\0')
        {
            aalc_error("Unterminated single quote");
            exit(EXIT_FAILURE);
        }
        data++;
        size++;
    }
    if(size > 1)
    {
        aalc_error("data too big for character : %s\n", original_data++);
        exit(EXIT_FAILURE);
    }
    return *(++original_data);
}
int atoui(char *in, unsigned int *out) 
{
    char *p;

    for(p = in; *p; p++) 
        if (*p > '9' || *p < '0') 
            return 1;

    *out = strtoul(in, NULL, 10);
    return 0;
}
dword_t read_uint(char* data, size_t* len)
{
    dword_t number;
    char* original_data;
    char* end;
    *len = 0;
    int base;
    original_data = data;

    /*if(strncmp(data, "0x", 2) == 0)
    {
        base = 16;
        len += 2;
        data += 2;
    }
    else if(strncmp(data, "0b", 2) == 0)
    {
        base = 2;
        len += 2;
        data += 2;
    }
    else if(strncmp(data, "0o", 2) == 0)
    {
        base = 8;
        len += 2;
        data += 2;
    }
    else if(strncmp(data, "0d", 2) == 0)
    {
        base = 10;
        len += 2;
        data += 2;
    }
    else
    {
        base = 10;
    }*/

    number = (dword_t)strtoul(original_data, &end, 0);
    *len += (unsigned long)(end - data);
    if(*len == 0)
    {
        aalc_error("Couldn't read dword value\n");
        exit(EXIT_FAILURE);
    }
    /*if(atoui(original_data, &number) == 1)
    {
        aalc_error("Could not convert %s to dword", original_data);
        exit(EXIT_FAILURE);
    }*/
    return number;

}
/* reads either a function name or a variable name */
/* stops reading when a non-symbol character is hit */
char* read_symbol_name(char* start)
{
    char* str;
    char* original_start;
    size_t len = 0;
    original_start = start;

    for(; ;)
    {
        ALLOC_NEW_ELEMENT_FOR_VECTOR(str, len, sizeof(char));
        if(!is_symbol_char(*start))
        {
            str[len - 1] = '\0';
            break;
        }
        
        str[len - 1] = *start;
        start++;
    }
    /*if(atoui(original_data, &number) == 1)
    {
        aalc_error("Could not convert %s to dword", original_data);
        exit(EXIT_FAILURE);
    }*/
    return str;
}

aal_data_type switch_dereference_type(char** expression)
{
    if(strncmp(*expression, "d$", 2) == 0)
    {
       (*expression) += 2; 
       return AAL_DATA_TYPE_DWORD;
    }
    else if(strncmp(*expression, "b$", 2) == 0)
    {

       (*expression) += 2; 
       return AAL_DATA_TYPE_BYTE;
    }
    else
    {
        return AAL_DATA_TYPE_NONE;
    }
}


expr_data source_read_next_data(char** data)
{
    size_t len;
    len = 0;
    ssize_t index;
    char* string;
    char* symbol_name;
    char c;
    dword_t numberu32;
    expr_data expr;
    if(**data == '"')
    {
        string = read_double_quote(*data);
        expr.data = string;
        expr.data_size = strlen(string);
    
        expr.type = EXPR_DATA_TYPE_STRING_LITERRAL;
        
        len = strlen(string) + 2;
    }
    else if(**data == '\'')
    {
        c = read_quote(*data);
        expr.data = malloc(1);
        *(char*)expr.data = c;
        expr.data_size = 1;
        expr.type = EXPR_DATA_TYPE_STRING_CHARACTER;
        len = strlen(string) + 2;
    }
    else if(is_digit(**data))
    {
        expr.data_size = sizeof(dword_t);
        expr.data = malloc(expr.data_size);
        *(dword_t*)expr.data = read_uint(*data, &len);
        expr.type = EXPR_DATA_TYPE_DATA_TYPE;
        expr.data_type = AAL_DATA_TYPE_DWORD;
    }
    else
    {
        symbol_name = read_symbol_name(*data);
        index = macro_index(symbol_name);
        if(index != -1)
        {
            expr.data_size = sizeof(dword_t);
            expr.data = malloc(expr.data_size);
            *(dword_t*)expr.data = aalc_state->macros[index].value;
            len = strlen(aalc_state->macros[index].name);
            printf("len : %lu\n", len);
            expr.type = EXPR_DATA_TYPE_DATA_TYPE;
            expr.data_type = AAL_DATA_TYPE_DWORD;
        }
        else
        {
            expr.type = EXPR_DATA_TYPE_NONE;
        }
        
    }
    *data = offsetptr_fd(*data, len, char);
    return expr;
}


aal_stack_variable* read_function_parameters(char** start, size_t* parameters_len)
{
    size_t i;
    aal_data_type type;
    aal_stack_variable* parameters;
    char* str;
    char* type_str;
    char stop_characters[] = " )";
    char c;

    *parameters_len = 0;

    for(i = 0 ; ; i++)
    {
        if(**start == ')')
        {
            return NULL;
        }
        type_str = strdupc(*start, ' ', 0);
        type = str2type(type_str);
        
        if(type == AAL_DATA_TYPE_ERROR)
        {
            aalc_error("Unkown type\n");
            exit(EXIT_FAILURE);
        }
        *start += strlen(type_str);
        if(**start == ')')
        {
            aalc_error("Cannot close parentesis on type");
            exit(EXIT_FAILURE);
        }
        char_expect(**start, ' ');
        (*start)++;
        str = read_symbol_name(*start);
        *start += strlen(str);
        ALLOC_NEW_ELEMENT_FOR_VECTOR(parameters, *parameters_len, sizeof(aal_stack_variable));
        parameters[*parameters_len - 1].name = str;
        parameters[*parameters_len - 1].type = type;
        parameters[*parameters_len - 1].data_size = type2size(type);

        if(**start == ')')
        {
            return parameters;
        }
        else if(**start == ' ')
        {
            (*start)++;
            continue;
        }
        else
        {
            aalc_error("Invalid character");
            exit(EXIT_FAILURE);
        }
    }
}





/*
    Loads the expression to register

    Example :
    mov eax, [esp+0x10]
    add eax, 2
*/




void parse_syscall_registers(char** expression)
{
    compiler_value_data value_data;
    size_t i;
    for(i = 0 ; i < 4 ; i++)
    {
        compiler_append_code("; set %s\n", reg_number_to_dword_reg(i));
        value_data = source_parse_expression_core(expression, i, FALSE, AAL_DATA_TYPE_DWORD);
        if(value_data.type == COMPILER_VALUE_TYPE_EXPRESSION)
        {
            compiler_set_register_dword(reg_number_to_dword_reg(i), switch_compiler_value(value_data, FALSE));
        }
        compiler_append_code("\n");
        aalc_state->register_usage[i] = TRUE;
        if(i < (4 - 1))
        {
            char_expect(**expression, ' ');
        }
        
        (*expression)++;
    }
    for(i = 0 ; i < 4 ; i++)
    {
        drop_register_usage(i);
    }
}

/* parse arguments and push them in reverse order */
compiler_value_data parse_push_arguments(char** curr, size_t function_index)
{
    char* str;
    stack arguments_stack;
    compiler_value_data value_data;
    size_t arg_num = 0;
    arguments_stack = create_stack();
    char stop_characters[] = " )";

    if(**curr == ')')
    {
        return value_data;
    }

    for(; **curr != '\x00' ;)
    {
        str = strdupc_multi(*curr, stop_characters, NULL, 1);  
        stack_push(&arguments_stack, get_as_stack_data(str, strlen(str) + 1));
        (*curr) += strlen(str);
        if(**curr == ')')
        {
            break;
        } 
        else if(**curr == '\x00')
        {
            aalc_error("Unterminated function call parentesis");
            exit(EXIT_FAILURE);
        }

        char_expect(**curr, ' ');
        (*curr)++;
    }

    if(arguments_stack.size != aalc_state->functions[function_index].paremeters_len)
    {
        aalc_error("Invalid number of arguments for function '%s' (expected %lu arguments but %lu were specified)", aalc_state->functions[function_index].name, aalc_state->functions[function_index].paremeters_len, arguments_stack.size);
        exit(EXIT_FAILURE);
    }

    while(arguments_stack.top > -1)
    {
        str = stack_top(arguments_stack, char);

        compiler_append_code("; argument %s\n", str);
        value_data = source_parse_expression(&str, aalc_state->functions[function_index].parameters[arguments_stack.top].type);
        
        if(aalc_state->functions[function_index].parameters[arg_num].type == AAL_DATA_TYPE_DWORD)
        {
            compiler_append_code("push %s\n", switch_compiler_value(value_data, FALSE));
        }
        else if(aalc_state->functions[function_index].parameters[arg_num].type == AAL_DATA_TYPE_BYTE)
        {
            compiler_append_code("sub esp, 1\n");
            compiler_append_code("mov byte [esp], %s\n\n", switch_compiler_value(value_data, TRUE));
        }
        else
        {
            aalc_error("Wrong expression for argument %lu\n", arg_num);
            exit(EXIT_FAILURE);
        }
        

        stack_pop(&arguments_stack);
        arg_num++;
    }

    free_stack(&arguments_stack);

    return value_data;
}