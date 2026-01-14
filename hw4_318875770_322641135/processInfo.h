// Process information structure for getProcInfo system call
struct processInfo {
  int state;      // Process state
  int ppid;       // Parent process PID
  int sz;         // Size of process memory, in bytes
  int nfd;        // Number of open file descriptors in the process
  int nrswitch;   // Number of context switches in
};
