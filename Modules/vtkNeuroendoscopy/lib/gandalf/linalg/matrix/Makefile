CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lX11 -lm -lX11 -lm 
LDFLAGS =  -L/usr/local/lib  -L/usr/lib64
INSTALL = /usr/bin/install -c
prefix = /usr/local
includedir = ${prefix}/include

include ../../make.inc
include ../../config

PACKAGE = linalg
SUBPACKAGE = matrix
OBJS = $(patsubst %.c,%$(OBJECT_SUFFIX).lo,$(CFILES))
CFILES = svduv.c  ldvmat.c  ldumat.c  qrbdv.c  eigval.c  eigen.c \
	 svduvf.c ldvmatf.c ldumatf.c qrbdvf.c eigvalf.c eigenf.c \
	 housev.c  qrevec.c  trnm.c  house.c  qreval.c \
	 housevf.c qrevecf.c trnmf.c housef.c qrevalf.c
HFILES =
TOPLEVEL = ../..

all: $(OBJS)

install: installsubh

$(CFILES): ../$(SUBPACKAGE).h

include ../../make.targets
