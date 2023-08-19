#include "compiler.h"
#include <bits/types/FILE.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aal-data.h"
#include "aal-functions.h"
#include "compiler-logging.h"
#include "numstack.h"
#include "preprocessor.h"
#include "source-parser.h"
#include "memory.h"
#include "compiler-syscalls.h"
#include "compiler-operations.h"
#include <stdarg.h>
#include "stack.h"
#include "expression-parser.h"
#include "aal-variables.h"



char* main_code = "[bits 32]\nsection .text\nglobal _start\n";
char* exit_code = "mov eax, 1\n\tmov ebx, 0\n\tint 0x80\n";

compiler_state* aalc_state;


void compiler_append_code(const char* format, ...)
{
    size_t i;
    va_list    args;
    va_start(args, format);

    if(strcmp(format, "\n"))
    {
        for(i = 0 ; i < aalc_state->asm_file_padding ; i++)
        {
            fputc('\t', aalc_state->asm_file);
            printf("\t");
        }
    }

    if(aalc_state->verbose)
    {
        vfprintf(stdout, format, args);

        va_end(args);
    }
    

    va_start(args, format);
    vfprintf(aalc_state->asm_file, format, args);
    

    va_end(args);
}

stack_data get_cond_frame_stack_data(condition_stack_frame sf)
{
    return get_as_stack_data(&sf, sizeof(condition_stack_frame));
}


int convert_to_assembly_source(FILE* source, FILE* asm_file, char* source_name, Bool verbose, Bool main_file)
{
    //char* currinst;
    //char* currchr;
    char* data;
    char* datatype_str;
    // char* str;
    char* str2;
    char* varname;
    char* funname;
    char stop_characters[] = ":(";
    char c;
    // expr_data expr;
    aal_data_type data_type;
    aal_data_type dereference_type;
    aal_data_type effective_type;
    size_t len;
    size_t offset;
    size_t size;
    ssize_t index;
    ssize_t index2;
    size_t arr_index;
    size_t offset_save;
    size_t condition_stack_frame_counter = 0;
    size_t loop_counter = 0;
    aal_stack_variable* parameters;
    Bool defining_function_stack;
    compiler_array_access_data access_array_data;
    compiler_value_data value_data;
    stack cond_stack;
    numstack loop_stack;
    condition_stack_frame stack_frame;
    aal_var_ref var_ref;
    stack arguments_stack;
    int source_ignore_whitespaces_ret;
    numstack currly_brackets_stack;
    size_t last_defined = 0;
    size_t i;
    Bool comment = FALSE;
    Bool defined_var = FALSE;
    
    if(main_file)
    {
        aalc_state = malloc(sizeof(compiler_state));
        memset(aalc_state, 0, sizeof(compiler_state));
    }

    aalc_state->asm_file = asm_file;
    aalc_state->source_file = source;
    aalc_state->current_source_file_name = source_name;
    aalc_state->currentline = 0;

    if(main_file)
    {
        aalc_state->current_function_index = -1;
        aalc_state->verbose = verbose;
        compiler_append_code(main_code);
        
        aalc_state->asm_file_padding = 1;
    }

    

    cond_stack = create_stack();
    loop_stack = create_numstack();
    currly_brackets_stack = create_numstack();

    preprocess_code(source, asm_file, verbose);

    while(!feof(source))
    {
        
        source_ignore_whitespaces_ret = source_ignore_whitespaces(source);
        
        /* if code is finished, stop */
        if(source_ignore_whitespaces_ret == 1)
        {
            break;
        }
        
        currinst = source_read_instruction(source);
        aalc_state->currentline++;

        if(compiler_ignore_comments(currinst, &comment) == TRUE)
        {
            continue;
        }

        currchr = currinst;

        data_type = str2type(read_symbol_name(currchr));

        /* if not defining */
        if(aalc_state->current_function_index != -1 && aalc_state->stack_frame.defining_stack_frame && (data_type == AAL_DATA_TYPE_ERROR) && !aalc_state->stack_frame.submitted_stack_frame)
        {
            compiler_set_stackframe(aalc_state->stack_frame.stack_frame_size);
        }

        printf("current instruction : %s\n", currinst);

        
        compiler_append_code("; %s\n", currinst);

        if(!is_stack_empty(cond_stack))
        {
            if((stack_top(cond_stack, condition_stack_frame))->pending_end && strcmp(currchr, "fail"))
            {
                end_condition(&cond_stack);
            }
        }
        
        if(strncmp(currchr, "func", strlen("func")) == 0)
        {
            currchr = offsetptr_fd(currchr, strlen("func"), char); // func
            char_expect(*(currchr), ' ');
            currchr++; // " "
            

            funname = strdupc_multi(currchr, stop_characters, &c, 0);
            currchr += strlen(funname); // name
            /* declare function */
            if(c == '(')
            {
                currchr++; // " "

                parameters = read_function_parameters(&currchr, &len);

                compiler_declare_function(funname, parameters, len);
                printf("declared function\n");
                aalc_state->current_function_index = -1;
            }
            /* define function */
            else if(c == ':')
            {
                index = function_name_to_index(funname);
                if(index == -1)
                {
                    aalc_error("Function '%s' was not declared\n", funname);
                    exit(EXIT_FAILURE);
                }
                compiler_define_function((size_t)index);
                last_defined = 4;
                
            }
            else
            {
                aalc_error("Unknown character after function name : %c", c);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if(aalc_state->current_function_index == -1)
            {
                aalc_error("Cannot define code outside a function");
                exit(EXIT_FAILURE);
            }
            else if(strncmp(currchr, "syscall", 7) == 0)
            {
                currchr = offsetptr_fd(currchr, 7, char);
                char_expect(*(currchr), ' ');
                currchr++; // " "

                parse_syscall_registers(&currchr);
                compiler_append_code("int 0x80\n");
            }
            else if(strncmp(currchr, "test", 4) == 0)
            {
                currchr = offsetptr_fd(currchr, 4, char);
                char_expect(*(currchr), ' ');
                currchr++; // " "
                stack_frame.fail_defined = FALSE;
                stack_frame.number = condition_stack_frame_counter;
                stack_frame.pending_end = FALSE;
                condition_stack_frame_counter++;

                stack_push(&cond_stack, get_cond_frame_stack_data(stack_frame));
                value_data = source_parse_expression(&currchr, AAL_DATA_TYPE_NONE);
                
                funname = malloc(strlen("_fail_condition%lu") + strlen(current_function.name) + 16 + 1);
                sprintf(funname, "%s_fail_condition%lu", current_function.name, (stack_top(cond_stack, condition_stack_frame))->number);
                compiler_append_cond(value_data, funname, FALSE);
                
                aalc_state->asm_file_padding++;
                last_defined = 1;
                
            }
            else if(strcmp(currchr, "fail") == 0)
            {
                compiler_append_code("jmp %s_end_condition%lu\n", current_function.name, (stack_top(cond_stack, condition_stack_frame))->number);
                compiler_append_code("%s_fail_condition%lu:\n", current_function.name, (stack_top(cond_stack, condition_stack_frame))->number);
                aalc_state->asm_file_padding++;
                (stack_top(cond_stack, condition_stack_frame))->fail_defined = TRUE;
                (stack_top(cond_stack, condition_stack_frame))->pending_end = FALSE;
                last_defined = 2;
                
            }
            else if(strcmp(currchr, "do") == 0)
            {
                numstack_push(&loop_stack, loop_counter);
                loop_counter++;
                compiler_append_code("jmp %s_loop_test%lu\n", current_function.name, numstack_peek(&loop_stack));
                compiler_append_code("%s_loop%lu:\n", current_function.name, numstack_peek(&loop_stack));
                last_defined = 3;
                
            }
            else if(strncmp(currchr, "loop", 4) == 0)
            {
                if(is_numstack_empty(loop_stack))
                {
                    aalc_error("Can't loop because no do instruction was provided for the loop");
                    exit(EXIT_FAILURE);   
                }
                currchr = offsetptr_fd(currchr, 4, char);
                char_expect(*(currchr), ' ');
                currchr++; // " "
                compiler_append_code("%s_loop_test%lu:\n", current_function.name, numstack_peek(&loop_stack));
                value_data = source_parse_expression(&currchr, AAL_DATA_TYPE_NONE);

                funname = malloc(strlen("_loop%lu") + strlen(current_function.name) + 16 + 1);
                sprintf(funname, "%s_loop%lu", current_function.name, numstack_peek(&loop_stack));
                compiler_append_cond(value_data, funname, TRUE);

                compiler_append_code("%s_loop_end%lu:\n", current_function.name, numstack_pop(&loop_stack));
            }
            else if(strcmp(currchr, "break") == 0)
            {
                if(is_numstack_empty(loop_stack))
                {
                    aalc_error("Can't break because no do instruction was provided for the loop");
                    exit(EXIT_FAILURE);   
                }
                compiler_append_code("jmp %s_loop_end%lu\n", current_function.name, numstack_peek(&loop_stack));
            }
            else if(strcmp(currchr, "end") == 0)
            {
                end_condition(&cond_stack);
            }
            else if(strncmp(currchr, "return", 6) == 0)
            {
                currchr += 6;
                if(*currchr != '\0')
                {
                    char_expect(*(currchr), ' ');
                    currchr++; // " "
                    value_data = source_parse_expression(&currchr, AAL_DATA_TYPE_DWORD);
                    if(value_data.type != COMPILER_VALUE_TYPE_REGISTER)
                    {
                        aalc_error("Return has to be an expression");
                        exit(EXIT_FAILURE);
                    }
                }
                
                compiler_append_code("; leave function\n");
                compiler_append_code("leave\n");
                compiler_append_code("ret\n\n");
                
                aalc_state->asm_file_padding = 0;
                printf("return\n");
            }
            else if(strcmp(currchr, "[") == 0)
            {

            }
            else if(strncmp(currchr, "]", 1) == 0)
            {
                
            }
            else if(strncmp(currchr, "{", 1) == 0)
            {
                if(last_defined == 0)
                {
                    aalc_error("Cannot open curly bracket here");
                    exit(EXIT_FAILURE);
                }
                numstack_push(&currly_brackets_stack, (size_t)last_defined);
            }
            else if(strcmp(currchr, "}") == 0)
            {
                if(is_numstack_empty(&currly_brackets_stack))
                {
                    aalc_error("Cannot close curly bracket here because it wasn't opened");
                    exit(EXIT_FAILURE);
                }
                last_defined = numstack_pop(&currly_brackets_stack);
                
                /* end condition */
                if(last_defined == 1)
                {
                    (stack_top(cond_stack, condition_stack_frame))->pending_end = TRUE;
                }
                /* end fail */
                else if(last_defined == 2)
                {
                    end_condition(&cond_stack);
                }
                /* loop */
                else if(last_defined == 3)
                {

                }
                /* function */
                else
                {
                    compiler_end_stackframe();
                    compiler_append_code("ret\n\n");
                    aalc_state->asm_file_padding = 0;
                }

                last_defined = 0;
            }
            else
            {
                access_array_data.type = COMPILER_ACCESS_ARRAY_TYPE_NONE;
                dereference_type = switch_dereference_type(&currchr);

                varname = read_symbol_name(currchr);                                                          
                len = strlen(varname);

                /* try to get data type*/
                data_type = str2type(varname);

                /* try to get variable name */
                var_ref = aal_variable_get_reference(varname);

                /* try to get function index */
                index = function_name_to_index(varname);

                var_ref.dereference_type = dereference_type;

                effective_type = switch_effective_type(dereference_type, var_ref);

                currchr = offsetptr_fd(currchr, len, char);
            
                
                if(data_type == AAL_DATA_TYPE_ERROR)
                {
                    
                    if(var_ref.type == AAL_VAR_REF_TYPE_ERROR)
                    {
                        /* error */
                        if(index == -1)
                        {
                            aalc_error("'%s' : Data type doesn't exist or variable or function doesn't exist", varname);
                            exit(EXIT_FAILURE);
                        }

                        /* call function */
                        parse_function_call(index, &currchr);
                        continue;
                    }

                    /* set variable */
                    parse_variable_set(&currchr, var_ref, varname, effective_type);
                    
                    continue;
                }
                if(aalc_state->stack_frame.submitted_stack_frame)
                {
                    aalc_error("Cannot define variable not at very start of function");
                    exit(EXIT_FAILURE);
                }

                aalc_state->stack_frame.defining_stack_frame = TRUE;
                /* declare variable */
                parse_variable_declaring(data_type, currinst, &currchr);
            }
        }
        

        
    }

    
    if(!is_stack_empty(cond_stack))
    {
        aalc_error("A conditionnal statement wasn't closed");
        exit(EXIT_FAILURE);
    }
    if(!is_stack_empty(loop_stack))
    {
        aalc_error("A loop statement wasn't closed");
        exit(EXIT_FAILURE);
    }
    if(!is_stack_empty(currly_brackets_stack))
    {
        aalc_error("A curly bracket wasn't closed");
        exit(EXIT_FAILURE);
    }

    if(main_file)
    {
        index = function_name_to_index("entry");
        if(index == -1)
        {
            aalc_error("Function entry was not declared");
            exit(EXIT_FAILURE);
        }
        else if(aalc_state->functions[index].defined == 0)
        {
            aalc_error("Function entry was not defined");
            exit(EXIT_FAILURE);
        }


        
        aalc_state->asm_file_padding = 0;
        compiler_append_code("_start:\n");
        aalc_state->asm_file_padding = 1;
        compiler_append_code("call entry\n");
        compiler_syscall_exit(0);
        aalc_state->asm_file_padding = 0;
        compute_data_segment();
        free(aalc_state);
    }
    
    
    free_stack(&cond_stack);
    free_numstack(&loop_stack);
    return 1;
}
