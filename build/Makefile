
VPATH=../src
CFLAGS=-Wall -O3 -g

ifeq ($(shell uname -s),Darwin)
  EXE_TYPE=macho64
  EXTRA_FLAGS=-m64
else
  EXE_TYPE=elf64
  #EXTRA_FLAGS=-fPIC
endif

OBJECTS= \
  bmp_read.o \
  bmp_write.o \
  color_to_bw.o \
  brightness.o \
  brightness_sse.o \
  brightness_avx2.o \
  pic_info.o \
  timer.o \
  yuv422.o \
  yuv422_sse.o

default: $(OBJECTS)
	$(CC) -o ../image_proc ../src/image_proc.c $(CFLAGS) \
	  $(OBJECTS) $(EXTRA_FLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I.. $(EXTRA_FLAGS)

%.o: %.asm
	nasm -f $(EXE_TYPE) -o $*.o $<

