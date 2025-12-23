#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "global_vars.h"
#include "system_call_error.h"
#include "counter_files.h"
#include "cmdfile_handler.h"
#include "job_queue.h"
#include "threads.h"
#include "macros.h"
#include "log_files.h"
// ?TODO?: Create thread struct (in h file) which holds thread id, current job info, available (mutex).
// TODO: Implement create_threads function which creates num_threads threads.
// TODO: Implement thread_routine function which each thread will run.

void msleep(int milisec) {
    // Sends the calling thread to sleep to milisec miliseconds
    // Convert miliseconds to microseconds
    int mikrosec = milisec * 1E3;
    // Sleep for mikrosec mikroseconds. Notify in case of failure
    if (usleep(mikrosec) != 0)
        print_sys_call_error("usleep");
}

static void execute_basic_command(Command *basic_cmd) {
    // Execute a basic command (msleep, increment, decrement)
    // If the command is "msleep", call msleep with the provided argument
    if (strcmp(basic_cmd->cmd_name, "msleep") == 0)
        msleep(basic_cmd->cmd_arg);
    // If the command is "increment", call increment with the provided argument
    else if (strcmp(basic_cmd->cmd_name, "increment") == 0)
        increment(basic_cmd->cmd_arg);

    // Else, i.e. the command is "decrement", call decrement with the provided argument
    else
        decrement(basic_cmd->cmd_arg);

}

static void execute_job(Command job_cmd[MAX_COMMANDS_IN_JOB]) {
    //TODO: Look for memory leaks here!!!
    Command *curr_cmd;
    // Iterate through the commands in the job
    for(int i = 0; job_cmd[i].cmd_name[0] != '\0'; i++) {
        // Current command
        curr_cmd = job_cmd + i;
        // If the comand is repeat, execute next command arg times
        if (strcmp(curr_cmd->cmd_name, "repeat") == 0) {
            // Execute the remaining commands arg times
            for (int count = 0; count < curr_cmd->cmd_arg; count++) {
                for (int j = i + 1; job_cmd[j].cmd_name[0] != '\0'; j++) {
                    curr_cmd = job_cmd + j;
                    execute_basic_command(curr_cmd);
                }
            }
            // Break the loop as we have executed all commands in the job as part of repeat
            break;
        }
        // Execute each basic command sequentially
        execute_basic_command(curr_cmd);
    }
}

static void* thread_routine(void* arg) {
    // Main routine for each thread - fetch and execute jobs from the shared work queue
    while (1) {
        // Conditinal wait until a job is available in the work queue
        // First, lock the mutex for the work queue
        pthread_mutex_lock(&shared_jobs_queue.lock);
        // Wait until there is at least one job in the queue or exit flag is set
        while (!shared_jobs_queue.size && !shared_jobs_queue.exit_flag) {
            // Wait on the condition variable for new jobs and unlock the mutex while waiting
            pthread_cond_wait(&ava_jobs_cond, &shared_jobs_queue.lock);
        }
        if (shared_jobs_queue.exit_flag && shared_jobs_queue.size == 0) {
            // Unlock the mutex for the work queue
            pthread_mutex_unlock(&shared_jobs_queue.lock);
            // Exit the thread if exit flag is set and there are no more jobs
            pthread_exit(NULL);
        }
        // Fetch the job from the front of the queue
        Command *job_to_execute = pop_job();
        // Increment the number of working threads
        shared_jobs_queue.num_of_working_threads ++;
        // Unlock the mutex for the work queue
        pthread_mutex_unlock(&shared_jobs_queue.lock);
        // Execute the fetched job
        execute_job(job_to_execute);
        // Decrement the number of working threads
        pthread_mutex_lock(&shared_jobs_queue.lock);
        shared_jobs_queue.num_of_working_threads --;
        if (shared_jobs_queue.size == 0)
            pthread_cond_signal(&shared_jobs_queue.cond_idle);
        pthread_mutex_unlock(&shared_jobs_queue.lock);
        // Free the memory allocated for the job commands
        if (job_to_execute != NULL)
            free(job_to_execute);
    }
    pthread_exit(NULL);
}

void create_num_threads_threads(int num_threads, pthread_t *threads_array) {
    // Create num_threads threads which will run thread_routine
    // pthread create status
    int status;
    // Iterate to create num_threads threads
    for (int i = 0; i < num_threads; i++) {
        // Create a new thread and check for errors
        status = pthread_create(&threads_array[i], NULL, thread_routine, NULL);
        if (status != 0) {
            printf("hw2: pthread_create failed: %s,\nexiting\n", strerror(status));
            exit(EXIT_FAILURE);
        }
    }
}
