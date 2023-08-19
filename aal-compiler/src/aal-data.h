#ifndef __AAL_DATA_H__
#define __AAL_DATA_H__

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#define get_stack_var(index) (&(aalc_state->stack_frame.stack_variables[index]))
// #define get_variable_offset_from_esp(index) ( aalc_state->stack_frame.stack_variables[index].offset + aalc_state->stack_frame.stack_variables[index].data_size)

typedef enum __expression_data_type_t
{
    EXPR_DATA_TYPE_NONE,
    EXPR_DATA_TYPE_STRING_LITERRAL,
    EXPR_DATA_TYPE_STRING_CHARACTER,
    EXPR_DATA_TYPE_DATA_TYPE,
    EXPR_DATA_TYPE_DATA_VAR_NAME,

    EXPR_DATA_TYPE_EQUAL_COMPARAISON,
    EXPR_DATA_TYPE_DIFFERENT_COMPARAISON,
    EXPR_DATA_TYPE_LESS_COMPARAISON,
    EXPR_DATA_TYPE_ABOVE_COMPARAISON,
    EXPR_DATA_TYPE_LESS_EQUAL_COMPARAISON,
    EXPR_DATA_TYPE_ABOVE_EQUAL_COMPARAISON
} expr_data_type;

typedef enum __aal_data_type_e
{
    AAL_DATA_TYPE_NONE,
    AAL_DATA_TYPE_DWORD,
    AAL_DATA_TYPE_BYTE,
    AAL_DATA_TYPE_ERROR
} aal_data_type;

typedef struct __aal_stack_variable_t
{
    size_t data_size;
    char* name;
    aal_data_type type;
    size_t offset;
} aal_stack_variable;

typedef struct __aal_stack_frame_t
{
    aal_stack_variable* stack_variables;
    size_t stack_variables_len;
    size_t stack_frame_size;
    Bool submitted_stack_frame;
    Bool defining_stack_frame;
} aal_stack_frame;

typedef enum __aal_variable_reference_type_e
{
    AAL_VAR_REF_TYPE_ERROR,
    AAL_VAR_REF_TYPE_STACK_FRAME,
    AAL_VAR_REF_TYPE_STACK_PARAMETER,
    AAL_VAR_REF_TYPE_DATA_LABEL,
} aal_var_ref_type;

typedef struct __aal_variable_reference_t
{
    aal_var_ref_type type;
    aal_data_type data_type;
    aal_data_type dereference_type;
    size_t index;
} aal_var_ref;

typedef struct __expression_data_t
{
    void* data;
    char* name;
    size_t data_size;
    expr_data_type type;
    aal_data_type data_type;

} expr_data;
size_t type2size(aal_data_type type);
aal_data_type str2type(char* type);

char* data_seg_idx_to_name(size_t index);
void compute_data_segment();
aal_var_ref aal_variable_get_reference(char* name);
size_t find_and_use_usable_register_number();
size_t find_usable_register_number();
#define drop_register_usage(n) aalc_state->register_usage[n] = FALSE

#endif