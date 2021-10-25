#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "commandstruct.h"
#include "parsecommand.h"
#include "execcommand.h"

void executeCommand(struct command *myCommand) {
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
    // TODO ------------------------------------------------------------------
    // The status command prints out either the exit status or the terminating signal of the last foreground process ran by your shell.
    // If this command is run before any foreground command is run, then it should simply return the exit status 0.
    // The three built-in shell commands do not count as foreground processes for the purposes of this built-in command - i.e., status should ignore built-in commands.
  } else {
    executeOtherCommand(myCommand);
  }
}


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
  while(1) {
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
    executeCommand(myCommand);
    freeCommand(myCommand);
  }
}