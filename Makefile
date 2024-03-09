CC = gcc
CFLAGS = -Wall -Wextra -ggdb

all: censor

censor: src/*
	$(CC) $(CFLAGS) src/* -o censor

.PHONY: all censor
