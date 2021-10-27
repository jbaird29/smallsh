#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "structs.h"
#include "parse.h"
#include "exec.h"
#include "background.h"
#include "sighandle.h"
#include "globals.h"



int main() {
  registerHandler(SIGINT, SIG_IGN);  // ignore SIGINT signal
  registerHandler(SIGTSTP, handle_fgModeToggleSignal);  // set TSTP to toggle "foreground-only mode" on and off
  struct bgProcess *head = initializeBgProcess();  // linked list holding all currently running background processes
  while(1) {
    reapBgProccesses(head);  // reap any background proceses that have terminated
    char *commandText = getUserCommand();  // prompt user for a command 
    if(!startsWithOrEmpty(commandText, '#')) {   // if command starts with '#' or is empty, do nothing; else continue
      struct command *myCommand = createCommand(commandText);  // parse the command and put info into a struct
      // printCommand(myCommand);  // for debugging
      runCommand(myCommand, commandText, head);  // execute the command
      freeCommand(myCommand);  // free the memory allocated for the struct
    }
    freeUserCommand(commandText);  // free the memory allocated for the user command;
  }
}