EXTN_HOME = .
EXTN_OBJS1 = $(EXTN_HOME)/mror_image_buffer.o $(EXTN_HOME)/locator_client.o
EXTN_OBJS2 = $(EXTN_HOME)/locator_common.o $(EXTN_HOME)/libxform.a

LIBS   = -lsocket -lnsl -lxview -lolgx -lX11 -L/usr/openwin/lib -lm
IFLAGS = -I$(EXTN_HOME) -I/usr/openwin/include
CFLAGS = -g -Wall -O2 $(IFLAGS)

all: spl_server

spl_server: spl_server.o $(EXTN_OBJS1) $(EXTN_OBJS2) Makefile
	gcc -o spl_server spl_server.o ${CFLAGS} $(OBJS) $(EXTN_OBJS1) $(EXTN_OBJS2) $(LIBS)

spl_server.o: spl_server.c $(EXTN_HOME)/mror_imagebuf.h $(EXTN_HOME)/mror.h
	gcc ${CFLAGS} -c spl_server.c

clean:
	rm spl_server.o
