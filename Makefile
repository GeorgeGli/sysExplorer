#DEBUG=1
#PROFILE=1

DEBUGFLAGS=  -g 
OPTFLAGS= -finline -march=native -O3 -DNDEBUG 

ifeq ($(PROFILE),1)
PROFFLAGS= -g -pg 
PLFLAGS= -g -pg
else
PROFFLAGS= 
PLFLAGS=
endif

INCLUDE_PATH=-I. 

CFLAGS= -Wall -D_GNU_SOURCE -pthread 
ifeq ($(DEBUG),1)
CFLAGS+=  $(DEBUGFLAGS) $(PROFFLAGS) $(INCLUDE_PATH)
else
CFLAGS+=  $(OPTFLAGS) $(PROFFLAGS) $(INCLUDE_PATH)
endif

LDFLAGS= $(PLFLAGS)  
LIBS= -lpthread


C_PROG= sysexplorer.c tests.c
PROG=$(C_PROC:.c=)

C_SRC= linkedlist.c strutils.c hashtable.c urldecode.c
C_OBJ=$(C_SRC:.c=.o)

C_SOURCES= $(C_PROG) $(C_SRC)
C_OBJECTS=$(C_SOURCES:.c=.o)


.PHONY: all

all: sysexplorer tests

sysexplorer: sysexplorer.o $(C_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

tests: tests.o $(C_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)



realclean:
	-rm sysexplorer tests $(C_OBJECTS) .depend *~
	touch .depend

depend: $(C_SOURCES)
	$(CC) $(CFLAGS) -MM $(C_SOURCES) > .depend

clean: realclean depend

include .depend

