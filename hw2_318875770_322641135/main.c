#include <stdlib.h>
#include <sys/time.h>
#include "global_vars.h"
#include "dispatcher.h"

struct timeval global_start_time; // Global start time

long long get_elapsed_time_ms() {
    //Return the times passed since the program started in milliseconds
    struct timeval now;
    gettimeofday(&now, NULL);
    long long seconds = now.tv_sec - global_start_time.tv_sec;
    long long microseconds = now.tv_usec - global_start_time.tv_usec;
    return (seconds * 1000) + (microseconds / 1000);
}

int main(int argc, char *argv[]) {
    // Set  Time zero immidiatly
    gettimeofday(&global_start_time, NULL); // Set Time Zero immediately
    //Call Dispatcher with command line arguments
    dispatcher(argc, argv);
    return 0;
}