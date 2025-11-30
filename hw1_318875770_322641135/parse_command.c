#include "parse_command.h"
#include <stdlib.h>

ParsedCommand* parse_command(char* command_line) {
    /*
    Parse the command_line into a ParsedCommand struct and return a pointer to it.
    */
    // Allocate memory for ParsedCommand struct
    ParsedCommand* parsed_command = (ParsedCommand*)malloc(sizeof(ParsedCommand));
    // Check for allocation failure and handle it
    if (!parsed_command) {
        printf("Failed to allocate memory\nexit 1\n");
        return NULL;
    }
    // Copy the command line into the command field
    strncpy(parsed_command->command, command_line, MAX_LINE_LENGTH);
    // Initialize args to their values based on command_line, arg_count, is_background, and is_internal
    update_parsed_command_args(parsed_command, command_line);
    parsed_command->is_internal = (strcmp(parsed_command->args[0], "cd") == 0 || strcmp(parsed_command->args[0], "jobs") == 0 || strcmp(parsed_command->args[0], "exit") == 0);
    // Return the ParsedCommand pointer
    return parsed_command;
}

void update_parsed_command_args(ParsedCommand* parsed_command, char* command_line) {
    /*
    Updates the args and arg_count fields of the ParsedCommand struct based on the provided command_line. Updates is_background if the last argument is '&'.
    */
    int i = 0;
    do{
        parsed_command->args[i] = strtok(i == 0 ? command_line : NULL, " \t\n");
        i++;
    } while (parsed_command->args[i - 1] != NULL && i < MAX_ARGS);
    parsed_command->arg_count = i;
    if (parsed_command->args[parsed_command->arg_count - 1] == '&') {
        parsed_command->is_background = 1;
        parsed_command->args[parsed_command->arg_count - 1] = NULL;
        parsed_command->arg_count--;
    } else {
        parsed_command->is_background = 0;
    }    
}
void free_parsed_command(ParsedCommand* parsed_command) {
    /*
    Frees the memory allocated for the ParsedCommand struct.
    */
    if (parsed_command) {
        free(parsed_command);
    }
}