#pragma once
#include <pthread.h>
#include "job_queue.h"

void create_num_threads_threads(int num_threads, pthread_t *threads_array);
void exit_all_threads(int num_threads, pthread_t *threads_array, Command *exit_cmd_array);
void msleep(int milisec);