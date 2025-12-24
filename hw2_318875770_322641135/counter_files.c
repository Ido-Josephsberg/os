// Create num_files counter files and initialize them to zero.

// IDO NOTE THE CHANGE I'VE MADE: pthread_mutex_unlock(&file_counters_mutexes[file_number]), pthread_mutex_unlock expects a poiner
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "global_vars.h"
#include "macros.h"
#include "counter_files.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "system_call_error.h"

// Global array of mutexes for counter files
pthread_mutex_t file_counters_mutexes[MAX_COUNTER_FILES];

void create_countxx_files(int num_files) {
    if (num_files <= 0) {
        printf("Invalid num_files: %d\n", num_files);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }

    // Initialize counter files
    for (int i = 0; i < num_files; i++) {
        char filename[MAX_FILE_NAME];
        // Write filename into file
        snprintf(filename, sizeof(filename), "count%02d.txt", i);

        //Opens file for writing, return error if fails
        FILE *fp = fopen(filename, "w");
        if (fp == NULL) {
            printf("Error creating %s\n", filename);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        // Initialize file content to 0 (long long type)
        if (fprintf(fp, "%lld\n", 0LL) < 0) {
            printf("Error writing to %s\n", filename);
            fclose(fp);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        // close file
        if (fclose(fp) != 0) {
            fprintf(stderr, "Error closing %s: %s\n", filename, strerror(errno));
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        // Initialize mutex for this file counter
        pthread_mutex_init(file_counters_mutexes + i, NULL);
    }
}

static void inc_dec_counter_file(int file_number, int inc_flag) {
    // Increment or decrement (increment if inc_flag is 1) file counter number file_number.
    // File path
    char file_name[12];
    sprintf(file_name, "count%02d.txt", file_number);
    // Lock the mutex corresponding to the file counter before accessing it to prevent race conditions.
    pthread_mutex_lock(file_counters_mutexes + file_number);
    // Open file counter number file_number
    FILE *fp = fopen(file_name, "r");
    // Check for errorsS
    if (fp == NULL) {
        printf("Error reading %s\n", file_name);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }
    // Read the current value from the file
    char str_counter[MAX_DIGITS];
    fgets(str_counter, MAX_DIGITS, fp);
    if (str_counter[strlen(str_counter) - 1] == '\n') {
        str_counter[strlen(str_counter) - 1] = '\0'; // Remove newline character
    }
    fclose(fp);
    // Convert the read string to long long integer
    long long counter_value = atoll(str_counter);
    // Increment or decrement the counter value based on inc_flag
    counter_value += (inc_flag ? 1 : -1);
    // Open the file again for writing the updated value
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("Error opening %s for writing\n", file_name);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }
    // Convert the updated counter value back to string
    fprintf(fp, "%lld\n", counter_value);
    // Close the file
    fclose(fp);
    // Unlock the file mutex
    pthread_mutex_unlock(file_counters_mutexes + file_number);
}   

void increment(int file_number) {
    // Increment file counter number file_number
    inc_dec_counter_file(file_number, 1);
}

void decrement(int file_number) {
    // Decrement file counter number file_number
    inc_dec_counter_file(file_number, 0);
}
