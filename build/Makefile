
VPATH=../src
CFLAGS=-Wall -O3 -g

OBJECTS= \
  bmp_read.o \
  bmp_write.o \
  color_to_bw.o \
  brightness.o \
  pic_info.o

default: $(OBJECTS)
	$(CC) -o ../image_proc ../src/image_proc.c $(CFLAGS) \
	  $(OBJECTS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

