CC=gcc
CFLAGS=-g -w -x c -std=c99
LDFLAGS=-Iinclude -Llib -lmingw32 -lSDLmain -lSDL
SOURCES=emulation.c main.c
OBJECTS=emulation.o main.o
EXECUTABLE=main

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

$(OBJECTS): $(SOURCES) emulation.h
	$(CC) $(CFLAGS) -c $(SOURCES) $(LDFLAGS)


