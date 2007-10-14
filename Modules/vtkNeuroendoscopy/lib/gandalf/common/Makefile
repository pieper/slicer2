CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lX11 -lm -lX11 -lm 
LDFLAGS =  -L/usr/local/lib  -L/usr/lib64
INSTALL = /usr/bin/install -c
PNG_LIBS = -lpng -lz
TIFF_LIB = -ltiff
JPEG_LIB = -ljpeg
GIF_LIB = @GIF_LIB@
LAPACK_LIBS = 
OPENGL_LIBS = -lGLU -lGL -lXmu -lXi -lX11 -lpthread
X_LIBS =  -L/usr/lib64
prefix = /usr/local
includedir = $(DESTDIR)/${prefix}/include

include ../make.inc
include ../config

PACKAGE = common
OBJS = $(patsubst %.c,%$(OBJECT_SUFFIX).lo,$(CFILES))
CFILES = misc_defs.c compare.c numerics.c allocate.c misc_error.c \
	 linked_list.c array.c bit_array.c gan_err.c gan_err_trace.c \
	 endian_io.c memory_stack.c file_io.c
HFILES = $(patsubst %.c,%.h,$(CFILES)) complex.h
TOPLEVEL = ..

# header files not to be included by themselves
EXTRA_HFILES = comp_noc.h

targets: $(OBJS)

all: test list-test

test:
	$(LIBTOOL) --mode=link $(CC) -I$(TOPLEVEL)/.. $(CFLAGS) -DCOMMON_TEST_MAIN common_test.c $(PATH_BUILDER_C) -o common_test $(LDFLAGS) $(LIB) $(LIBS)

list-test:
	$(LIBTOOL) --mode=link $(CC) -I$(TOPLEVEL)/.. $(CFLAGS) -DLIST_TEST_MAIN list_test.c $(PATH_BUILDER_C) -o list_test $(LDFLAGS) $(LIB) $(LIBS)

install: installh

$(CFILES): ../$(PACKAGE).h

include ../make.targets

GARBAGE += common_test list_test
