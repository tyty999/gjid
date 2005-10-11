include Common.mk

EXE	= gjid
OBJS	= gjid.o level.o font.o icon.o
SRCS	= gjid.cc level.cc font.cc icon.cc mkdata.cc
LIBS	+= -lfbgl -lustl -lc

all:	${EXE} data/gjid.dat

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

mkdata:	mkdata.o level.o font.o icon.o
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
	@${RM} -f ${OBJS} ${EXE} data/gjid.dat

depend: ${SRCS}
	@${RM} -f .depend
	@for i in ${SRCS}; do	\
	    ${CXX} ${CXXFLAGS} -M -MT $${i%%.cc}.o $$i >> .depend;	\
	done

confclean:
	rm -f bsconf Common.mk config.h

-include .depend
