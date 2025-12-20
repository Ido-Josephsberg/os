#pragma once

void parse_cmd(char *cmd_str, Command *cmd);
void parse_worker_line(const char *line, Command *job_cmds);

typedef struct {
    char cmd_name[MAX_CMD_NAME_LENGTH]; //TODO: change to array of chars cmd_name[MAX_CMD_NAME_LENGTH];
    int cmd_arg;
} Command;