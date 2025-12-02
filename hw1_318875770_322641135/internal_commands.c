#include "internal_commands.h"
#include "background_processes.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

void cd(char* path, int arg_count) {
    /* 
    Change dir command
    */
    // Check if got 2 arguments (one command and one argument)
    if (arg_count != 2) {
        printf("hw1shell: invalid command\n");
    }
    // Change dir by system call. Notify in case of an error
    else if (chdir(path) != 0) {
        printf("hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

void jobs(AllBackgroundProcesses* all_background_processes, int arg_count) { 
    /*
    Prints the current jobs (pid tab original command)
    */
    // Check if got 1 argument (only command)
    if (arg_count != 1)
        printf("hw1shell: invalid command\n");
    // Iterate through not reaped background processes and print their pid and command.
    BackgroundProcess* process;
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        process = &(all_background_processes->processes[i]);
        if (process->pid != -1) {
            printf("%d\t%s\n", process->pid, process->command);
        }
    }
}