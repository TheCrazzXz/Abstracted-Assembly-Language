#include "stack.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

stack_data get_as_stack_data(void* data, size_t size)
{
    stack_data ret;

    ret.size = size;
    ret.data = malloc(ret.size);
    memcpy(ret.data, data, ret.size);

    return ret;
}

stack create_stack()
{
    stack s;

    memset(&s, 0, sizeof(s));
    s.top = -1;

    return s;
}

void stack_push(stack* s, stack_data data)
{
    ALLOC_NEW_ELEMENT_FOR_VECTOR(s->mem, s->size, sizeof(stack_data));
    s->mem[++s->top] = data;
}

stack_data* stack_peek(stack* s)
{
    return &s->mem[s->top];
}

void stack_pop(stack* s)
{
    free(s->mem[s->top--].data);
}

void free_stack(stack* s)
{
    free(s->mem);
}