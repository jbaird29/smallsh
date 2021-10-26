#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "command.h"
#include "parse.h"
#include "exec.h"
#include "background.h"
#include "sighandle.h"


int main() {
  registerHandler(SIGINT, SIG_IGN);  // ignore SIGINT signal
  char commandText[2049];  // max length of 2048 characters
  int lastExitStatus = 0;
  struct bgProcess *head = initializeBgProcess();
  while(1) {
    reapBgProccesses(head);
    memset(commandText, '\0', sizeof(commandText));
    putchar(':');
    putchar(' ');
    fflush(stdout);
    fgets(commandText, 2049, stdin);
    // Below line copied from: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
    // Purpose: How to remove newline character after fgets()   Date: 10/22/2021
    commandText[strcspn(commandText, "\n")] = 0;
    if(startsWithOrEmpty(commandText, '#')) { 
      continue; // if command starts with '#' or is empty, do nothing
    }
    struct command *myCommand = createCommand(commandText);
    // printCommand(myCommand);  // for debugging
    runCommand(myCommand, &lastExitStatus, head);
    freeCommand(myCommand);
  }
}