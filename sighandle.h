#ifndef SIGHANDLE_H
#define SIGHANDLE_H

void registerHandler(int signal, void (*handler_func)(int));

#endif
