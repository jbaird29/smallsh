#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 
#include "structs.h"
#include "globals.h"
#include <limits.h>


// changes the working directory of smallsh; with no arguments -> changes to HOME; one argument -> changes to that path
void runCd(struct command *myCommand) {
  char path[PATH_MAX+1];
  if(myCommand->argCount == 0) {  // no arguments - cd changes to the directory specified in the HOME environment variable
    int result = chdir(getenv("HOME"));
    if(result == -1) perror("cd");
  } else if(myCommand->argCount == 1) {  // one argument - the path of a directory to change to
    int result = chdir(myCommand->arguments[0]);
    if(result == -1) perror("cd");
  } else {
    printf("cd: only takes one argument");
    fflush(stdout);
  }
  // Usage of the PATH_MAX constant copied from: https://stackoverflow.com/questions/4992108/getcwd-second-parameter
  // Date: 10/26/2021
  getcwd(path, PATH_MAX+1); // stores the cwd into the path variable
  setenv("PWD", path, 1);  // updates the PWD environment variable with the new working directory
}