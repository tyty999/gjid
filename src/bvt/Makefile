include ../Common.mk

BVTCCS	= $(wildcard bvt*.cc)
SRCS	= ${BVTCCS}

BVTS	= $(BVTCCS:.cc=)
OBJS	= $(SRCS:.cc=.o)
LIBS	= ${LANG_LIBS} -l${LIBNAME} -lustl -lc
TOCLEAN	+= ${BVTS}

.PHONY:	all run clean depend

all:	${BVTS}

run:	${BVTS}
	@for i in ${BVTS}; do echo "Running $$i"; ./$$i < $$i.cc | diff $$i.std -; done

bvt%:	bvt%.o
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

clean:
	@echo "Removing generated files ..."
	@${RM} -f ${OBJS} ${TOCLEAN} *.rpo

depend: ${SRCS}
	@${CXX} ${CXXFLAGS} -M ${SRCS} > .depend

-include .depend
 
