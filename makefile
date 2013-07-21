CC=gcc
CFLAGS=-g -w -x c -std=c99
LDFLAGS=-Iinclude -Llib -lmingw32 -lSDLmain -lSDL
SOURCES=emulation.c main.c chip_opcodes.c schip_opcodes.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

$(OBJECTS): $(SOURCES) emulation.h
	$(CC) $(CFLAGS) -c $(SOURCES) $(LDFLAGS)

clean:
	rm *.o main.exe
