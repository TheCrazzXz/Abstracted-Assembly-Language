#ifndef __SOURCE_PARSER_H__
#define __SOURCE_PARSER_H__


#include <stdio.h>
#include "aal-data.h"
#include "compiler-operations.h"
#include "memory.h"
#include <stdint.h>


char* read_double_quote(char* data);
struct __compiler_value_data_t;

void char_expect(char c, char expect);
char* source_read_instruction(FILE* source_file);
int source_ignore_whitespaces(FILE* source_file);
Bool compiler_ignore_comments(char* current_instruction, Bool* comment);
expr_data source_read_next_data(char** data);
dword_t read_uint(char* data, size_t* len);
char* read_symbol_name(char* start);

void parse_syscall_registers(char** expression);
aal_stack_variable* read_function_parameters(char** start, size_t* parameters_len);


struct __compiler_value_data_t parse_push_arguments(char** curr, size_t function_index);
aal_data_type switch_dereference_type(char** expression);

#endif