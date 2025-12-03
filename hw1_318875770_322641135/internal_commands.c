#include "internal_commands.h"
#include "background_processes.h"
#include "parse_command.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

void cd(ParsedCommand* parsed_command) {
    /* 
    Change dir command
    */
    // Check if got 2 arguments (one command and one argument) or is a background command
    if (parsed_command->arg_count != 2 || parsed_command->is_background) {
        printf("hw1shell: invalid command\n");
    }
    // Change dir by system call. Notify in case of an error
    else if (chdir(parsed_command->args[1]) != 0) {
        printf("hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

void jobs(AllBackgroundProcesses* all_background_processes, ParsedCommand* parsed_command) { 
    /*
    Prints the current jobs (pid tab original command)
    */
    // Check if got 1 argument (only command) or is a background command
    if (parsed_command->arg_count != 1 || parsed_command->is_background)
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