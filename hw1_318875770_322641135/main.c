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
    AllBackgroundProcesses all_background_processes;
    all_background_processes.count = 0;
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        all_background_processes.processes[i] = (BackgroundProcess*)malloc(sizeof(BackgroundProcess));
        all_background_processes.processes[i]->pid = -1;
    }
    // The variable to hold the command line input
    char command_line[MAX_LINE_LENGTH];
    // Infinite loop of the shell
    while (TRUE) {
        // Get user input
        printf("hw1shell$ ");
        fgets(command_line, MAX_LINE_LENGTH, stdin);
        // Replace newline character from fgets with null terminator
        command_line[strcspn(command_line, "\n")] = 0;
        // continue to next iteration if the command line is empty
        if (strlen(command_line) == 0) {
            reap_zombie_processes(&all_background_processes, FALSE);
            continue;
        }
        // Parse the command line input into a ParsedCommand struct
        ParsedCommand* parsed_command = parse_command(command_line);
        if (!parsed_command) {
            continue; // Skip to next iteration if parsing failed
        }
        if (strcmp(parsed_command->args[0], "cd") == 0) {
            // Handle internal command 'cd'
            cd(parsed_command->args[1]);
        } else if (strcmp(parsed_command->args[0], "jobs") == 0) {
            // Handle internal command 'jobs'
            jobs(&all_background_processes);
        }
        // If the command is "exit", break the loop and exit after reap subprocesses and free memory
        else if (strcmp(command_line, "exit") == 0) {
            // TODO: Wait for all subprocesses to finish and free allocated memory
            // Reap any remaining zombie processes, wait for them to finish
            reap_zombie_processes(&all_background_processes, TRUE);
            // Free the parsed command memory
            free_parsed_command(parsed_command);
            // Free background processes memory
            free_background_processes(&all_background_processes);
            break;
        } else {
            // Handle external commands
            execute_external_command(parsed_command, &all_background_processes);
        }


        // TODO: reap zombies and notify (12)
        // Reap zombie processes
        reap_zombie_processes(&all_background_processes, FALSE);


    }

    return 0;
}