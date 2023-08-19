#ifndef __COMPILER_SYSCALLS_H__
#define __COMPILER_SYSCALLS_H__

#include <stdlib.h>

void compiler_syscall(char* syscall_number, char* ebx, char* ecx, char* edx);
void compiler_syscall_write(size_t fd, char* data, size_t size);
void compiler_syscall_exit(size_t code);

#endif