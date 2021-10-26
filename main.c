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



// given a buffer, gets a command from the user and inputs it into the buffers
void getUserCommand(char *commandText) {
  memset(commandText, '\0', 2049*sizeof(char));
  putchar(':');
  putchar(' ');
  fflush(stdout);
  fgets(commandText, 2049, stdin);
  // Below line copied from: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
  // Purpose: How to remove newline character after fgets()   Date: 10/22/2021
  commandText[strcspn(commandText, "\n")] = 0;
}


int main() {
  registerHandler(SIGINT, SIG_IGN);  // ignore SIGINT signal
  registerHandler(SIGTSTP, handle_fgModeToggleSignal);  // set TSTP to toggle "foreground-only mode" on and off
  struct bgProcess *head = initializeBgProcess();  // linked list holding all currently running background processes
  char commandText[2049];  // holds user-inputted command; has max length of 2048 characters per instructions
  while(1) {
    reapBgProccesses(head);  // reap any background proceses that have terminated
    getUserCommand(commandText);  // prompt user for a command 
    if(!startsWithOrEmpty(commandText, '#')) {   // if command starts with '#' or is empty, do nothing; else continue
      struct command *myCommand = createCommand(commandText);  // parse the command and put info into a struct
      // printCommand(myCommand);  // for debugging
      runCommand(myCommand, head);  // execute the command
      freeCommand(myCommand);  // free the memory allocated for the struct
    }
  }
}