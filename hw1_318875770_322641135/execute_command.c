#include "execute_command.h"
#include "background_processes.h"
#include "parse_command.h"
void execute_command(ParsedCommand* parsed_command, AllBackgroundProcesses* all_background_processes) {
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
        // Fork a new process to execute the command in the background
        int pid = fork();
        if (pid < 0) {
            // TODO: notify system call failure (13)
            return;
        } else if (pid == 0) {
            // Child process
            // Execute the command
            execvp(parsed_command->args[0], parsed_command->args);
            // If execvp returns, there was an error
            printf("hw1shell: invalid command\n");
            return;
        } else {
            // Parent process
            // Add the new background process to the list - check for empty slot
            int i = 0, found_slot = 0;
            while (i < MAX_BACKGROUND_PROCESSES && !found_slot) {
                if ((all_background_processes->processes[i])->pid == -1) {
                    (all_background_processes->processes[i])->pid = pid;
                    all_background_processes->count++;
                    strncpy((all_background_processes->processes[i])->command, parsed_command->command, MAX_LINE_LENGTH);
                    found_slot = 1;
                }
                i++;
            }

            // Notify the user that the process is running in the background
            printf("hw1shell: pid %d started\n", pid);
    }
}
