#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "commandstruct.h"

/* ------------------------ HELPER FUNCTIONS ------------------------ */

// redirects stdin and stdout to the input and output file names, if provided in the command
static void redirectInputOrOutput(struct command *myCommand) {
  if(myCommand->inputFile) {
    int inputFD = open(myCommand->inputFile, O_RDONLY);  // Open input file
    if(inputFD == -1) {   // ensure the file was opened correctly
      perror("input open()"); 
      exit(EXIT_FAILURE); 
    }
    int result = dup2(inputFD, STDIN_FILENO);  // redirect stdin to the input file
    if(result == -1) {
      perror("input dup2()"); 
      exit(EXIT_FAILURE); 
    }
  }
  if(myCommand->outputFile) {
    int outputFD = open(myCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open output file
    if(outputFD == -1) {   // ensure the file was opened correctly
      perror("output open()"); 
      exit(EXIT_FAILURE); 
    }
    int result = dup2(outputFD, STDOUT_FILENO);  // redirect stdout to the output file
    if(result == -1) {
      perror("output dup2()"); 
      exit(EXIT_FAILURE); 
    }
  }
}

// converts the commany strcut and arguments into a vector, ready to be passed to an execv() function
static void fillExecVector(struct command *myCommand, char *newargv[]) {
  // ["ls", "-a", "-l", NULL]
  newargv[0] = myCommand->program;  // the first element is the program ["ls"]
  for(int i = 0; i < myCommand->argCount; i++) newargv[i+1] = myCommand->arguments[i];  // next elements are the arguments ["-a", "-l"]
  newargv[myCommand->argCount + 1] = NULL;  // the final element is the NULL [NULL]
}


/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

// runs fork() and exec() to execute a command
void executeOtherCommand(struct command *myCommand) {
  // TODO ------------------------------------------------------------------
  // Your shell will execute any commands other than the 3 built-in command by using fork(), exec() and waitpid()
  // Whenever a non-built in command is received, the parent (i.e., smallsh) will fork off a child.
  // The child will use a function from the exec() family of functions to run the command.
  // Your shell should use the PATH variable to look for non-built in commands, and it should allow shell scripts to be executed
  // If a command fails because the shell could not find the command to run, then the shell will print an error message and set the exit status to 1
  // A child process must terminate after running a command (whether the command is successful or it fails).
	pid_t childPid = fork();  // in the child: equals 0;  in the parent: equals the child's pid
  if(childPid == -1) {
    perror("fork() failed!");
    exit(1);
  } else if(childPid == 0) {
    char *newargv[myCommand->argCount + 2]; // ["ls", "-a", "-l", NULL]  length is # of arguments, add two for the program and NULL
    fillExecVector(myCommand, newargv);  // fill the array with the strings listed above
    redirectInputOrOutput(myCommand);  // set up redirection of input file and output file
    execvp(newargv[0], newargv);  // execute the command
    perror("execvp");  // only reaches this code upon an error
    exit(EXIT_FAILURE);
  } else {
    // if background, we need to not block; if foreground, we need to block
    int childStatus;
    if(!myCommand->isBackground) {
      childPid = waitpid(childPid, &childStatus, 0);
    } else {
      // WNOHANG specified. If the child hasn't terminated, waitpid will immediately return with value 0
      childPid = waitpid(childPid, &childStatus, WNOHANG);
    }
  }
}