#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "globals.h"
#include "structs.h"


// given a signal and a handler function, registers that signal handler
void registerHandler(int signal, void (*handler_func)(int)) {
  struct sigaction SIGINT_action = {0};
  SIGINT_action.sa_handler = handler_func;  // assign the signal handler function
  sigfillset(&SIGINT_action.sa_mask);  // block all other signals
  SIGINT_action.sa_flags = SA_RESTART;  // restart any system calls after interrupt
  sigaction(signal, &SIGINT_action, NULL);  // register the handler
}


// signal handler - when user sends TSTP signal, the shell toggles on and off between foreground only mode
void handle_fgModeToggleSignal(int signo){
  fgOnlyMode = !fgOnlyMode;  // toggle the global state variable
  int result = waitpid(lastFpProcess.childPid, &lastFpProcess.wstatus, 0);  // if a fg process is still running, wait for it to complete
  if(result == -1) putchar('\n');  // if result == -1, then the shell was awaiting user input on the command line; so print a newline
  char* message = fgOnlyMode ? "Entering foreground-only mode (& is now ignored)\n" : "Exiting foreground-only mode\n";
  int messageSize = fgOnlyMode ? 50 : 30;
  write(STDOUT_FILENO, message, messageSize);
  if(result == -1) {  // if result == -1, then the shell was awaiting user input on the command line; so print a new prompt symbol
    putchar(':');
    putchar(' ');
  }
  fflush(stdout);
}