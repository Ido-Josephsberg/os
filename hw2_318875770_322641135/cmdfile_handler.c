// TODO: Implement commands from command files "cmd_file_executor".
    // Should read commands from each line of the command file, dispatcher  commands should be executed 
    // sequentially.
    // Commands starting with worker should be assigned to shared work queue (to be executed by worker threads
    // when available).

//TODO: Hangle log files? 

#include "dispatcher.h"
#include "macros.h"
#include "cmdfile_handler.h"
#include "job_queue.h"
#include <stdlib.h>
#include <stdio.h>

void parse_cmd(char *cmd_str, Command *cmd) {
    if (cmd_str == NULL || cmd == NULL) {
        fprintf(stderr, "Invalid command string or command struct: NULL\n");
        exit(EXIT_FAILURE);
    }

    char *saveptr;

    char *name = strtok_r(cmd_str, " \t\n", &saveptr);
    char *arg_str = strtok_r(NULL, " \t\n", &saveptr);

    if (name == NULL) {
        fprintf(stderr, "Invalid command format: no command name\n");
        exit(EXIT_FAILURE);
    }

    // Copy command name into Command struct
    strncpy(cmd->cmd_name, name, sizeof(cmd->cmd_name) - 1);
    cmd->cmd_name[sizeof(cmd->cmd_name) - 1] = '\0'; // Ensure null-termination
    
    // If argument exists, parse it; otherwise set to 0
    if (arg_str != NULL) {
        cmd->cmd_arg = atoi(arg_str);
    } else {
        cmd->cmd_arg = 0; // Default value for commands without arguments
    }
}

void parse_worker_line(const char *line, Command *job_cmds) {   

    if (line == NULL || job_cmds == NULL) {
        fprintf(stderr, "Invalid command line: NULL\n");
        // exit() - all open files are automatically closed by the C runtime / OS.
        exit(EXIT_FAILURE);
    }
    if (line[0] == '\n' || line[0] == '\0') {
        // Empty line, nothing to process
        return;
    }
    
    //Strip line endings and whitespaces if needed
    char *saveptr1; // Used for thread-safe/nested parsing
    int i = 0; // Index for job_cmds array

    // Split by semicolon to get each individual command
    char *command_str = strtok_r(line, ";\r\n", &saveptr1);
    while (command_str != NULL && i < MAX_COMMANDS_IN_JOB) {

        char *command_str_cp = command_str;
        parse_cmd(command_str_cp, job_cmds + i);
        
        // Move to next job command
        i++;
        
        // Move to the next command string after the next semicolon
        command_str = strtok_r(NULL, ";\r\n", &saveptr1);
    }

    //TODO: Hangle LOG FILES? 
}
    

void insert_job_into_queue(/*TODO: complete args*/) {
    //TODO: Implement job insertion logic
}
