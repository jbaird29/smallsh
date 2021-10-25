#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "parsecommand.h"

/* ------------------------ HELPER FUNCTIONS ------------------------ */

// given a string, returns a pointer to a new string with the $$ pid variable expanded (if any)
static char *copyAndExpandPidVar(char *oldStr) {
  // oldStr    = foo$$$$bar$$
  // varMarkers   = 000111100011  -> iterate through string and mark 1 where there are double $$
  // pidVarCount  = 3 --> the number of double variables
  // newStringLen = strlen(stringToCopy) - 2*pidVarCount + strlen(pid)*pidVarCount + 1 = 12 - 2*3 + 6*3
  // iterate through string again
  //     if the varMarker is 0 ->  add that character to new buffer; i = i+1
  //     if the varMarker is 1 ->  concat the pid onto new buffer; i = i+2
  pid_t pid = getpid();  // get the pid, as a number
  // Below line of code copied from: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
  // How to calcualte the number of digits in an integer; Date: 10/25/2021
  int pidStrLen = (int)ceil(log10(pid));  // figure out how many digits are in the pid
  char pidStr[pidStrLen+1];  // create a buffer to hold the string representation of the pid
  sprintf(pidStr, "%d", pid);  // print the string representation of pid into the buffer
  int oldStrLen = strlen(oldStr);   // length of the old string 
  bool variableMarkers[oldStrLen];  // array value holds a true if that char was part of a "$$", else false
  variableMarkers[0] = 0;
  int pidVarCount = 0;  // holds the number of instances of a "$$" in the string
  int i = 1;
  while(i < oldStrLen) {
    if(oldStr[i] == '$' && oldStr[i-1] == '$') {  // if a "$$" instance was discovered
      variableMarkers[i] = true;  // mark these characters as part of that $$
      variableMarkers[i-1] = true;
      pidVarCount++;  // increment the number of discovered $$'s
      i = i + 2;
    } else {
      variableMarkers[i] = false;
      i = i + 1;
    }
  }
  int newStrLen = oldStrLen - (2 * pidVarCount) + (pidStrLen * pidVarCount);  // self-explanatory
  char *newStr = calloc(newStrLen + 1, sizeof(char));  // allocate memory space for the new string
  int oldStr_i = 0, newStr_i = 0;  // variables to incrementer through old string and new strnig
  while(oldStr_i < oldStrLen) {
    if(!variableMarkers[oldStr_i]) {  // if this spot in the string was NOT part of a "$$"
      newStr[newStr_i] = oldStr[oldStr_i];  // copy the letter from old string to new string
      oldStr_i = oldStr_i + 1;  // increment each pointer by 1
      newStr_i = newStr_i + 1;  // increment each pointer by 1
    } else {  // if this spot in the string WAS part of a "$$"
      strcat(newStr, pidStr);  // replace the $$ with the pid
      oldStr_i = oldStr_i + 2; // skip the next entry (which was already counted as a part of this $$)
      newStr_i = newStr_i + pidStrLen;  // increment by the number of digits in newStr
    }
  }
  return newStr;
}


// static int TEST_copyAndExpandPidVar() {
//   char oldstring[] = "$$f$oo$$$o$bar$$";
//   char *newString = copyAndExpandPidVar(oldstring);
//   printf("Old: %s\n", oldstring);
//   printf("New: %s\n", newString);
//   return strcmp(newString, "29325f$oo29325$o$bar29325") == 0 ? 1 : 0;
// }


// given an opereator (either > or <) and filename argument, adds that filename to the command's inputFile or outputFile
static void addInputOrOutputFile(struct command *myCommand, char *file, char operator) {
  if(operator == '<') {
    myCommand->inputFile = copyAndExpandPidVar(file);
  } else {
    myCommand->outputFile = copyAndExpandPidVar(file);
  }
}


// returns false the end of the argument list has been reached (ie the word in the command text is a > or < or &)
static bool isAnArgument(char *string) {
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




/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

// given a line of text, creates and returns a pointer to a command struct
struct command *createCommand(char *commandText) {
  // syntax:  command [arg1 arg2 ...] [< input_file] [> output_file] [&]
  struct command *myCommand = malloc(sizeof(struct command));
  // initialize the command struct default values
  myCommand->program = NULL;
  for (int i = 0; i<MAX_ARGUMENTS; i++) myCommand->arguments[i] = NULL;
  myCommand->argCount = 0;
  myCommand->inputFile = NULL;
  myCommand->outputFile = NULL;
  myCommand->isBackground = false;
  // initialize variables for strtok
  char *saveptr;
  char *token;

  // first word is the program
  token = strtok_r(commandText, " ", &saveptr);
  myCommand->program = copyAndExpandPidVar(token);

  // next words are the arguments; continue until < > or & reached
  token = strtok_r(NULL, " ", &saveptr);
  while(token && isAnArgument(token)) {
    myCommand->arguments[myCommand->argCount] = copyAndExpandPidVar(token);
    token = strtok_r(NULL, " ", &saveptr);
    myCommand->argCount++;
  }

  // parse the < or > (two times) if they are present
  for(int i = 0; i < 2; i++) {
    if(token && (strcmp(token, ">") == 0 || strcmp(token, "<") == 0)) {
      char operator = token[0];  // get the operator (either > or <)
      token = strtok_r(NULL, " ", &saveptr);  // get the parameter (either inputFile or outputFile)
      addInputOrOutputFile(myCommand, token, operator);
      token = strtok_r(NULL, " ", &saveptr);
    }
  }

  // parse the & if it is present
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


// For Debugging: given a commands, prints it to stdout
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