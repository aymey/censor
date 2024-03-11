CC = gcc
CFLAGS = -Wall -Wextra -ggdb

all: src/payload.s censor

censor: src/main.c src/elf.*
	$(CC) $(CFLAGS) src/main.c src/elf.* -o censor

src/payload.s: src/payload.c
	$(CC) -S src/payload.c -o src/payload.s


.PHONY: all
