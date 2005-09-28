include ../Common.mk

BVTCCS	= $(wildcard bvt*.cc)
SRCS	= ${BVTCCS} stdtest.cc

BVTS	= $(BVTCCS:.cc=)
OBJS	= $(SRCS:.cc=.o)
LIBS	= ${LANG_LIBS} -l${LIBNAME} -lustl -lc
TOCLEAN	+= ${BVTS}

.PHONY:	all run clean depend

all:	stdtest.o ${BVTS}

run:	${BVTS}
	@for i in ${BVTS}; do echo "Running $$i"; ./$$i | diff $$i.std -; done

bvt%:	bvt%.o stdtest.o
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

%.o:	%.cc $(wildcard *.gch)
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc $(wildcard *.gch)
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

gch:	stdtest.h.gch
stdtest.h.gch:	stdtest.h $(wildcard ../*.h)
	@echo "    Creating precompiled header ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

clean:
	@echo "Removing generated files ..."
	@${RM} -f ${OBJS} ${TOCLEAN} *.rpo

depend: ${SRCS}
	@${CXX} ${CXXFLAGS} -M ${SRCS} > .depend

-include .depend
 
