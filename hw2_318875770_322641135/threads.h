#pragma once
#include <pthread.h>
#include "job_queue.h"

void create_num_threads_threads(int num_threads, pthread_t *threads_array);
void exit_all_threads(int num_threads, pthread_t *threads_array, Command *exit_cmd_array);
<<<<<<< HEAD
static void execute_job(Command *job_cmd);
static void execute_basic_command(Command *basic_cmd); 
=======
>>>>>>> dev_hw2
void msleep(int milisec);