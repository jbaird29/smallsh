# smallsh

## Overview

smallsh is a simple shell implementing a subset of features of well-known shells, such as bash

- Provides a prompt for running commands
- Handles blank lines and comments
- Expands the variable $$ into PID
- Natively executes 3 built-in commands (exit, cd, and status)
- Executes other commands via fork() and exec()
- Supports input and output redirection
- Supports running commands in foreground and background processes
- Implements custom handlers for 2 signals - SIGINT and SIGTSTP

## Command Prompt

The expected syntax of a command line is: command [arg1 arg2 ...] [< input_file] [> output_file] [&]. Quoting and the pipe operator are not supported. Command lines have a maximum length of 2048 characters and a maximum of 512 arguments.

## Comments

Any line that begins with the # character is a comment line and will be ignored

## Background Commands

Any non built-in command with an & at the end will be run as a background command. The shell will print the process id of a background process when it begins. When a background process terminates, a message showing the process id and exit status will be printed. This message is printed before the prompt for a new command is displayed. Standard input and output for a background command, if not redirected, will be /dev/null.

## Signals

**SIGINT**: The shell process ignores SIGINT. A child running as background processes ignores SIGINT. A child running as a foreground process will terminate upon receiving SIGINT.

**SIGTSTP**: A child running as a foreground process ignores SIGTSTP. A child running as background process ignores SIGTSTP. The shell process will toggle between "foreground-only" mode, wherein the shell will ignore the & operator, and "normal" mode, wherein the & operator works as expected.

## Compiling & Running

Use `make start` to build and run, or use `gcc --std=gnu99 -lm -o smallsh background.c cd.c exec.c globals.c main.c parse.c sighandle.c status.c` to compile and then `./smallsh` to run
