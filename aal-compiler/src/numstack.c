#include "numstack.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

numstack create_numstack()
{
    numstack s;

    memset(&s, 0, sizeof(s));
    s.top = -1;

    return s;
}

void numstack_push(numstack* s, size_t data)
{
    ALLOC_NEW_ELEMENT_FOR_VECTOR(s->mem, s->size, sizeof(size_t));
    s->mem[++s->top] = data;
}

size_t numstack_peek(numstack* s)
{
    return s->mem[s->top];
}

size_t numstack_pop(numstack* s)
{
    size_t data = numstack_peek(s);
    s->top--;
    return data;
}

void free_numstack(numstack* s)
{
    free(s->mem);
}