UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L.

INC = include/
SRC = src/
BIN = bin/
PARSER_SRC_FILES = $(wildcard src/KML*.c)
PARSER_OBJ_FILES = $(patsubst src/KML%.c,bin/KML%.o,$(PARSER_SRC_FILES))

ifeq ($(UNAME), Linux)
	XML_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	XML_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif

#Generates libkmlparser shared object for linking
parser: $(BIN)libkmlparser.so

$(BIN)libkmlparser.so: $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o
	$(CC) -shared -o $(BIN)libkmlparser.so $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o -lxml2 -lm

#Compiles all files named KML*.c in src/ into object files, places all corresponding KML*.o files in bin/
$(BIN)KML%.o: $(SRC)KML%.c $(INC)LinkedListAPI.h $(INC)KML*.h
	$(CC) $(CFLAGS) -I$(XML_PATH) -I$(INC) -c -fpic $< -o $@

$(BIN)liblist.so: $(BIN)LinkedListAPI.o
	$(CC) -shared -o $(BIN)liblist.so $(BIN)LinkedListAPI.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

#This is the target for the in-class XML example
xmlExample: $(SRC)libXmlExample.c 
	$(CC) $(CFLAGS) -I$(XML_PATH) $(SRC)libXmlExample.c -lxml2 -o $(BIN)xmlExample

#End of original makefile
#--------------------------------------------------------------------------------------------------

#Kyle Lukaszek
#ID: 1113798
#Course CIS 2750 F22
#Assignment 1

#Tester Functionality

#Make libkmlparser.so, generate tester object file, generate test binary by linking .so with .o file, cleanup /bin folder
test: parser $(BIN)test.o makebin cleanup

$(BIN)test.o: $(SRC)test.c
	$(CC) $(CFLAGS) -c -fpic -I$(INC) -I$(XML_PATH) $(SRC)test.c -o $(BIN)test.o

makebin:
	$(CC) $(CFLAGS) $(LDFLAGS)/bin -o $(BIN)test $(BIN)test.o -lkmlparser
#--------------------------------------------------------------------------------------------------

#Utility Makefile Functions

#removes all compiled files from /bin
clean:
	rm -rf $(BIN)xmlExample $(BIN)*.o $(BIN)*.so

#removes only .o files 
cleanup:
	rm -rf $(BIN)*.o