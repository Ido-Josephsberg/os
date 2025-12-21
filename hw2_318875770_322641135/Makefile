all: main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error
	gcc -pthread -g -Wall main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error -o hw2

main: main.c dispatcher.h
	gcc -pthread -g -Wall -c main.c -o main

dispatcher: dispatcher.c dispatcher.h threads.h counter_files.h cmdfile_handler.h job_queue.h log_files.h
	gcc -pthread -g -Wall -c dispatcher.c -o dispatcher

threads: threads.c threads.h counter_files.h cmdfile_handler.h job_queue.h system_call_error.h
	gcc -pthread -g -Wall -c threads.c -o threads

counter_files: counter_files.c counter_files.h system_call_error.h
	gcc -pthread -g -Wall -c counter_files.c -o counter_files

cmdfile_handler: cmdfile_handler.c cmdfile_handler.h job_queue.h
	gcc -pthread -g -Wall -c cmdfile_handler.c -o cmdfile_handler

job_queue: job_queue.c job_queue.h
	gcc -pthread -g -Wall -c job_queue.c -o job_queue

log_files: log_files.c log_files.h
	gcc -pthread -g -Wall -c log_files.c -o log_files

system_call_error: system_call_error.c system_call_error.h
	gcc -pthread -g -Wall -c system_call_error.c -o system_call_error

clean:
	\rm hw2 main dispatcher threads counter_files cmdfile_handler job_queue log_files system_call_error
