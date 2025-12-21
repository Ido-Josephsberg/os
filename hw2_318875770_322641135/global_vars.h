#pragma once

#include "dispatcher.h"
#include "job_queue.h"
#include "macros.h"

//extern Command;
extern int curr_num_of_jobs;
extern JobQueue shared_jobs_queue;
extern pthread_mutex_t file_counters_mutexes[MAX_COUNTER_FILES];
extern pthread_mutex_t mutex_of_shared_jobs_queue;
extern pthread_cond_t ava_jobs_cond;