#ifndef __COMPILER_OPERATIONS_H__
#define __COMPILER_OPERATIONS_H__

#include <stdlib.h>
#include "aal-data.h"
#include "memory.h"
#include "stack.h"
#define compiler_operate_register(operation, register_name, value, type) compiler_append_code("%s %s %s, %s\n", operation, type, register_name, value)
#define is_digit(c) (c >= '0' && c <= '9')
#define is_hex_digit(c) ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
#define is_caps_char(c) (c >= 'A' && c <= 'Z')
#define is_uncaps_char(c) (c >= 'a' && c <= 'z')
#define is_symbol_char(c) (is_caps_char(c) || is_uncaps_char(c) || c == '_')
typedef enum aal_operation_e
{
    /* move */
    AAL_OPERATION_NOT_SPECIFIC,

    /* arithmetic operations */
    AAL_OPERATION_ADDITION,
    AAL_OPERATION_SUBTRACTION,
    AAL_OPERATION_MULTIPLICATION,
    AAL_OPERATION_DIVISION,
    AAL_OPERATION_MODULO,

    /* bitwise operations */
    AAL_OPERATION_BITWISE_AND,
    AAL_OPERATION_BITWISE_OR,
    AAL_OPERATION_BITWISE_XOR
} aal_operation;

char* reg_number_to_reg(size_t n, Bool byte_register);

char* reg_number_to_dword_reg(size_t n);
char* reg_number_to_byte_reg(size_t n, char low_or_high);

aal_stack_variable* get_stack_var_from_var_ref(aal_var_ref var_ref);

char* get_array_index_code(aal_var_ref var_ref, size_t index);
char* get_array_index_from_register_code(aal_var_ref var_ref, size_t register_index);

char* get_variable_code(aal_var_ref var_ref);

aal_operation character_to_operation(char* data);

void register_operation(aal_operation operation, char* register_name, char* var_asm, char* type);
void add_square_brackets(char** str);
struct __compiler_value_data_t;
struct __compiler_array_access_data_t;
void compiler_append_cond(struct __compiler_value_data_t value_data, char* dest, Bool activated);

char* switch_compiler_value(struct __compiler_value_data_t value_data, Bool force_byte);
char* switch_compiler_access_array(aal_var_ref var_ref, struct __compiler_array_access_data_t array_access_data);
aal_data_type switch_effective_type(aal_data_type dereference_type, aal_var_ref var_ref);

void end_condition(stack* cond_stack);

#endif