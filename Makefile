CC	= g++
LDFLAGS	= -L../gamelib
LIBS	= -lgame -lvga -lvgagl

EXE	= gjid
OBJS	= gjid.o datafile.o level.o
SRCS	= gjid.cc datafile.cc level.cc

BINDIR	= /usr/games
LIBDIR	= /usr/games/lib

CFLAGS	= -Wall -O2 -m486 -I../include -DGL_GR -DLIBDIR=\"$(LIBDIR)\"

#######################################################

all:	$(EXE) printnum

$(EXE):	$(OBJS)
	rm -f $(EXE)
	$(CC) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)
	strip $(EXE)

printnum:	printnum.o
	gcc -o printnum printnum.o
	strip printnum

suid:
	chown root $(EXE)
	chmod 4711 $(EXE)

.cc.o:
	$(CC) $(CFLAGS) -c $<
.c.o:
	gcc $(CFLAGS) -c $<

install:
	install -o root -m 4711 $(EXE) $(BINDIR)
	install -m 644 gjid.dat $(LIBDIR)

clean:
	rm -f $(EXE) $(OBJS)

dataclean:
	rm -f gjid.size gjid.dat printnum printnum.o
	rm -f data/robot_?.pix oneway_?.pix

depend:
	$(CC) $(CFLAGS) -c -MM $(SRCS) > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

