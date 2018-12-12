
VPATH=../src
CFLAGS=-Wall -O3 -g

OBJECTS= \
  bmp_read.o \
  bmp_write.o \
  color_to_bw.o \
  brightness.o \
  brightness_sse.o \
  brightness_avx2.o \
  pic_info.o \
  yuv422.o \
  yuv422_sse.o

default: $(OBJECTS)
	$(CC) -o ../image_proc ../src/image_proc.c $(CFLAGS) \
	  $(OBJECTS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.asm
	nasm -f elf64 -o $*.o $<

