#ifndef PARSE_H
#define PARSE_H 

bool startsWithOrEmpty(char * text, char comparison);

char *getUserCommand();

void freeUserCommand(char *commandText);

struct command *createCommand(char *commandText);

void freeCommand(struct command *myCommand);

void printCommand(struct command *myCommand);

#endif 