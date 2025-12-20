// TODO: Implement dispatcher initialization function:
    // Includes check number of parameters from command line.
    // Includes calling create_counter_files.
    // Includes calling create_threads.

// TODO: Implement dispatcher_msleep -> DONE
// TODO: Implement dispatcher_wait.
// TODO: Implement finalize_dispatcher.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "dispatcher.h"
#include "macros.h"
#include "counter_files.h"
#include "cmdfile_handler.h"
#include "log_files.h"

/*Assistive Functions For Dispatcher*/

void init_dispatcher(int num_counters, int num_threads, int log_enabled) {
    // Validate num_counters and create counter files
    if ((num_counters <= 0) || (num_counters > MAX_COUNTER_FILES)) {
        fprintf(stderr, "Invalid number of counters: %d\n", num_counters);
        exit(EXIT_FAILURE);
    }
    // Create counter files
    create_counterxx_files(num_counters);

    // Validate num_threads and create threads
    if ((num_threads <= 0) || (num_threads > MAX_WORKER_THREADS)) {
        fprintf(stderr, "Invalid number of threads: %d\n", num_threads);
        exit(EXIT_FAILURE);
    }
    // Create worker threads if log_enabled is set
    if (log_enabled) {
        create_threadxx_files(num_threads);
    }

}

void run_dispatcher(FILE *cmd_file, int num_counters, int num_threads, int log_enabled) {
    // This function contains the dispatcher loop logic

    // Validate cmd_file
    if (cmd_file == NULL) {
        perror("Failed to open command file");

        exit(EXIT_FAILURE);
    }

    // Initialize shared Queue:
    JobQueue shared_job_queue = {NULL, NULL, 0}; // Initialize an empty job queue

    // Dispatcher Loop
    char line[MAX_JOB_FILE_LINE];
    while (fgets(line, sizeof(line), cmd_file)) {
               
        // Check if a line is a worker or dispatcher command
        //int is_worker_cmd = 0; // TODO: Implement logic to determine if it's a worker command
        char* curr_line_ptr = line;

        //skip leading whitespaces
        while (*curr_line_ptr == ' ' || *curr_line_ptr == '\t') {
            curr_line_ptr++;
        }
        // Check if worker command
        if (strncmp(curr_line_ptr, "worker", 6) == 0) {
            // Process worker command - insert into shared job queue
            Command job_cmds[MAX_COMMANDS_IN_JOB];
            parse_worker_line(line, job_cmds);
            push_job(job_cmds, shared_job_queue);
        }
        else {
            Command disp_cmd = {NULL, 0};
            parse_cmd(line, &disp_cmd);
            // TODO implement dispatcher command calling
        }          
        
        // Wait for all background commands to complete before processing next line
        dispatcher_wait();
    }    
    //Convention: closing the cmd_file in the dispatcher main function called dispatcher().
}

void dispatcher_msleep(int milliseconds /*TODO: insert more args if needed*/) {
    usleep(milliseconds * 1000); // Convert milliseconds to microseconds, the usleep meansurement argument
}

void dispatcher_wait() {
    //TODO: Implement wait logic
}

void finalize_dispatcher() {
    //TODO: Implement finalization logic
}

/*Main Dispatcher Function*/

void dispatcher(int argc, char *argv[]) {
    // check for correct number of arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s cmdfile.txt num_threads num_counters log_enabled\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //Extract user argumentsvalues
    FILE *cmd_file = fopen(argv[1], "r");
    if (!cmd_file) {
        perror("Failed to open command file");
        exit(EXIT_FAILURE);
    }
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    int log_enabled = atoi(argv[4]);

    // Initialize dispatcher
    init_dispatcher(num_counters, num_threads, log_enabled);

    //Dispatcher Loop
    char line[MAX_JOB_FILE_LINE];  
    while (fgets(line, sizeof(line), cmd_file)) {
        // Remove trailing newline if necessary
        run_dispatcher(cmd_file,num_counters, num_threads, log_enabled);
    }
    
    // Cleanup, write stats.txt, and exit
    finalize_dispatcher(); 

    // Close command file
    if (fclose(cmd_file) != 0) {
        fprintf(stderr, "Error closing command file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }
}