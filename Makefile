include Common.mk

EXE	= gjid
OBJS	= gjid.o datafile.o level.o
SRCS	= gjid.cc datafile.cc level.cc

CFLAGS	+= -I ../include -I ../gamelib -DFB_GR
CXXFLAGS+= -I ../include -I ../gamelib -DFB_GR
LDFLAGS += -L../gamelib
LIBS	+= -lgame

include Comrubin.mk
