#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "structs.h"
#include "globals.h"

#define MAX_COMMAND_LEN 2048

/* ------------------------ HELPER FUNCTIONS ------------------------ */

// given a string, returns a pointer to a new string with the $$ pid variable expanded (if any)
static char *copyAndExpandPidVar(char *oldStr) {
  pid_t pid = getpid();  // get the pid, as a number
  // Below line of code copied from: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
  // How to calcualte the number of digits in an integer; Date: 10/25/2021
  int pidStrLen = (int)ceil(log10(pid));  // figure out how many digits are in the pid
  char pidStr[pidStrLen+1];  // create a buffer to hold the string representation of the pid
  sprintf(pidStr, "%d", pid);  // put the string representation of pid into the buffer
  // count the # of $$'s in the oldStr, to figure out the necessary size of the newStr buffer
  int pidVarCount = 0;  // holds the number of instances of a "$$" in the string
  char *curr = strstr(oldStr, "$$");  // search for the first instance of a $$ in oldStr
  while(curr) {  // if one was found
    pidVarCount++;  // increment the counter
    curr = strstr(curr+2, "$$");  // move two characters forward and re-search for a $$
  }
  int newStrLen = strlen(oldStr) - (2 * pidVarCount) + (pidStrLen * pidVarCount);  // self-explanatory
  char *newStr = calloc(newStrLen + 1, sizeof(char));  // allocate memory space for the new string
  // now, copy the letters from oldStr to newStr, but replacing any $$'s
  char *prev = oldStr;  // for purposes of incrementing; used to hold the previous spot in oldStr
  curr = strstr(prev, "$$");  // search for the first instance of a $$ in oldStr
  while(curr) {
    strncat(newStr, prev, (curr-prev)*sizeof(char));  // copy all the characters up until the $$
    strcat(newStr, pidStr);  // append the PID string
    prev = curr+2;  // move two characters forward
    curr = strstr(prev, "$$");  // re-search for another $$
  }
  strcat(newStr, prev);  // append the remaining portion of oldStr
  return newStr;
}


// given an opereator (either > or <) and filename argument, adds that filename to the command's inputFile or outputFile
static void addInputOrOutputFile(struct command *myCommand, char *file, char operator) {
  if(operator == '<') {
    myCommand->inputFile = copyAndExpandPidVar(file);
  } else {
    myCommand->outputFile = copyAndExpandPidVar(file);
  }
}


// allocates memory for a command struct, initializes its values, and returns a pointer to it
static struct command *initializeCommand() {
  struct command *myCommand = malloc(sizeof(struct command));
  // initialize the command struct default values
  myCommand->program = NULL;
  for (int i = 0; i<MAX_ARGUMENTS; i++) myCommand->arguments[i] = NULL;
  myCommand->argCount = 0;
  myCommand->inputFile = NULL;
  myCommand->outputFile = NULL;
  myCommand->isBackground = false;
  return myCommand;
}


// if the last printable char (excl spaces) in text is 'comparison', returns the index of that char; else returns -1
int endsWith(char *text, int len, char comparison) {
  for(int i = len-1; i >= 0; i--) {
    if(text[i] == comparison) return i;  // if this letter is 'comparison' value, return true
    if(text[i] >= 33 && text[i] <= 126) return -1;  // if this letter is a printable character, return false (does not start with comp)
    // otherwise, this letter is a nonprintable character such as a space; iterate to next letter
  }
  return -1;  // if ALL letters were non-printable, return false (it was an empty line)
}





/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

// Returns true if the first printable character in 'text' is 'comparison' (comparision will be '#' for comments)
// Also returns true if 'text' is comprised of only non printable characters (eg spaces or newlines)
// Otherwsies returns false
bool startsWithOrEmpty(char * text, char comparison) {
  int i = 0;
  while(text[i] != '\0') {
    if(text[i] == comparison) return true;  // if this letter is 'comparison' value (will be # for comments), return true
    if(text[i] >= 33 && text[i] <= 126) return false;  // if this letter is a printable character, return false (does not start with comp)
    i++;  // otherwise, this letter is a nonprintable character such as a space; iterate to next letter
  }
  return true;  // if ALL letters were non-printable, return true (it was an empty line)
}


// gets a command from the user and returns a pointer to the string
char *getUserCommand() {
  char *commandText = calloc(MAX_COMMAND_LEN + 1, sizeof(char));  // allocate memory space for the string
  putchar(':');
  putchar(' ');
  fflush(stdout);
  fgets(commandText, MAX_COMMAND_LEN+1, stdin);
  // Below line copied from: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
  // Purpose: How to remove newline character after fgets()   Date: 10/22/2021
  commandText[strcspn(commandText, "\n")] = 0;
  return commandText;
}


// given a user command, frees the memory allocated to it
void freeUserCommand(char *commandText) {
  free(commandText);
}


// given a line of text, creates and returns a pointer to a command struct
// syntax:  command [arg1 arg2 ...] [< input_file] [> output_file] [&]
struct command *createCommand(char *commandText) {
  struct command *myCommand = initializeCommand();  // allocates a command struct and initializes values
  // first parse the & if it is present, and remove it from the string
  int ind = endsWith(commandText, strlen(commandText), '&');  // if text ends with &, returns the index of that &; else returns -1
  if(ind != -1) {  // if the last printable character (excluding spaces) is an '&'
    if(!fgOnlyMode) myCommand->isBackground = true;  // set isBackground to true ONLY IF fgOnlyMode is not active
    commandText[ind] = '\0';  // remove the ampersand from the string; it has been validly parsed
  }
  // next tokenize the string from start to end
  char *saveptr;
  char *token;
  // first word is the program
  token = strtok_r(commandText, " ", &saveptr);
  myCommand->program = copyAndExpandPidVar(token);
  // next words are the arguments
  token = strtok_r(NULL, " ", &saveptr);
  while(token && strcmp(token, ">") != 0 && strcmp(token, "<") != 0) {  // continue tokenizing until and < or > is reached
    myCommand->arguments[myCommand->argCount] = copyAndExpandPidVar(token);  // save the argument into the struct
    myCommand->argCount++;  // increment the count of arguments
    token = strtok_r(NULL, " ", &saveptr);  // get the next token
  }
  // finally parse the < or > (two times); if they are not present, nothing happens
  for(int i = 0; i < 2; i++) {
    if(token && (strcmp(token, ">") == 0 || strcmp(token, "<") == 0)) {
      char operator = token[0];  // get the operator (either > or <)
      token = strtok_r(NULL, " ", &saveptr);  // get the parameter (either inputFile or outputFile)
      addInputOrOutputFile(myCommand, token, operator);  // add it to the struct
      token = strtok_r(NULL, " ", &saveptr);  // get the next token
    }
  }
  return myCommand;
}


// given a pointer to a command struct, frees the memory allocated to it
void freeCommand(struct command *myCommand) {
  free(myCommand->program);
  for(int i = 0; i<myCommand->argCount; i++) free(myCommand->arguments[i]);
  if(myCommand->inputFile) free(myCommand->inputFile);
  if(myCommand->outputFile) free(myCommand->outputFile);
  free(myCommand);
}


// For Debugging: given a commands, prints it to stdout
void printCommand(struct command *myCommand) {
  printf("Program: %s\n", myCommand->program);
  for(int i = 0; i<myCommand->argCount; i++) printf("Argument %d: %s\n", i, myCommand->arguments[i]);
  printf("Input: %s\n", myCommand->inputFile);
  printf("Output: %s\n", myCommand->outputFile);
  printf("Is Background: %s\n", myCommand->isBackground ? "true" : "false");
  fflush(stdout);
}