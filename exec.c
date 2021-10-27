#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "structs.h"
#include "background.h"
#include "sighandle.h"
#include "status.h"
#include "globals.h"
#include "parse.h"
#include "cd.h"


/* ------------------------ HELPER FUNCTIONS ------------------------ */
// redirects stdin or stdout to point to new filename; if stdFileNo is 0 -> stdin; if stdFileNo is 1 -> stdout
static void redirectStd(char *filename, int stdFileNo) {
  int openFlags = (stdFileNo == 0) ? (O_RDONLY) : (O_WRONLY | O_CREAT | O_TRUNC);  // 0 for stdin, 1 for stdout
  int fd = open(filename, openFlags, 0644);  // open the file
  if(fd == -1) {    // ensure the file was opened correctly
    printf("cannot open %s for %s.\n", filename, stdFileNo == 0 ? "input" : "output");
    fflush(stdout);
    exit(EXIT_FAILURE); 
  }
  int result = dup2(fd, stdFileNo);  // redirect stdin/stdout to the input/output file
  if(result == -1) {  // ensure redirection was successful
    stdFileNo == 0 ? perror("input dup2()") : perror("output dup2()");
    exit(EXIT_FAILURE); 
  }
}


// converts the commany strcut and arguments into a vector, ready to be passed to an execv() function
static void fillExecVector(struct command *myCommand, char *newargv[]) {
  // ["ls", "-a", "-l", NULL]
  newargv[0] = myCommand->program;  // the first element is the program ["ls"]
  for(int i = 0; i < myCommand->argCount; i++) newargv[i+1] = myCommand->arguments[i];  // next elements are the arguments ["-a", "-l"]
  newargv[myCommand->argCount + 1] = NULL;  // the final element is the NULL [NULL]
}


// given a command, runs child process -> redirects in/output, sets up sig handlers, and executes the program with its args
static void runChildProcess(struct command *myCommand) {
  if(!myCommand->isBackground) {  // if foreground process
    if(myCommand->inputFile) redirectStd(myCommand->inputFile, 0);  // redir to input if provided, else leave as stdin
    if(myCommand->outputFile) redirectStd(myCommand->outputFile, 1);  // redir to output if provided, else leave as stdin
    registerHandler(SIGINT, SIG_DFL);  // foregound processes should terminate upon receiving SIGINT
    registerHandler(SIGTSTP, SIG_IGN);  // foregound processes should ignore SIGTSP
  } else {  // if background process
    myCommand->inputFile ? redirectStd(myCommand->inputFile, 0) : redirectStd("/dev/null", 0);  // redir to input or /dev/null
    myCommand->outputFile ? redirectStd(myCommand->outputFile, 1) : redirectStd("/dev/null", 1);  // redir to output or /dev/null
    registerHandler(SIGINT, SIG_IGN);  // background processes should ignore SIGINT
    registerHandler(SIGTSTP, SIG_IGN);  // background processes should ignore SIGTSP
  }
  char *newargv[myCommand->argCount + 2]; // ["ls", "-a", "-l", NULL]  length is # of arguments, add two for the program and NULL
  fillExecVector(myCommand, newargv);  // fill the array with the strings listed above
  execvp(newargv[0], newargv);  // execute the command
  perror(myCommand->program);
  // perror("execvp");  // only reaches this code upon an error
  exit(EXIT_FAILURE);
}


// runs fork() and exec() to execute a command
static void runExecCommand(struct command *myCommand, struct bgProcess *head) {
	pid_t childPid = fork();  // in the child: equals 0;  in the parent: equals the child's pid
  if(childPid == -1) {
    perror("fork() failed!");
    exit(1);
  } else if(childPid == 0) {  // child process
    runChildProcess(myCommand);
  } else {  // parent process
    if(!myCommand->isBackground) {  // if foreground, we need to block
      lastFpProcess.childPid = childPid;  // set the global variable lastFpProcess 
      // NOTE: waitpid is also called in SIGTSTP handler; if that reaps the child process, result will equal -1 but wstatus will still be validly set
      waitpid(childPid, &lastFpProcess.wstatus, 0);  // blocking until the child process terminates; set global variable lastFpProcess 
      if(!WIFEXITED(lastFpProcess.wstatus)) printStatus(lastFpProcess.wstatus);  // if terminated by signal, print that signal
    } else {  // if background, we do not block
      printf("background pid is %d\n", childPid);
      fflush(stdout);
      bgProcessInsertFront(head, childPid);  // insert this processID into the linked list, to be reaped later
    }
  }
}




/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

void runCommand(struct command *myCommand, char *commandText, struct bgProcess *head) {
  if(strcmp(myCommand->program, "exit") == 0) {
    terminateBgProccesses(head);  // sends SIGTERM signals to all bg processes and frees the memory consumed by the list
    freeCommand(myCommand);  // free the memory allocated for the struct
    freeUserCommand(commandText);  // free the memory allocated for the user command
    exit(EXIT_SUCCESS);
  } else if (strcmp(myCommand->program, "cd") == 0) {
    runCd(myCommand); 
  } else if (strcmp(myCommand->program, "status") == 0) {
    printStatus(lastFpProcess.wstatus);  // prints the exit val or term signal to stdout
  } else {
    runExecCommand(myCommand, head);
  }
}