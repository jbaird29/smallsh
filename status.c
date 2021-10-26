#include <stdio.h>
#include <stdlib.h>


// given a wstatus obtained by wait() or waitpid() - prints the exit value or terminating signal to stdout
void printStatus(wstatus) {
  if(WIFEXITED(wstatus)){
    printf("exit value %d\n", WEXITSTATUS(wstatus));
  } else{
    printf("terminated by signal %d\n", WTERMSIG(wstatus));
  }
  fflush(stdout);
}