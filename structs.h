#ifndef COMMAND_H
#define COMMAND_H 

#define MAX_ARGUMENTS 512

// structured representation of a user-inputted command
struct command {
  char *program;
  char *arguments[MAX_ARGUMENTS]; // maximum of 512 arguments
  int argCount;
  char *inputFile;
  char *outputFile;
  bool isBackground;
};


// linked list for holding the pid of children running in background
struct bgProcess {
  pid_t childPid;
  struct bgProcess *next;
};

// holds the pid and wstatus of the most recently executed foreground process
struct lastProcess {
  pid_t childPid;
  int wstatus;
};

#endif