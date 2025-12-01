#pragma once
#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 65 // Maximum number of arguments (command and 64 parameters)
typedef struct
{
    char command[MAX_LINE_LENGTH];
    char* args[MAX_ARGS];
    int arg_count;
    int is_background;
} ParsedCommand;

ParsedCommand* parse_command(char* command_line);
void free_parsed_command(ParsedCommand* parsed_command);

