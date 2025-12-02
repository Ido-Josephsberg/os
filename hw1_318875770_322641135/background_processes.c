#include "background_processes.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void reap_zombie_processes(AllBackgroundProcesses* all_background_processes, int wait_for_all) {
    /*
    Reaps any zombie background processes. If wait_for_all is TRUE,
    waits for all background processes to finish (used when exiting the shell).
    If FALSE, only reaps processes that have already finished and notifies the user.
    */
    int return_pid;
    // Set options for waitpid based on whether to wait for all processes or not
    int options = wait_for_all ? 0 : WNOHANG;
    // Iterate through all background processes
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        // Get the background process at index i
        BackgroundProcess* process = &(all_background_processes->processes[i]);
        // If the process is existing
        if (process->pid != -1) {
            // Wait for the process based on options (non-blocking or blocking)
            int return_pid = waitpid(process->pid, NULL, options);
            // Check the result of waitpid
            if (return_pid == -1) {
                // Notify about system call failure
                printf("hw1shell: %s failed, errno is %d\n", "waitpid", errno);
            } else if (return_pid > 0 && (!wait_for_all)) {
                // Process has terminated and not on 'wait for all' mode (not after exit command)
                // Notify the user that the background process has finished
                printf("hw1shell: pid %d finished\n", process->pid);
                process->pid = -1; // Mark the slot as empty
                all_background_processes->count--;
            }
        }
    }
}