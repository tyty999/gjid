include Common.mk

EXE	= gjid
OBJS	= gjid.o datafile.o level.o
SRCS	= gjid.cc datafile.cc level.cc mkdata.cc

CFLAGS	+= -I../gamelib
CXXFLAGS+= -I../gamelib
LDFLAGS += -L../gamelib
LIBS	+= -lgame -lfbgl -lustl

all:	${EXE} gjid.dat

include Comrubin.mk

mkdata:	mkdata.o level.o
	@${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

gjid.dat:	mkdata data/* gjid.story gjid.levels
	@echo "Creating the data file ... "
	@./mkdata
