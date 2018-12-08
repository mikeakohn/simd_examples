
VPATH=../src

OBJECTS= \
  bmp_read.o \
  bmp_write.o \
  brightness.o

default: $(OBJECTS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

