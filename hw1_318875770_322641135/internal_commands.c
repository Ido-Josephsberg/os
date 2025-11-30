#include "internal_commands.h"
#include <unistd.h>

int cd(char* path) {
    if (chdir(path) != 0) {
        printf("hw1shell: invalid command\n");
        return -1;
    }
    return 0;
}

int jobs() { 
    return 0;
}