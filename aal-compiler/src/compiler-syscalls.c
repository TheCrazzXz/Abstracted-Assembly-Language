#include "compiler-syscalls.h"
#include "compiler.h"
#include "compiler-operations.h"

void compiler_syscall(char* syscall_number, char* ebx, char* ecx, char* edx)
{
    if(syscall_number != NULL)
        compiler_set_register_dword("eax", syscall_number);
    if(ebx != NULL)
        compiler_set_register_dword("ebx", ebx);
    if(ecx != NULL)
        compiler_set_register_dword("ecx", ecx);
    if(edx != NULL)
        compiler_set_register_dword("edx", edx);
    
    compiler_append_code("int 0x80\n\n");
}

void compiler_syscall_write(size_t fd, char* data, size_t size)
{
    char* syscall_number = "4";
    char* fd_str = malloc(16);
    char* size_str = malloc(16);

    sprintf(fd_str, "%lu", fd);
    sprintf(size_str, "%lu", size);

    compiler_syscall(syscall_number, fd_str, data, size_str);

    free(fd_str);
    free(size_str);
}
void compiler_syscall_exit(size_t code)
{
    char* syscall_number = "1";
    char* code_str = malloc(16);

    sprintf(code_str, "%lu", code);
    compiler_syscall(syscall_number, code_str, NULL, NULL);

    free(code_str);
}