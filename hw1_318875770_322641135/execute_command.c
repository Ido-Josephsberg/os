#include "execute_command.h"
#include "background_processes.h"
#include "parse_command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
void execute_external_command(ParsedCommand* parsed_command, AllBackgroundProcesses* all_background_processes) {
    /*
    Execute the command represented by the ParsedCommand struct.
    */
    // Check if the command is background
    if (parsed_command->is_background) {
        // Check if we can add a new background process
        if (all_background_processes->count >= MAX_BACKGROUND_PROCESSES) {
            printf("hw1shell: too many background commands running\n");
            return;
        }
    }
    // Fork a new process to execute the command in the background or foreground
    int pid = fork();
    // Check for fork failure
    if (pid < 0) {
        // Notify about system call failure
        printf("hw1shell: %s failed, errno is %d\n", "fork", errno);
        return;
    } else if (pid == 0) {
        // Child process
        // Execute the command
        execvp(parsed_command->args[0], parsed_command->args);
        // If execvp returns, there was an error
        // TODO: DO 13 and exit
        printf("hw1shell: invalid command\n");
        // Notify about system call failure
        printf("hw1shell: %s failed, errno is %d\n", "execvp", errno);
        exit(1);
    } else {
        // Parent process
        // If the command is foreground, wait for it to finish
        if (!(parsed_command->is_background)) {
            // Wait for the child process to finish and check for errors
            if (waitpid(pid, NULL, 0) == -1) {
                // TODO: notify on error
                return;
        }
        // If the command is background, add it to the list of background processes
        }
        else {
            // Check for empty slot
            int i = 0, found_slot = 0;
            while (i < MAX_BACKGROUND_PROCESSES && !found_slot) {
                if ((&(all_background_processes->processes[i]))->pid == -1) {
                    found_slot = 1;
                }
                else
                    i++;
            }
            // Notify the user that the process is running in the background
            printf("hw1shell: pid %d started\n", pid);
            // Add the new background process to the list
            BackgroundProcess* empty_slot = &(all_background_processes->processes[i]);
            empty_slot->pid = pid;
            all_background_processes->count++;
            strncpy(empty_slot->command, parsed_command->command, MAX_LINE_LENGTH);

        }
    }
}