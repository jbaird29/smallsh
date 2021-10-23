CC=gcc

build: main.c
	@$(CC) --std=gnu99 -Wall -o smallsh main.c