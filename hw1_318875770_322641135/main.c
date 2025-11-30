#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 1024
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_command.h"
#include "background_processes.h"
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
        // Parse the command line input into a ParsedCommand struct
        ParsedCommand* parsed_command = parse_command(command_line);
        if (!parsed_command) {
            continue; // Skip to next iteration if parsing failed
        }



        // TODO: reap zombies and notify (12)

        // If the command is "exit", break the loop and exit after reap subprocesses and free memory
        if (strcmp(command_line, "exit") == 0) {
            // TODO: Wait for all subprocesses to finish and free allocated memory
            // Free the parsed command memory
            free_parsed_command(parsed_command);
            break;
        }
    }

    return 0;
}