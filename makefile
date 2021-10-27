CC=gcc

start: build
	@./smallsh

build: main.c
	@$(CC) --std=gnu99 -Wall -o smallsh background.c cd.c exec.c globals.c main.c parse.c sighandle.c status.c 