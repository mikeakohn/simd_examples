
VPATH=../src

OBJECTS= \
  brightness.o

default: $(OBJECTS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

