CC=gcc

start: build
	@./smallsh

build: main.c
	@$(CC) --std=gnu99 -Wall -o smallsh main.c parsecommand.c execcommand.c