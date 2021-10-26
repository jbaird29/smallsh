#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"


/* ------------------------ HELPER FUNCTIONS ------------------------ */

// given a bgProcess node, frees the memory allocated for it
static void freeBgProcess(struct bgProcess *node) {
  free(node);
}



/* ------------------------ EXTERNAL FUNCTIONS ------------------------ */

// initializes a new node in the linked list of background processes
struct bgProcess *initializeBgProcess() {
  struct bgProcess *node = malloc(sizeof(struct bgProcess));
  node->childPid = 0;
  node->next = NULL;
  return node;
}


// given the head of the list and a new childPid, inserts new node into list at the front
void bgProcessInsertFront(struct bgProcess *head, pid_t childPid) {
  struct bgProcess *node = initializeBgProcess();  // create new node in listed list
  node->childPid = childPid;
  node->next = head->next;  // insert_front() this node into the linked list
  head->next = node;
} 


// given the head of the list, iterates through nodes and reaps processes that have terminated (printing a message to stdout)
void reapBgProccesses(struct bgProcess *head) {
  struct bgProcess *prev = head;
  struct bgProcess *node = head->next;  // head is a sentinel; first node in the list will be head->next (NULL for empty list)
  int wstatus;
  while(node) {
    if(waitpid(node->childPid, &wstatus, WNOHANG)) {  // if the process has terminated (returns pid)
      printf("background pid %d is done: ", node->childPid);  // print the pid and exit status
      WIFEXITED(wstatus) ? printf("exit value %d\n", WEXITSTATUS(wstatus)) : printf("terminated by signal %d\n", WTERMSIG(wstatus));
      prev->next = node->next;  // remove this node from the list
      freeBgProcess(node);  // free the memory
      node = prev->next;  // iterate to the next node
    } else {  // if the process has not terminated (returns 0)
      prev = prev->next;  // iterate to next node in list
      node = node->next;
    }
  }
}