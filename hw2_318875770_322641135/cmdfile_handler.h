#pragma once

void parse_cmd(char *cmd_str, Command *cmd);
void parse_worker_line(const char *line, Command *job_cmds);

