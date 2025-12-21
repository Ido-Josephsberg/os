#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "macros.h"

void create_threadxx_files(int num_files) {
    if (num_files <= 0) {
        fprintf(stderr, "Invalid num_files: %d\n", num_files);
        //close cmd_file in dispatcher main function
        exit(EXIT_FAILURE);
    }

    // Initialize thread files
    for (int i = 1; i <= num_files; i++) {
        char filename[MAX_FILE_NAME];
        snprintf(filename, sizeof(filename), "thread_%02d.txt", i);
        
        FILE *fp = fopen(filename, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error creating %s: %s\n", filename, strerror(errno));
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        
        if (fprintf(fp, "%lld\n", 0LL) < 0) {
            fprintf(stderr, "Error writing to %s: %s\n", filename, strerror(errno));
            fclose(fp);
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
        
        if (fclose(fp) != 0) {
            fprintf(stderr, "Error closing %s: %s\n", filename, strerror(errno));
            //close cmd_file in dispatcher main function
            exit(EXIT_FAILURE);
        }
    }
    
}