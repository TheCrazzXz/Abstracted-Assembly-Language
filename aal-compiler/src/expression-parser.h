#ifndef __EXPRESSION_PARSER_H__
#define __EXPRESSION_PARSER_H__

#include "aal-data.h"
#include "memory.h"
#include <stdlib.h>
struct __compiler_value_data_t;


struct __compiler_value_data_t source_parse_expression_core(char** expression, ssize_t reg_number, Bool, aal_data_type dst_type);
struct __compiler_value_data_t source_parse_expression(char** expression, aal_data_type dst_type);

#endif