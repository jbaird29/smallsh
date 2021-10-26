#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "command.h"
#include "parse.h"
#include "exec.h"
#include "background.h"


// Returns true if the first printable character in 'text' is 'comparison'
// Also returns true if 'text' is comprised of only non printable characters (eg spaces or newlines)
// Otherwsies returns false
bool startsWithOrEmpty(char * text, char comparison) {
  int i = 0;
  while(text[i] != '\0') {
    if(text[i] == comparison) {
        // if this letter is 'comparison' value, return true
      return true;
    } else if(text[i] >= 33 && text[i] <= 126) {
       // if this letter is a printable character, return false (does not start with comp)
      return false;
    }
    // if this letter is a nonprintable character, iterate to next letter
    i++;
  }
  // if ALL letters were non printable, return true
  return true;
}


int main() {
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
    executeCommand(myCommand, &lastExitStatus, head);
    freeCommand(myCommand);
  }
}