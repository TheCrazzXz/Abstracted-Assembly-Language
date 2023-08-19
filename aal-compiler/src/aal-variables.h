#ifndef __AAL_VARIABLES_H__
#define __AAL_VARIABLES_H__

#include "aal-data.h"
#include "compiler.h"
#include "compiler-operations.h"

aal_data_type dereference_variable(char** expression, char** str, size_t n, aal_operation op, Bool* set_in_place, aal_data_type dst_type);

void parse_variable_set(char** curr, aal_var_ref var_ref, char* varname, aal_data_type effective_type);
void parse_variable_declaring(aal_data_type data_type, char* current_instruction, char** curr);

void compiler_set_u32_variable(aal_var_ref var_ref, compiler_array_access_data array_access_data, compiler_value_data value_data);
void compiler_set_u8_variable(aal_var_ref var_ref, compiler_array_access_data array_access_data, compiler_value_data value_data);
#endif