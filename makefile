CC=gcc
CFLAGS=-g -ggdb -w -x c -std=c99
LDFLAGS=-Iinclude -Llib -lmingw32 -lSDLmain -lSDL
SOURCES=emulation.c main.c
OBJECTS=emulation.o main.o
EXECUTABLE=main

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

$(OBJECTS): $(SOURCES) emulation.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SOURCES)


