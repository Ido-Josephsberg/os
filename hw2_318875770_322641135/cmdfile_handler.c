// TODO: Implement commands from command files "cmd_file_executor".
    // Should read commands from each line of the command file, dispatcher  commands should be executed 
    // sequentially.
    // Commands starting with worker should be assigned to shared work queue (to be executed by worker threads
    // when available).

// QUESTIONS:
    // 1. Does the is there a jobs queue struct already defined? or should I define one?

#include "dispatcher.h"
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>

void process_cmdfile_line(const char *line) {   
    //TODO: Implement command parsing and dispatching logic
    //          Parse and process each command in the line
    //          check if worker or dispatcher command

    // NOTICE: if every fail checking here, close the cmd file in dispatcher main function.

    // Think if you want first to insert the line into a job/queue struct or parse it directly first.
    
    
}

void insert_job_into_queue(/*TODO: complete args*/) {
    //TODO: Implement job insertion logic
}
