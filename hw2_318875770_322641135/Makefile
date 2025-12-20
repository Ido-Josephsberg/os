all: main
	gcc main -o hw2
main: main.c
	gcc -pthread -g -Wall -c main.c -o main
cmdfile_handler: cmdfile_handler.c cmdfile_handler.h
	gcc -pthread -g -Wall -c cmdfile_handler.c -o cmdfile_handle
counter_files: counter_files.c counter_files.h
	gcc -pthread -g -Wall -c counter_files.c -o counter_files
dispatcher: dispatcher.c dispatcher.h
	gcc -pthread -g -Wall -c dispatcher.c -o dispatcher
job_queue: job_queue.c job_queue.h
	gcc -pthread -g -Wall -c job_queue.c -o job_queue
system_call_error: system_call_error.c system_call_error.h
	gcc -pthread -g -Wall -c system_call_error.c -o system_call_error
	

clean:
	\rm hw2 main
