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


// allocates memory for a string and copies the target string into that memory space
void allocateAndCopyString(char **ptrToStringToAllocate, char *stringtoCopy) {
  *ptrToStringToAllocate = calloc(strlen(stringtoCopy) + 1, sizeof(char));
  strcpy(*ptrToStringToAllocate, stringtoCopy);
}


void addInputOrOutputFile(struct command *myCommand, char *file, char operator) {
  if(operator == '<') {
    allocateAndCopyString(&myCommand->inputFile, file);
  } else {
    allocateAndCopyString(&myCommand->outputFile, file);
  }
}

// returns True
bool isAnArgument(char *string) {
  // TODO - "If the & character appears anywhere else, just treat it as normal text"
  // could & be an argument to the program?
  // if so, then I will have additionally to ensure that the & here is the final character in the string
  // something like: strcmp(string, "&") == 0 && isLastCharacter(string)
  if(strcmp(string, ">") == 0 || strcmp(string, "<") == 0 || strcmp(string, "&") == 0) {
    return false;
  } else {
    return true;
  }
}


// given a line of text, creates and returns a pointer to a command struct
struct command *createCommand(char *commandText) {
  // syntax:  command [arg1 arg2 ...] [< input_file] [> output_file] [&]
  struct command *myCommand = malloc(sizeof(struct command));
  char *saveptr;
  char *token;

  // first word is the program
  token = strtok_r(commandText, " ", &saveptr);
  allocateAndCopyString(&myCommand->program, token);

  // next words are the arguments; continue until < > or & reached
  for (int i=0; i<MAX_ARGUMENTS; i++) myCommand->arguments[i] = NULL;
  int i = 0;
  token = strtok_r(NULL, " ", &saveptr);
  while(token && isAnArgument(token)) {
    allocateAndCopyString(&myCommand->arguments[i], token);
    token = strtok_r(NULL, " ", &saveptr);
    i++;
  }

  // parse the < or > (two times) if they are present
  for(int i = 0; i < 2; i++) {
    if(token && (token[0] == '<' || token[0] == '>')) {
      char operator = token[0];  // get the operator (either > or <)
      token = strtok_r(NULL, " ", &saveptr);  // get the parameter (either inputFile or outputFile)
      addInputOrOutputFile(myCommand, token, operator);
      token = strtok_r(NULL, " ", &saveptr);
    }
  }

  // parse the & if it is present
  myCommand->isBackground = false;
  if(token && token[0] == '&') {
    myCommand->isBackground = true;
  }

  return myCommand;
}


// given a pointer to a command struct, frees the memory allocated to it
void freeCommand(struct command *myCommand) {
  free(myCommand->program);
  for(int i = 0; i<MAX_ARGUMENTS; i++) {
    if(myCommand->arguments[i]) free(myCommand->arguments[i]);
  }
  if(myCommand->inputFile) free(myCommand->inputFile);
  if(myCommand->outputFile) free(myCommand->outputFile);
  free(myCommand);
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
      freeCommand(myCommand);
    }
  }
}