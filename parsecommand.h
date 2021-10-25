#define MAX_ARGUMENTS 512

struct command {
  char *program;
  char *arguments[MAX_ARGUMENTS]; // maximum of 512 arguments
  int argCount;
  char *inputFile;
  char *outputFile;
  bool isBackground;
};

struct command *createCommand(char *commandText);

void freeCommand(struct command *myCommand);

void printCommand(struct command *myCommand);