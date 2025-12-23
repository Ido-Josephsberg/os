#include <stdlib.h>
#include <sys/time.h>
#include "global_vars.h"
#include "dispatcher.h"

struct timeval global_start_time; // Global start time

int main(int argc, char *argv[]) {
    // Set  Time zero immidiatly
    gettimeofday(&global_start_time, NULL); // Set Time Zero immediately
    //Call Dispatcher with command line arguments
    dispatcher(argc, argv);
    return 0;
}