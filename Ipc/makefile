# Sources:
SRCS:=main.c ram.c processore2.c
OBJS:=$(SRCS:.c=.o)


# Config:
CC:=gcc
CFLAGS:= -c
LD:=gcc
LDFLAGS= -pthread

# Targets:

all: main

clean:
	@echo Cleaning.
	@rm -f *.o
	@rm -f main

main: $(OBJS)
	@echo $@
	@$(LD) $(LDFLAGS) -o $@ $^


ram.o: ram.c ram.h utility.h
	@echo $@
	@ $(CC) $(CFLAGS) -o $@ $<

processore2.o: processore2.c processore2.h utility.h
	@echo $@
	@ $(CC) $(CFLAGS) -o $@ $<

main.o: main.c utility.h processore2.h ram.h
	@echo $@
	@ $(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean

