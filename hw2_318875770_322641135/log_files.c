#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include "global_vars.h"
#include "macros.h"

long long get_elapsed_time_ms() {
    //Return the times passed since the program started in milliseconds
    struct timeval now;
    gettimeofday(&now, NULL);
    long long seconds = now.tv_sec - global_start_time.tv_sec;
    long long microseconds = now.tv_usec - global_start_time.tv_usec;
    return (seconds * 1000) + (microseconds / 1000);
}

void create_threadxx_files(int num_files) {
    if (num_files <= 0) {
        printf("Invalid num_files: %d\n", num_files);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }

    // Initialize thread files
    for (int i = 1; i <= num_files; i++) {
        char filename[MAX_FILE_NAME];
        snprintf(filename, sizeof(filename), "thread%d.txt", i);
        
        FILE *fp = fopen(filename, "w");
        if (fp == NULL) {
            printf("Error creating %s\n", filename);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        
        if (fprintf(fp, "%lld\n", 0LL) < 0) {
            printf("Error writing to %s\n", filename);
            fclose(fp);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        
        if (fclose(fp) != 0) {
            printf("Error closing %s\n", filename);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
    }
    
}

void write_thread_log(char* filename,char* line, int thread_num, int is_start) {
      // Validate that thread_num matches the filename threadXX.txt
    char expected_filename[MAX_FILE_NAME];
    sprintf(expected_filename, sizeof(len(filename)), "thread%d.txt", thread_num);
    
    if (strcmp(filename, expected_filename) != 0) {
        printf("Error: thread_num %d does not match filename %s (expected %s)\n", thread_num, filename, expected_filename);
        exit(EXIT_FAILURE);
    }
    // Write to thread log file the start or end time of a job
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("Error opening %s for appending\n", filename);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }
    long long current_time_ms = get_elapsed_time_ms();
    if (is_start) {
        if (fprintf(fp, " TIME %lld: START job %s\n", current_time_ms, line) < 0) {
            printf("Error writing start time to %s\n", filename);
            fclose(fp);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
    } else {
        if (fprintf(fp, " TIME %lld: END job %s\n", current_time_ms, line) < 0) {
            printf("Error writing end time to %s\n", filename);
            fclose(fp);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);

}