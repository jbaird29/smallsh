#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ARGUMENTS 512

struct command {
  char *program;
  char *arguments[MAX_ARGUMENTS]; // maximum of 512 arguments
  char *inputFile;
  char *outputFile;
  bool isBackground;
};


struct command *createCommand(char *commandText) {
  // command [arg1 arg2 ...] [< input_file] [> output_file] [&]
  struct command *myCommand = malloc(sizeof(struct command));
  char *saveptr;
  char *token;

  // first word is the program
  token = strtok_r(commandText, " ", &saveptr);
  myCommand->program = calloc(strlen(token) + 1, sizeof(char));
  strcpy(myCommand->program, token);

  // next words are the arguments; continue until < > or & reached
  for (int i=0; i<MAX_ARGUMENTS; i++) myCommand->arguments[i] = NULL;
  int i = 0;
  token = strtok_r(NULL, " ", &saveptr);
  while(token && token[0] != '<' && token[0] != '>' && token[0] != '&') {
    myCommand->arguments[i] = calloc(strlen(token) + 1, sizeof(char));
    strcpy(myCommand->arguments[i], token);
    token = strtok_r(NULL, " ", &saveptr);
    i++;
  }

  // parse the <
  if(token && token[0] == '<') {
    token = strtok_r(NULL, " ", &saveptr);  // get the parameter (inputFile)
    myCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
    strcpy(myCommand->inputFile, token);
    token = strtok_r(NULL, " ", &saveptr);
  }

  // parse the >
  if(token && token[0] == '>') {
    token = strtok_r(NULL, " ", &saveptr);  // get the parameter (outputFile)
    myCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
    strcpy(myCommand->outputFile, token);
    token = strtok_r(NULL, " ", &saveptr);
  }

  // parse the &
  myCommand->isBackground = false;
  if(token && token[0] == '&') {
    myCommand->isBackground = true;
  }

  return myCommand;
}

void printCommand(struct command *myCommand) {
  printf("Program: %s\n", myCommand->program);
  int i = 0;
  while(myCommand->arguments[i] != NULL) {
    printf("Argument %d: %s\n", i, myCommand->arguments[i]);
    i++;
  }
  printf("Input: %s\n", myCommand->inputFile);
  printf("Output: %s\n", myCommand->outputFile);
  printf("Is Background: %s\n", myCommand->isBackground ? "true" : "false");
  fflush(stdout);
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
    // Date: 10/22/2021
    // How to remove newline character after fgets() 
    commandText[strcspn(commandText, "\n")] = 0;
    if(strcmp(commandText, "exit") == 0) {
      // if command was "exit" then stop the program
      return EXIT_SUCCESS;
    } else if(!startsWithOrEmpty(commandText, '#')) { 
      // if command starts with '#' or is empty, do nothing
      struct command *myCommand = createCommand(commandText);
      printCommand(myCommand);
    }
  }
}