#pragma once
#include "job_queue.h"
#include "macros.h"

//extern Command;
extern JobQueue shared_jobs_queue;
extern pthread_mutex_t file_counters_mutexes[MAX_COUNTER_FILES];
extern pthread_cond_t ava_jobs_cond;