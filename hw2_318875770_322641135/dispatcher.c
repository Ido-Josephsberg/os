// TODO: Implement dispatcher initialization function:
    // Includes check number of parameters from command line. --------> DONE
    // Includes calling create_counter_files.  -----------------------> DONE
    // Includes calling create_threads.  -----------------------------> DONE

// TODO: Implement dispatcher_msleep ---------------------------------> DONE
// TODO: Implement dispatcher_wait.  ---------------------------------> DONE
// TODO: Implement finalize_dispatcher.
// TODO: Implement log enabled functionality.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "global_vars.h"
#include "macros.h"
#include "dispatcher.h"
#include "counter_files.h"
#include "job_queue.h"
#include "cmdfile_handler.h"
#include "threads.h"
#include "log_files.h"

// Declare Global available jobs condition variable
pthread_cond_t ava_jobs_cond = PTHREAD_COND_INITIALIZER;
// Declare Global shared jobs queue
JobQueue shared_jobs_queue;
/*Assistive Functions For Dispatcher*/

static void init_dispatcher(int num_counters, int num_threads, int log_enabled, pthread_t *threads_array) {
    // Validate num_counters and create counter files
    if ((num_counters <= 0) || (num_counters > MAX_COUNTER_FILES)) {
        printf("Invalid number of counters: %d\n", num_counters);
        exit(EXIT_FAILURE);
    }
    // Validate num_threads and create threads
    if ((num_threads <= 0) || (num_threads > MAX_WORKER_THREADS)) {
        printf("Invalid number of threads: %d\n", num_threads);
        exit(EXIT_FAILURE);
    }
    // Create num_threads threads
    create_num_threads_threads(num_threads, threads_array);
    // Create worker threads and dispatcher.txt log if log_enabled is set
    if (log_enabled) {
        create_threadxx_files(num_threads);
        // Create dispatcher.txt log
        FILE *fp = fopen("dispatcher.txt", "w");
        if (fp == NULL) {
            printf("Error creating %s\n", "dispatcher.txt");
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        if (fclose(fp) != 0) {
            printf("Error closing %s\n", "dispatcher.txt");
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
    }
    // Create counter files
    create_countxx_files(num_counters);
    // Creadete dispatcher.txt log
}

static void dispatcher_wait() {   
    //Lock the mutex to inspect the state safely
    pthread_mutex_lock(&shared_jobs_queue.lock);

    // Conditional Wait loop
    while (shared_jobs_queue.num_of_working_threads > 0 || shared_jobs_queue.size > 0) {
        /* pthread_cond_wait does the following:
           - Automatically unlocks the mutex so workers can progress.
           - Puts the dispatcher thread into a sleep state (blocking).
           - Re-locks the mutex immediately upon being signaled and waking up.
        */
        pthread_cond_wait(&shared_jobs_queue.cond_idle, &shared_jobs_queue.lock);
    }

    // Once active_jobs == 0, we have the lock and can proceed
    pthread_mutex_unlock(&shared_jobs_queue.lock);
}

static void run_dispatcher(FILE *cmd_file, int num_counters, int num_threads, int log_enabled) {
    // This function contains the dispatcher loop logic

    // Validate cmd_file
    if (cmd_file == NULL) {
        printf("Failed to open command file");
        exit(EXIT_FAILURE);
    }

    // Initialize shared Queue:
    shared_jobs_queue = (JobQueue) {NULL, NULL, 0, 0, log_enabled}; // Initialize an empty job queue
    pthread_mutex_init(&shared_jobs_queue.lock, NULL);
    pthread_cond_init(&shared_jobs_queue.cond_idle, NULL);

    // Dispatcher Loop
    char line[MAX_JOB_FILE_LINE];
    while (fgets(line, sizeof(line), cmd_file)) {
               
        // Check if a line is a worker or dispatcher command
        char* curr_line_ptr = line;

        //skip leading whitespaces
        while (*curr_line_ptr == ' ' || *curr_line_ptr == '\t') {
            curr_line_ptr++;
        }
        // Check if worker command
        if (strncmp(curr_line_ptr, "worker", 6) == 0) {
            // Process worker command - insert into shared job queue
            Command* job_cmds = (Command*)malloc(sizeof(Command) * (MAX_COMMANDS_IN_JOB + 1));
            if (job_cmds == NULL) {
                printf("hw2: memory allocation failed, exiting\n");
                exit(EXIT_FAILURE);
            }

            // Parse the worker line into job_cmds array
            parse_worker_line(curr_line_ptr + 6, job_cmds);
            // Lock the job queue mutex before pushing the job
            pthread_mutex_lock(&shared_jobs_queue.lock);
            push_job(job_cmds,line);
            pthread_mutex_unlock(&shared_jobs_queue.lock);
        }
        else {
            Command disp_cmd = {"", 0};
            parse_cmd(curr_line_ptr, &disp_cmd);
            
            // Check dispatcher commands:
            if (log_enabled){
                write_into_log_file(line, 0, 0, 1);
            }
            if (strcmp(disp_cmd.cmd_name, "dispatcher_msleep") == 0) {
                msleep(disp_cmd.cmd_arg);
            }
            else if (strcmp(disp_cmd.cmd_name, "dispatcher_wait") == 0) {
                dispatcher_wait();
            }
            else {
                printf("Unknown dispatcher command: %s\n", disp_cmd.cmd_name);
            }
        }          
        
    }    
    //Convention: closing the cmd_file in the dispatcher main function called dispatcher().
}

static void finalize_dispatcher(pthread_t *threads_array, int num_threads) {
    // Cleanup resources, write stats.txt, and exit all threads
    // Allocate exit command array
    Command *exit_cmd_array = (Command*)malloc(sizeof(Command) * num_threads);
    if (exit_cmd_array == NULL) {
        printf("hw2: memory allocation failed, exiting\n");
        exit(EXIT_FAILURE);
    }
    // Exit all threads and join them
    exit_all_threads(num_threads, threads_array, exit_cmd_array);
    // Free exit command array
    if (exit_cmd_array) {
        free(exit_cmd_array);
    }
    // TODO: Write stats.txt if needed
}

/*Main Dispatcher Function*/

void dispatcher(int argc, char *argv[]) {
    // check for correct number of arguments
    if (argc != 5) {
        printf("Usage: %s cmdfile.txt num_threads num_counters log_enabled\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //Extract user argumentsvalues
    FILE *cmd_file = fopen(argv[1], "r");
    if (!cmd_file) {
        printf("Failed to open command file");
        exit(EXIT_FAILURE);
    }
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    int log_enabled = atoi(argv[4]);

    // Create an array to hold thread IDs
    pthread_t threads_array[MAX_WORKER_THREADS];

    // Initialize dispatcher
    init_dispatcher(num_counters, num_threads, log_enabled, threads_array);

    // Run Dispatcher
    run_dispatcher(cmd_file,num_counters, num_threads, log_enabled);
        
    // Cleanup, write stats.txt, and exit
    finalize_dispatcher(threads_array, num_threads); 

    // Close command file
    if (fclose(cmd_file) != 0) {
        printf("Error closing command file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}
