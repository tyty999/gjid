CC 	= g++
AR 	= ar
RANLIB	= ranlib
CFLAGS 	= -Wall -W -Werror -g -DFB_GR -I../include

LIBDIR	= $(HOME)/lib
INCDIR	= $(HOME)/include

OBJS 	= icon.o animicon.o animap.o mob.o mouse.o font.o palette.o win.o \
	button.o label.o input.o event.o graph_fb.o
SRCS	= animap.cc animicon.cc font.cc icon.cc mob.cc mouse.cc palette.cc \
	win.cc button.cc label.cc input.cc event.cc graph_fb.cc
TARGET 	= libgame.a

###########################################################

$(TARGET):	$(OBJS)
	rm -f $(TARGET)
	$(AR) q $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)

install:	$(TARGET)
	install -m 644 $(TARGET) $(LIBDIR)
	install -d $(INCDIR)/game
	install -m 644 *.h $(INCDIR)/game

.cc.o:
	$(CC) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.bak core *~ $(TARGET) $(OBJS)

DEPEND_FILE:=$(shell if [ -f .depend ]; then echo .depend; \
		     else echo /dev/null; fi)

depend:
	$(CC) $(CFLAGS) -MM $(SRCS) > .depend

include $(DEPEND_FILE)

