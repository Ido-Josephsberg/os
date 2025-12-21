all: main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error global_vars
    gcc -pthread -g -Wall -o hw2 main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error global_vars

main: main.c
    gcc -pthread -g -Wall -c main.c -o main

dispatcher: dispatcher.c dispatcher.h
    gcc -pthread -g -Wall -c dispatcher.c -o dispatcher

threads: threads.c threads.h
    gcc -pthread -g -Wall -c threads.c -o threads

counter_files: counter_files.c counter_files.h
    gcc -pthread -g -Wall -c counter_files.c -o counter_files

cmdfile_handler: cmdfile_handler.c cmdfile_handler.h
    gcc -pthread -g -Wall -c cmdfile_handler.c -o cmdfile_handler

job_queue: job_queue.c job_queue.h
    gcc -pthread -g -Wall -c job_queue.c -o job_queue

log_files: log_files.c log_files.h
    gcc -pthread -g -Wall -c log_files.c -o log_files

system_call_error: system_call_error.c system_call_error.h
    gcc -pthread -g -Wall -c system_call_error.c -o system_call_error

global_vars: global_vars.c global_vars.h
    gcc -pthread -g -Wall -c global_vars.c -o global_vars

clean:
    rm -f hw2 main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error global_vars