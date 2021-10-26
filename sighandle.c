#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void registerHandler(int signal, void (*handler_func)(int)) {
  struct sigaction SIGINT_action = {0};
  SIGINT_action.sa_handler = handler_func;  // assign the signal handler function
  sigfillset(&SIGINT_action.sa_mask);  // block all other signals
  SIGINT_action.sa_flags = 0;  // No flags set
  sigaction(signal, &SIGINT_action, NULL);  // register the handler
}