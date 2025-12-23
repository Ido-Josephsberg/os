#pragma once

#include "macros.h"
#include <pthread.h>

// Command struct representing a single command
typedef struct {
    char cmd_name[MAX_CMD_NAME_LENGTH];
    int cmd_arg;
} Command;

// Job struct representing a job in the job queue
typedef struct Job {
    Command* job_cmds; // pointer to array of Command pointers in the job
    struct Job* next; // Pointer to the next job in the queue
    char job_line[MAX_JOB_FILE_LINE]; // Original job line for logging purposes
} Job;

typedef struct JobQueue {
    Job* head; // Pointer to the head of the job queue
    Job* tail; // Pointer to the tail of the job queue
    int size; // Current size of the job queue
    int num_of_working_threads; // Number of threads currently working on jobs
    int log_enabled; // Flag indicating if logging is enabled
    pthread_mutex_t lock; // Mutex for synchronizing access to the job queue
    pthread_cond_t cond_idle; // Condition variable to signal when the queue is idle
} JobQueue;

void push_job(Command *job_cmd, char* line);
Job* pop_job();