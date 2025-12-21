#include "job_queue.h"
#include <stdlib.h>
#include <stdio.h>


void push_job(Command *job_cmd, JobQueue *queue) {
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
    
    // If the queue is empty, set head and tail to the new job
    if (queue->size == 0) {
        queue->head = new_job;
        queue->tail = new_job;
    } else {
        // Otherwise, add the new job to the end of the queue
        queue->tail->next = new_job;
        queue->tail = new_job;
    }
    queue->size++;
}

Command* pop_job(JobQueue *queue) {
    // Remove and return the job commands from the head of the job queue
    // If the queue is empty, return NULL
    if (queue->size == 0) {
        return NULL;
    }
    // Get the job at the head of the queue
    Job* head_job = queue->head;
    Command* job_cmds = head_job->job_cmds;

    // Update the head of the queue to the next job
    queue->head = head_job->next;
    // If the queue is now empty, update the tail to NULL
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    queue->size--;

    // Free the memory allocated for the removed job struct
    free(head_job);

    // Return pointer to the Commands array of the removed job
    //TODO NOTE: TO FREE THE MEMORY
    return job_cmds;
}