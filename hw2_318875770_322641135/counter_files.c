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
    for (int i = 1; i <= num_files; i++) {
        char filename[MAX_FILE_NAME];
        // Write filename into file
        snprintf(filename, sizeof(filename), "count%02d.txt", i);

        //Opens file for writing, return error if fails
        FILE *fp = fopen(filename, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error creating %s: %s\n", filename, strerror(errno));
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        // Initialize file content to 0 (long long type)
        if (fprintf(fp, "%lld\n", 0LL) < 0) {
            fprintf(stderr, "Error writing to %s: %s\n", filename, strerror(errno));
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
    int fd = open(file_name, O_RDWR);
    // Check for errorsS
    if (fd == -1) {
        print_sys_call_error("open");
        pthread_mutex_unlock(file_counters_mutexes + file_number);
        return;
    }
    // Read the current value from the file
    char str_counter[MAX_DIGITS];
    if (read(fd, str_counter, MAX_DIGITS) == -1) {
        print_sys_call_error("read");
        close(fd);
        pthread_mutex_unlock(file_counters_mutexes + file_number);
        return;
    }
    // Convert the read string to long long integer
    long long counter_value = atoll(str_counter);
    // Increment or decrement the counter value based on inc_flag
    counter_value += (inc_flag ? 1 : -1);
    // Move the file offset to the beginning of the file. Notify in case of failure
    if (lseek(fd, 0, SEEK_SET) == -1) {
        print_sys_call_error("lseek");
        close(fd);
        pthread_mutex_unlock(file_counters_mutexes + file_number);
        return;
    }
    // Convert the updated counter value back to string
    sprintf(str_counter, "%lld", counter_value);
    // Write the updated counter value back to the file and notify in case of failure
    if (write(fd, str_counter, strlen(str_counter)) == -1) {
        print_sys_call_error("write");
        close(fd);
        pthread_mutex_unlock(file_counters_mutexes + file_number);
        return;
    }
    // Close the file descriptor and notify in case of failure
    if (close(fd) == -1) {
        print_sys_call_error("close");
        pthread_mutex_unlock(file_counters_mutexes + file_number);
        return;
    }
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
