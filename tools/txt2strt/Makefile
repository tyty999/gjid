include ../../Common.mk

EXE	= txt2strt
SRCS	= $(wildcard *.cc)
OBJS	= $(SRCS:.cc=.o)
LIBS	= -lfbgl -lustl -lc

######################################################################

all:	${EXE}

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

.PHONY: install uninstall

install: ${EXE}
	@echo "Installing ${EXE} to ${BINDIR} ..."
	@${INSTALLDIR} ${BINDIR}
	@${INSTALLBIN} ${EXE} ${BINDIR}

uninstall:
	@echo "Removing ${EXE} from ${BINDIR} ..."
	@${RM} -f ${BINDIR}/${EXE}

%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S ${OPTIMIZATION} -g0 -Os -DNDEBUG=1 -fomit-frame-pointer -o $@ -c $<

.PHONY:	clean depend

depend: ${SRCS}
	@${RM} -f .depend
	@for i in ${SRCS}; do	\
	    ${CXX} ${CXXFLAGS} -M -MT $${i%%.cc}.o $$i >> .depend;	\
	done

clean:
	@echo "Removing generated files ..."
	@${RM} -f ${EXE} ${OBJS}

-include .depend
