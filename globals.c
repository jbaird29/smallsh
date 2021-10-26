#include <stdbool.h>
#include <sys/types.h>
#include "structs.h"

// global variable to handle whether foreground-only mode is "on" or "off"
bool fgOnlyMode = false;

// global variable holding the pid and last exit status of the most recently run foreground process
struct lastProcess lastFpProcess = {0, 0};