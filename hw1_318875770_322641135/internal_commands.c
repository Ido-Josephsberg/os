#include "internal_commands.h"
#include "background_processes.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

void cd(char* path) {
    if (chdir(path) != 0) {
        printf("hw1shell: invalid command\n");
        printf("hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

void jobs(AllBackgroundProcesses* all_background_processes) { 
    BackgroundProcess* process;
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        process = all_background_processes->processes[i];
        if (process->pid != -1) {
            printf("%d\t%s\n", process->pid, process->command);
        }
    }
}