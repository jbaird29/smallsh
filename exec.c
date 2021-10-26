#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"
#include "background.h"
#include "sighandle.h"
#include "status.h"

/* ------------------------ HELPER FUNCTIONS ------------------------ */

// redirects stdin or stdout to point to new filename; if stdFileNo is 0 -> stdin; if stdFileNo is 1 -> stdout
static void redirectStd(char *filename, int stdFileNo) {
  int openFlags = (stdFileNo == 0) ? (O_RDONLY) : (O_WRONLY | O_CREAT | O_TRUNC);  // 0 for stdin, 1 for stdout
  int fd = open(filename, openFlags, 0644);  // open the file
  if(fd == -1) {    // ensure the file was opened correctly
    stdFileNo == 0 ? perror("input open()") : perror("output open()");
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
    if(myCommand->outputFile) redirectStd(myCommand->outputFile, 1);
    registerDefaultSIGINT();  // foregound processes should terminate upon receiving SIGINT
  } else {  // if background process
    myCommand->inputFile ? redirectStd(myCommand->inputFile, 0) : redirectStd("/dev/null", 0);  // redir to input or /dev/null
    myCommand->outputFile ? redirectStd(myCommand->outputFile, 1) : redirectStd("/dev/null", 1); 
    registerIgnoreSIGINT();  // background processes should ignore SIGINT
  }
  char *newargv[myCommand->argCount + 2]; // ["ls", "-a", "-l", NULL]  length is # of arguments, add two for the program and NULL
  fillExecVector(myCommand, newargv);  // fill the array with the strings listed above
  execvp(newargv[0], newargv);  // execute the command
  perror("execvp");  // only reaches this code upon an error
  exit(EXIT_FAILURE);
}


// runs fork() and exec() to execute a command
static void runExecCommand(struct command *myCommand, int *lastExitStatus, struct bgProcess *head) {
	pid_t childPid = fork();  // in the child: equals 0;  in the parent: equals the child's pid
  if(childPid == -1) {
    perror("fork() failed!");
    exit(1);
  } else if(childPid == 0) {  // child process
    runChildProcess(myCommand);
  } else {  // parent process
    if(!myCommand->isBackground) {  // if foreground, we need to block
      waitpid(childPid, lastExitStatus, 0);  // blocking until the child process terminates
      if(!WIFEXITED(*lastExitStatus)) printStatus(*lastExitStatus);  // if terminated by signal, print that signal
    } else {  // if background, we do not block
      printf("background pid is %d\n", childPid);
      fflush(stdout);
      bgProcessInsertFront(head, childPid);  // insert this processID into the linked list, to be reaped later
    }
  }
}




/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

void runCommand(struct command *myCommand, int *lastExitStatus, struct bgProcess *head) {
  if(strcmp(myCommand->program, "exit") == 0) {
    // TODO ------------------------------------------------------------------
    // The exit command exits your shell. It takes no arguments. When this command is run, 
    // your shell must kill any other processes or jobs that your shell has started before it terminates itself.
    exit(EXIT_SUCCESS);
  } else if (strcmp(myCommand->program, "cd") == 0) {
    // TODO ------------------------------------------------------------------
    // The cd command changes the working directory of smallsh.
    // By itself - with no arguments - it changes to the directory specified in the HOME environment variable
      // This is typically not the location where smallsh was executed from, unless your shell executable is located in the HOME directory, in which case these are the same.
    // This command can also take one argument: the path of a directory to change to. Your cd command should support both absolute and relative paths.
  } else if (strcmp(myCommand->program, "status") == 0) {
    printStatus(*lastExitStatus);  // prints the exit val or term signal to stdout
  } else {
    runExecCommand(myCommand, lastExitStatus, head);
  }
}