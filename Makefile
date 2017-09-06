# BH test code

APP_NAME    = test

APP_SRC     = main.c
LIB_NAME    = $(APP_NAME).a

DEFAULT: obj app lib

CC          := gcc
AR          := ar

SRC         =$(wildcard *.c)
SRC_H       =$(wildcard *.h)

LIB_SRC     = $(filter-out $(APP_SRC), $(SRC))
CFLAGS      = -Wall
LDFLAGS     =
ARFLAGS 	= rcs
LIBS        = -lcurl


obj:
	$(CC) -c $(CFLAGS) $(SRC)

lib: obj
	$(AR) $(ARFLAGS) $(LIB_NAME) $(subst .c,.o,$(LIB_SRC))

app: lib
	$(CC) -o $(APP_NAME).exe $(subst .c,.o,$(SRC)) $(LDFLAGS) $(LIBS)
	rm -f *.o
	echo DONE

clean: 
	rm -f *.o
	rm -f *.a
	rm -f *.exe
