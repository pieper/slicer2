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
GLUT_LIBS = 
X_LIBS =  -L/usr/lib64
prefix = /usr/local
includedir = $(DESTDIR)/${prefix}/include

include ../make.inc
include ../config

PACKAGE = image
TOPLEVEL = ..
CFILES = pixel.c image_defs.c \
	 image_gl_uchar.c image_gl_short.c image_gl_ushort.c \
	 image_gl_int.c image_gl_uint.c \
	 image_gl_float.c image_gl_double.c \
	 image_bit.c image_pointer.c \
	 image_gla_uchar.c image_gla_short.c image_gla_ushort.c \
	 image_gla_int.c image_gla_uint.c \
	 image_gla_float.c image_gla_double.c \
	 image_rgb_uchar.c image_rgb_short.c image_rgb_ushort.c \
	 image_rgb_int.c image_rgb_uint.c \
	 image_rgb_float.c image_rgb_double.c \
	 image_rgba_uchar.c image_rgba_short.c image_rgba_ushort.c \
	 image_rgba_int.c image_rgba_uint.c \
	 image_rgba_float.c image_rgba_double.c \
	 image_bgr_uchar.c image_bgr_short.c image_bgr_ushort.c \
	 image_bgr_int.c image_bgr_uint.c \
	 image_bgr_float.c image_bgr_double.c \
	 image_bgra_uchar.c image_bgra_short.c image_bgra_ushort.c \
	 image_bgra_int.c image_bgra_uint.c \
	 image_bgra_float.c image_bgra_double.c \
	 image_vfield2D_float.c image_vfield2D_double.c \
	 image_vfield2D_short.c image_vfield2D_int.c \
	 image_vfield3D_float.c image_vfield3D_double.c \
	 image_vfield3D_short.c image_vfield3D_int.c \
         image_rgbx_uint8.c image_rgbx_uint10.c image_rgbas_uint10.c image_rgba_uint12.c \
	 image_yuvx444_uint8.c image_yuva444_uint8.c image_yuv422_uint8.c \
	 image_gl_uint10.c image_gl_uint12.c \
         image_extract.c image_convert.c image_channel.c openGL_texture.c image_display.c image_pyramid.c \
	 image_invert.c image_compare.c image_composite.c image_flip.c \
	 image_scale.c
OBJS = $(patsubst %.c,%$(OBJECT_SUFFIX).lo,$(CFILES))
HFILES = $(patsubst %.c,%.h,$(CFILES)) io.h \
	 image_gl_uint8.h   image_gl_uint16.h   image_gl_uint32.h   image_gl_float32.h   image_gl_float64.h   image_gl_int32.h \
	 image_gla_uint8.h  image_gla_uint16.h  image_gla_uint32.h  image_gla_float32.h  image_gla_float64.h  image_gla_int32.h \
	 image_rgb_uint8.h  image_rgb_uint16.h  image_rgb_uint32.h  image_rgb_float32.h  image_rgb_float64.h  image_rgb_int32.h \
	 image_rgba_uint8.h image_rgba_uint16.h image_rgba_uint32.h image_rgba_float32.h image_rgba_float64.h image_rgba_int32.h \
	 image_bgr_uint8.h  image_bgr_uint16.h  image_bgr_uint32.h  image_bgr_float32.h  image_bgr_float64.h  image_bgr_int32.h \
	 image_bgra_uint8.h image_bgra_uint16.h image_bgra_uint32.h image_bgra_float32.h image_bgra_float64.h image_bgra_int32.h \
	 image_vfield2D_int16.h image_vfield2D_int32.h image_vfield2D_float32.h image_vfield2D_float64.h \
	 image_vfield3D_int16.h image_vfield3D_int32.h image_vfield3D_float32.h image_vfield3D_float64.h

# header files not to be included by themselves
EXTRA_HFILES = image_grey_noc.h image_colour_noc.h


targets: $(OBJS)
	(cd io; $(MAKE))

all: test bitmap-test
	(cd io; $(MAKE) all)

install: installh
	(cd io; $(MAKE) install)

$(CFILES): ../$(PACKAGE).h

test:
	$(LIBTOOL) --mode=link $(CC) -I$(TOPLEVEL)/.. $(CFLAGS) -DIMAGE_TEST_MAIN image_test.c $(PATH_BUILDER_C) -o image_test $(LDFLAGS) $(LIB) $(LIBS)

bitmap-test:
	$(LIBTOOL) --mode=link $(CC) -I$(TOPLEVEL)/.. $(CFLAGS) -DBITMAP_TEST_MAIN bitmap_test.c $(PATH_BUILDER_C) -o bitmap_test $(LDFLAGS) $(LIB) $(LIBS)

image-display-test:
	$(LIBTOOL) --mode=link $(CC) -I$(TOPLEVEL)/.. $(CFLAGS) -DIMAGE_DISPLAY_TEST_MAIN image_display_test.c $(PATH_BUILDER_C) -o image_display_test $(LDFLAGS) $(LIB) $(LIBS)

include ../make.targets


GARBAGE += image_test bitmap_test
clean::
	(cd io; $(MAKE) clean)

docsource::
	(cd io; $(MAKE) docsubsource)

docfinish::
	(cd io; $(MAKE) docsubfinish)
