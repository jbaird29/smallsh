#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char input[2049];  // max length of 2048 characters
  while(1) {
    putchar(':');
    putchar(' ');
    fgets(input, 2049, stdin);
    // Below line copied from: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
    // Date: 10/22/2021
    // How to remove newline character after fgets() 
    input[strcspn(input, "\n")] = 0;
    if(strcmp(input, "exit") == 0) {
      // if command was "exit" then stop the program
      return EXIT_SUCCESS;
    } else {
      // TBU - pass this into command parsing function
      printf("%s\n", input);
    }
  }
}