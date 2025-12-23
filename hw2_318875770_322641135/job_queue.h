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
<<<<<<< HEAD
    char* job_line[MAX_JOB_FILE_LINE]; // Original job line for logging purposes
=======
    char job_line[MAX_JOB_FILE_LINE]; // Original job line for logging purposes
    long long time_after_reading_line_ms; // Time after reading the line for logging purposes
>>>>>>> hw2_ido_new
} Job;

typedef struct JobQueue {
    Job* head; // Pointer to the head of the job queue
    Job* tail; // Pointer to the tail of the job queue
    int size; // Current size of the job queue
    int total_jobs_added; // Total number of jobs added to the queue
    int num_of_working_threads; // Number of threads currently working on jobs
    int log_enabled; // Flag indicating if logging is enabled
    int exit_flag; // Flag to indicate if threads should exit
    long long total_turnaround_time_ms; // Total turnaround time of all jobs
    long long min_turnaround_time_ms; // Minimum turnaround time of all jobs
    long long max_turnaround_time_ms; // Maximum turnaround time of all jobs
    pthread_mutex_t lock; // Mutex for synchronizing access to the job queue
    pthread_cond_t cond_idle; // Condition variable to signal when the queue is idle
} JobQueue;

void push_job(Command *job_cmd, char* line, long long time_after_reading_line_ms);
Job* pop_job();
