#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>



void registerIgnoreSIGINT() {
  struct sigaction SIGINT_action = {0};
  SIGINT_action.sa_handler = SIG_IGN;  // Register "ignore" as the signal handler
  sigfillset(&SIGINT_action.sa_mask);  // block all other signals
  SIGINT_action.sa_flags = 0;  // No flags set
  sigaction(SIGINT, &SIGINT_action, NULL);  // register the handler
}


void registerDefaultSIGINT() {
  struct sigaction SIGINT_action = {0};
  SIGINT_action.sa_handler = SIG_DFL;  // Register "default" as the signal handler (i.e. terminate itself)
  sigfillset(&SIGINT_action.sa_mask);  // block all other signals
  SIGINT_action.sa_flags = 0;  // No flags set
  sigaction(SIGINT, &SIGINT_action, NULL);  // register the handler
}