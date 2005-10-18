include Common.mk

EXE	= gjid
OBJS	= $(filter-out mkdata.o, $(SRCS:.cc=.o))
SRCS	= $(wildcard *.cc)
LIBS	+= -lfbgl -lustl -lc

all:	${EXE} data/gjid.dat

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

mkdata:	mkdata.o $(filter-out gjid.o,$(OBJS))
	@echo "Linking $@ ... "
	@${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

data/gjid.dat:	mkdata $(filter-out data/gjid.dat,$(wildcard data/*))
	@echo "Creating the data file ... "
	@./mkdata

install: ${EXE}
	@echo "Installing ${EXE} to ${BINDIR} ..."
	@${INSTALLDIR} ${BINDIR}
	@${INSTALLEXE} ${EXE} ${BINDIR}

uninstall:
	@echo "Removing ${EXE} from ${BINDIR} ..."
	@${RM} -f ${BINDIR}/${EXE}

%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} ${OPTIMIZE} -g0 -Os -fomit-frame-pointer -DNDEBUG=1 -S -o $@ -c $<

%.h.gch:	%.h
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

.PHONY:	clean depend dox confclean

clean:
	@echo "Removing generated files ..."
	@${RM} -f ${OBJS} ${EXE} mkdata mkdata.o data/gjid.dat

depend: ${SRCS}
	@${RM} -f .depend
	@for i in ${SRCS}; do	\
	    ${CXX} ${CXXFLAGS} -M -MT $${i%%.cc}.o $$i >> .depend;	\
	done

confclean:
	rm -f bsconf Common.mk config.h

-include .depend
