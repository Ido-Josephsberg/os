#pragma once

void process_cmdfile_line(const char *line);
void insert_job_into_queue(/*TODO: complete args*/);

struct Command {
    char* cmd_name;
    int cmd_arg;
};