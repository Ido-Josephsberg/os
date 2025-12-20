#pragma once

/*Files*/
#define MAX_FILE_NAME 64      //max length of file name
#define MAX_JOB_FILE_LINE 1024 //max chars in job file line

/*Threads & Counters*/
#define MAX_WORKER_THREADS 4096 //max worker threads
#define MAX_COUNTER_FILES 100  //max counter files

/*Jobs*/
#define MAX_COMMANDS_IN_JOB 128 //max chars in job line. min command + min arg + space = 8 chars -> floor(1024/8) = 128
