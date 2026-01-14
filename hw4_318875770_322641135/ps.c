#include "types.h"
#include "stat.h"
#include "user.h"
#include "processInfo.h"

// State names matching the enum in proc.h
char *stateNames[] = {
  "unused",
  "embryo  ",
  "sleeping",
  "runnable",
  "running ",
  "zombie  "
};

int
main(int argc, char *argv[])
{
  int maxpid;
  int pid;
  struct processInfo pinfo;

  maxpid = getMaxPid();

  // Print header
  printf(1, "Total number of active processes: %d\n", getNumProc());
  printf(1, "Maximum PID: %d\n", maxpid);
  printf(1, "PID\tSTATE\t\tPPID\tSZ\tNFD\tNRSWITCH\n");


  for(pid = 1; pid <= maxpid; pid++){
    if(getProcInfo(pid, &pinfo) == 0){
        printf(1, "%d\t%s\t%d\t%d\t%d\t%d\n",
            pid,
            stateNames[pinfo.state],
            pinfo.ppid,
            pinfo.sz,
            pinfo.nfd,
            pinfo.nrswitch);
    
    }
  }

  exit();
}
