#pragma once
#include "background_processes.h"
#include "parse_command.h"
void cd(ParsedCommand* parsed_command);
void jobs(AllBackgroundProcesses* all_background_processes, ParsedCommand* parsed_command);