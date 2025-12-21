#pragma once
#include <pthread.h>
#include "job_queue.h"

static void* thread_routine(void *arg);
void create_num_threads_threads(int num_threads, pthread_t *threads_array);
void exit_all_threads(int num_threads, pthread_t *threads_array, Command *exit_cmd_array);
static void execute_job(Command *job_cmd);
static void execute_basic_command(Command *basic_cmd); 
void msleep(int milisec);