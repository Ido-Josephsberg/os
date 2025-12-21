#pragma once

#include "job_queue.h"

void parse_cmd(char *cmd_str, Command *cmd);
void parse_worker_line(char *line, Command *job_cmds);

