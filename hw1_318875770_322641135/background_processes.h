#define MAX_LINE_LENGTH 1024
#define MAX_BACKGROUND_PROCESSES 4
typedef struct {
    int pid;
    char command[MAX_LINE_LENGTH];
} BackgroundProcess;

typedef struct {
    BackgroundProcess processes[MAX_BACKGROUND_PROCESSES];
    int count;
} AllBackgroundProcesses;

