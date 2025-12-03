#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 1024
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_command.h"
#include "background_processes.h"
#include "execute_command.h"
#include "internal_commands.h"
//#include "allocate_handle.h"


    

int main(int argc, char const *argv[])
{
    // Initialize all background processes struct
    AllBackgroundProcesses all_background_processes;
    // Initialize parsed command struct
    ParsedCommand parsed_command;
    // Set all background process count to 0 and pids of their processes to -1
    all_background_processes.count = 0;
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) 
        (all_background_processes.processes[i]).pid = -1;
    
    // The variable to hold the command line input
    char command_line[MAX_LINE_LENGTH];
    // Infinite loop of the shell
    while (TRUE) {
        // Get user input
        printf("hw1shell$ ");
        if (fgets(command_line, MAX_LINE_LENGTH, stdin) == NULL) {
            printf("Got EOF or read error has occured.\n");
            exit(1);
        }

        // Replace newline character from fgets with null terminator
        command_line[strcspn(command_line, "\n")] = 0;

        // continue to next iteration if the command line is empty
        if (strlen(command_line) == 0) {
            reap_zombie_processes(&all_background_processes, FALSE);
            continue;
        }

        // Parse the command line input into a ParsedCommand struct
        parse_command(&parsed_command, command_line);

        // Check if the command is an internal command
        if (strcmp(parsed_command.args[0], "cd") == 0) {
            // Handle internal command 'cd'
            cd(&parsed_command);
        } else if (strcmp(parsed_command.args[0], "jobs") == 0) {
            // Handle internal command 'jobs'
            jobs(&all_background_processes, &parsed_command);
        }
        // If the command is "exit", break the loop and exit after reap subprocesses and free memory
        else if (strcmp(parsed_command.args[0], "exit") == 0) {
            // Check if got one argument (only command)
            if (parsed_command.arg_count != 1 || parsed_command.is_background)
                printf("hw1shell: invalid command\n");
            else {
                // Reap any remaining zombie processes, wait for them to finish (don't notify the user)
                reap_zombie_processes(&all_background_processes, TRUE);
                // Exit the shell
                break;
            }
            
        } else {
            // Handle external commands
            execute_external_command(&parsed_command, &all_background_processes);
        }
        // Reap zombie processes and notify the user
        reap_zombie_processes(&all_background_processes, FALSE);
    }

    return 0;
}