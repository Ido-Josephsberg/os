#pragma once

void create_threadxx_files(int num_files);
void write_into_log_file(char* line, int thread_number,int is_start, int is_dispatcher, long long ms_time);