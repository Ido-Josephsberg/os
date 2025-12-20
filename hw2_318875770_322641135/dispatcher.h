#pragma once

#include <stdio.h>
#include <stdlib.h>

// Dispatcher functions
void init_dispatcher(int num_counters, int num_threads, int log_enabled);
void dispatcher_msleep(int milliseconds);
void dispatcher_wait(JobQueue* job_queue);
void run_dispatcher(FILE *cmd_file, int num_counters, int num_threads, int log_enabled);
void finalize_dispatcher();
void dispatcher(int argc, char *argv[]);