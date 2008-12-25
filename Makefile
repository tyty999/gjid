-include Config.mk

################ Source files ##########################################

EXE	:= ${NAME}
SRCS	:= $(filter-out mkdata.cc,$(wildcard *.cc))
OBJS	:= $(addprefix $O,$(SRCS:.cc=.o))

################ Compilation ###########################################

.PHONY: all clean dist distclean maintainer-clean

all:	Config.mk config.h ${EXE} data/gjid.dat

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

$Omkdata:	$Omkdata.o $(filter-out $Ogjid.o,${OBJS})
	@echo "Linking $@ ... "
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

data/gjid.dat:	$Omkdata $(filter-out data/gjid.dat,$(wildcard data/*)) data/strings.strt
	@echo "Creating the data file ... "
	@$Omkdata

data/strings.strt:	data/strings.txt
	@echo "Creating the strings file ... "
	@txt2strt $^

$O%.o:	%.cc
	@echo "    Compiling $< ..."
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@${CXX} ${CXXFLAGS} -MMD -MT "$(<:.cc=.s) $@" -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

################ Installation ##########################################

.PHONY:	install uninstall

ifdef BINDIR
EXEI	:= $(addprefix ${BINDIR}/,${EXE})

install:	${EXEI}
${EXEI}:	${EXE}
	@echo "Installing $< as $@ ..."
	@${INSTALLEXE} $< $@

uninstall:
	@echo "Removing ${EXEI} ..."
	@rm -f ${EXEI}
endif

################ Maintenance ###########################################

clean:
	@rm -f ${EXE} ${OBJS} $(OBJS:.o=.d)
	@rmdir $O &> /dev/null || true

ifdef MAJOR
DISTVER	:= ${MAJOR}.${MINOR}
DISTNAM	:= ${NAME}-${DISTVER}
DISTLSM	:= ${DISTNAM}.lsm
DISTTAR	:= ${DISTNAM}.tar.bz2

dist:
	@echo "Generating ${DISTTAR} and ${DISTLSM} ..."
	@mkdir .${DISTNAM}
	@rm -f ${DISTTAR}
	@cp -r * .${DISTNAM} && mv .${DISTNAM} ${DISTNAM}
	@+${MAKE} -sC ${DISTNAM} maintainer-clean
	@tar jcf ${DISTTAR} ${DISTNAM} && rm -rf ${DISTNAM}
	@echo "s/@version@/${DISTVER}/" > ${DISTLSM}.sed
	@echo "s/@date@/`date +%F`/" >> ${DISTLSM}.sed
	@echo -n "s/@disttar@/`du -h --apparent-size ${DISTTAR}`/" >> ${DISTLSM}.sed;
	@sed -f ${DISTLSM}.sed docs/${NAME}.lsm > ${DISTLSM} && rm -f ${DISTLSM}.sed
endif

distclean:	clean
	@rm -f Config.mk config.h config.status

maintainer-clean: distclean

${OBJS}:		Makefile Config.mk config.h
Config.mk:		Config.mk.in
config.h:		config.h.in
Config.mk config.h:	configure
	@if [ -x config.status ]; then echo "Reconfiguring ..."; ./config.status; \
	else echo "Running configure ..."; ./configure; fi

-include ${OBJS:.o=.d}
