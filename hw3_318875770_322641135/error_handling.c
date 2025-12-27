#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void sys_call_error(const char *syscall_name) {
    // Print the error message for the given system call name along with the current errno value
    printf("hw3: %s failed, errno is %d\n", syscall_name, errno);
    exit(EXIT_FAILURE);
}

void error(const char *error_message) {
    // Print a general error message
    printf("hw3: %s\n", error_message);
    exit(EXIT_FAILURE);
}