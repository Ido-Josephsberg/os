#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job_queue.h"
#include "global_vars.h"

void push_job(Command *job_cmd, char* line) {
    // Create a new job and add it to the job queue
    // Allocate memory for the new job
    Job* new_job = (Job*)malloc(sizeof(Job));
    // Check for memory allocation failure and notify
    if (new_job == NULL) {
        printf("hw2: memory allocation failed, exiting\n");
        exit(EXIT_FAILURE);
    }
    // Initialize the new job struct
    new_job->job_cmds = job_cmd;  // take ownership of caller's malloc
    new_job->next = NULL;
    strncpy((char*)new_job->job_line, line, MAX_JOB_FILE_LINE - 1); //TODO: doesnt it need to be wothout -1
    new_job->job_line[MAX_JOB_FILE_LINE - 1] = '\0'; // Ensure null-termination

    // If the queue is empty, set head and tail to the new job and wake up a thread
    if (shared_jobs_queue.size == 0) {
        shared_jobs_queue.head = new_job;
        shared_jobs_queue.tail = new_job;
    } else {
        // Otherwise, add the new job to the end of the queue
        shared_jobs_queue.tail->next = new_job;
        shared_jobs_queue.tail = new_job;
    }
    shared_jobs_queue.size++;
    // Signal waiting threads that a new job is available.
    pthread_cond_signal(&ava_jobs_cond);
}

Command* pop_job() {
    // Remove and return the job commands from the head of the job queue. Call with the job queue mutex locked.
    // If the queue is empty, return NULL
    if (shared_jobs_queue.size == 0) 
        return NULL;
    // Get the job at the head of the queue
    Job* head_job = shared_jobs_queue.head;
    Command* job_cmds = head_job->job_cmds;

    // Update the head of the queue to the next job
    shared_jobs_queue.head = head_job->next;
    // If the queue is now empty, update the tail to NULL
    if (shared_jobs_queue.head == NULL) {
        shared_jobs_queue.tail = NULL;
    }
    shared_jobs_queue.size--;

    // Free the memory allocated for the removed job struct
    free(head_job);

    // Return pointer to the Commands array of the removed job
    //TODO NOTE: TO FREE THE MEMORY
    return job_cmds;
}