#pragma once

void process_cmdfile_line(const char *line);

struct Command {
    char* cmd_name;
    int cmd_arg;
};