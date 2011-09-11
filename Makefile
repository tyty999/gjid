-include Config.mk

################ Source files ##########################################

EXE	:= ${NAME}
SRCS	:= $(wildcard *.cc fbgl/*.cc fbgl/con/*.cc fbgl/gob/*.cc fbgl/x11/*.cc)
OBJS	:= $(addprefix $O,$(SRCS:.cc=.o))
DATAF	:= data/${NAME}.cpio
DATAFC	:= data/default.fnt data/levels.dat

################ Compilation ###########################################

.PHONY: all clean dist distclean maintainer-clean

all:	Config.mk config.h ${EXE} ${DATAF}

run:	${EXE} ${DATAF}
	@./${EXE}

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${DATAF}:	${DATAFC}
	@echo "Creating the data file ... "
	@(cd data; ls $(notdir $^) | cpio -o > $(notdir $@))

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
	@[ ! -d ./$O ] || rm -rf ./$O data/${EXE}.cpio

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
