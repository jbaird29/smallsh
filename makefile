CC=gcc

start: build
	@./smallsh

build: main.c
	@$(CC) --std=gnu99 -Wall -o smallsh main.c parse.c exec.c background.c