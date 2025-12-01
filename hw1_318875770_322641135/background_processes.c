#include "background_processes.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


void free_background_processes(AllBackgroundProcesses* all_background_processes) {
    /*
    Frees the memory allocated for all background processes.
    */
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        if (all_background_processes->processes[i]) {
            free(all_background_processes->processes[i]);
        }
    }
}

void reap_zombie_processes(AllBackgroundProcesses* all_background_processes, int wait_for_all) {
    /*
    Reaps any zombie background processes and notifies the user.
    */
    int return_pid;
    int options = wait_for_all ? 0 : WNOHANG;
    for (int i = 0; i < MAX_BACKGROUND_PROCESSES; i++) {
        BackgroundProcess* process = all_background_processes->processes[i];
        if (process->pid != -1) {
            int return_pid = waitpid(process->pid, NULL, options);
            printf("hw1shell: %s failed, errno is %d\n", "waitpid", errno);
            if (return_pid == -1) {
                // TODO: handle error
                continue;
            } else if (return_pid > 0) {
                // Process has terminated
                printf("hw1shell: pid %d finished\n", process->pid);
                process->pid = -1; // Mark the slot as empty
                all_background_processes->count--;
            }
        }
    }
}