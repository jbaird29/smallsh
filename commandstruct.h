#ifndef COMMANDSTRUCT_H
#define COMMANDSTRUCT_H 

#define MAX_ARGUMENTS 512

// structured representation of a user-inputted command
struct command {
  char *program;
  char *arguments[MAX_ARGUMENTS]; // maximum of 512 arguments
  int argCount;
  char *inputFile;
  char *outputFile;
  bool isBackground;
};

#endif