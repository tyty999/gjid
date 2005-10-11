include Common.mk

EXE	= gjid
OBJS	= gjid.o datafile.o level.o font.o icon.o
SRCS	= gjid.cc datafile.cc level.cc font.cc icon.cc mkdata.cc
LIBS	+= -lfbgl -lustl

all:	${EXE} gjid.dat

include Comrubin.mk

mkdata:	mkdata.o level.o font.o icon.o
	@echo "Linking $@ ... "
	@${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

gjid.dat:	mkdata data/*
	@echo "Creating the data file ... "
	@./mkdata
