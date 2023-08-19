#ifndef __numstack_H__
#define __numstack_H__

#include <stdlib.h>

#include "memory.h"

#define numstack_elem(numstack, index, type) (type*)(((numstack).mem)[index].data)
#define numstack_top(numstack, type) numstack_elem(numstack, (numstack).top, type)
#define is_numstack_empty(numstack) (numstack.top < 0)

typedef struct _numstack_t
{
    size_t* mem;
    size_t size;
    ssize_t top;
} numstack;

numstack create_numstack();

void numstack_push(numstack* ps, size_t data);
size_t numstack_peek(numstack* ps);
size_t numstack_pop(numstack* ps);
void free_numstack(numstack* ps);

#endif