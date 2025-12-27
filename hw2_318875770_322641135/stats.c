#include <stdio.h>
#include <stdlib.h>
#include "global_vars.h"
#include "log_files.h"
#include "stats.h"

static void write_stats(FILE *fp) {
    // Write statistics into the provided file pointer
    // Write total run time
    fprintf(fp, "total running time: %lld milliseconds\n", get_elapsed_time_ms());
    fprintf(fp, "sum of jobs turnaround time: %lld milliseconds\n", shared_jobs_queue.total_turnaround_time_ms);
    fprintf(fp, "min job turnaround time: %lld milliseconds\n", shared_jobs_queue.min_turnaround_time_ms);
    float average_turnaround_time = 0.0;
    if (shared_jobs_queue.total_jobs_added > 0) 
        average_turnaround_time = (float)shared_jobs_queue.total_turnaround_time_ms / shared_jobs_queue.total_jobs_added;
    fprintf(fp, "average job turnaround time: %f milliseconds\n", average_turnaround_time);
    fprintf(fp, "max job turnaround time: %lld milliseconds\n", shared_jobs_queue.max_turnaround_time_ms);
}
void create_stats_file() {
    // Create and initialize stats.txt file
    FILE *fp = fopen("stats.txt", "w");
    if (fp == NULL) {
        printf("Error creating stats.txt\n");
        exit(EXIT_FAILURE);
    }
    // Write stats content
    write_stats(fp);
    // Close file
    if (fclose(fp) != 0) {
        printf("Error closing stats.txt\n");
        exit(EXIT_FAILURE);
    }
}