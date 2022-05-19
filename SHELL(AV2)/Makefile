CC=gcc
CFLAGS=-c -g -Wall -std=c99
LDFLAGS=-pthread

SOURCES=$(wildcard *.c)
LIBRARIES=my_shell.o nivel7.o
INCLUDES=my_shell.h
LINKS=-lreadline
PROGRAMS= $(patsubst %.c, %, $(wildcard *.c))
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

#$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

nivel7: nivel7.o $(LIBRARIES) $(INCLUDES) $(LINKS)
	$(CC) $(LDFLAGS) $< -o $@ $(LINKS)

my_shell: my_shell.o $(LIBRARIES) $(INCLUDES) $(LINKS)
	$(CC) $(LDFLAGS) $< -o $@ $(LINKS)

%.o: %.c $(INCLUDES) $(LINKS)
	$(CC) $(CFLAGS) -o $@ -c $< $(LINKS)

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)