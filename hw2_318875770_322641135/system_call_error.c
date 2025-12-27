#include <stdio.h>
#include <errno.h>
void print_sys_call_error(const char *syscall_name) {
    // Print the error message for the given system call name along with the current errno value
    printf("hw2: %s failed, errno is %d\n", syscall_name, errno);
}