#pragma once
#include "background_processes.h"
#include "parse_command.h"

void execute_external_command(ParsedCommand* parsed_command, AllBackgroundProcesses* all_background_processes);