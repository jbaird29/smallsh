#ifndef SIGHANDLE_H
#define SIGHANDLE_H

void registerHandler(int signal, void (*handler_func)(int));

void handle_fgModeToggleSignal(int signo);

#endif
