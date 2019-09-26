PROJS=lab1
CC=gcc
CFLAGS=-m64
all: $(PROJS)
	@echo Done!
$(PROJS): lab1.c
	@$(CC) $(CFLAGS) -o $@ $(@:=.c)
