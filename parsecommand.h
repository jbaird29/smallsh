#ifndef PARSECOMMAND_H
#define PARSECOMMAND_H 

struct command *createCommand(char *commandText);

void freeCommand(struct command *myCommand);

void printCommand(struct command *myCommand);

#endif 