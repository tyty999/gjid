-include Config.mk

################ Source files ##########################################

SRCS	:= $(wildcard *.cc gob/*.cc con/*.cc x11/*.cc)
INCS	:= $(filter-out ${NAME}.h,$(wildcard *.h gob/*.h con/*.h x11/*.h))
OBJS	:= $(addprefix $O,$(SRCS:.cc=.o))

TTSEXE	:= $Otxt2strt
TTSSRC	:= txt2strt.cc gob/strt.cc gob/gif.cc
TTSOBJ	:= $(addprefix $O,$(TTSSRC:.cc=.o))

################ Compilation ###########################################

.PHONY: all clean html check dist distclean maintainer-clean

all:	Config.mk config.h ${NAME}/config.h
ALLTGTS	:= Config.mk config.h ${NAME}/config.h

ifdef BUILD_SHARED
SLIBL	:= $O$(call slib_lnk,${NAME})
SLIBS	:= $O$(call slib_son,${NAME})
SLIBT	:= $O$(call slib_tgt,${NAME})
ALLTGTS	+= ${SLIBT} ${SLIBS} ${SLIBL}

all:	${SLIBT} ${SLIBS} ${SLIBL}
${SLIBT}:	${OBJS}
	@echo "Linking $(notdir $@) ..."
	@${LD} -fPIC ${LDFLAGS} $(call slib_flags,$(subst $O,,${SLIBS})) -o $@ $^ ${LIBS}
${SLIBS} ${SLIBL}:	${SLIBT}
	@(cd $(dir $@); rm -f $(notdir $@); ln -s $(notdir $<) $(notdir $@))

endif
ifdef BUILD_STATIC
LIBA	:= $Olib${NAME}.a
ALLTGTS	+= ${LIBA}

all:	${LIBA}
${LIBA}:	${OBJS}
	@echo "Linking $@ ..."
	@rm -f $@
	@${AR} qc $@ ${OBJS}
	@${RANLIB} $@
endif
ifdef TTSEXE
ALLTGTS	+= ${TTSEXE}

all:	${TTSEXE}
${TTSEXE}:	${TTSOBJ}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ ${TTSOBJ} ${TTSLIBS}
endif

$O%.o:	%.cc
	@echo "    Compiling $< ..."
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@${CXX} ${CXXFLAGS} -MMD -MT "$(<:.cc=.s) $@" -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

include bvt/Module.mk

################ Installation ##########################################

.PHONY:	install uninstall install-incs uninstall-incs

####### Install headers

ifdef INCDIR	# These ifdefs allow cold bootstrap to work correctly
LIDIR	:= ${INCDIR}/${NAME}
INCSI	:= $(addprefix ${LIDIR}/,$(filter-out ${NAME}.h,${INCS}))
RINCI	:= ${LIDIR}.h

install:	install-incs
install-incs: ${INCSI} ${RINCI}
${INCSI}: ${LIDIR}/%.h: %.h
	@echo "Installing $@ ..."
	@${INSTALLDATA} $< $@
${RINCI}: ${NAME}.h
	@echo "Installing $@ ..."
	@${INSTALLDATA} $< $@
uninstall:	uninstall-incs
uninstall-incs:
	@echo "Removing ${LIDIR}/ and ${LIDIR}.h ..."
	@(cd ${INCDIR}; rm -f ${INCSI} ${NAME}.h; [ ! -d ${NAME} ] || rm -rf ${NAME})
endif

####### Install libraries (shared and/or static)

ifdef LIBDIR
ifdef BUILD_SHARED
LIBTI	:= ${LIBDIR}/$(notdir ${SLIBT})
LIBLI	:= ${LIBDIR}/$(notdir ${SLIBS})
LIBSI	:= ${LIBDIR}/$(notdir ${SLIBL})
install:	${LIBTI} ${LIBLI} ${LIBSI}
${LIBTI}:	${SLIBT}
	@echo "Installing $@ ..."
	@${INSTALLLIB} $< $@
${LIBLI} ${LIBSI}: ${LIBTI}
	@(cd ${LIBDIR}; rm -f $@; ln -s $(notdir $<) $(notdir $@))
endif
ifdef BUILD_STATIC
LIBAI	:= ${LIBDIR}/$(notdir ${LIBA})
install:	${LIBAI}
${LIBAI}:	${LIBA}
	@echo "Installing $@ ..."
	@${INSTALLLIB} $< $@
endif
ifdef TTSEXE
TTSEXEI	:= ${BINDIR}/$(notdir ${TTSEXE})
install:	${TTSEXEI}
${TTSEXEI}:	${TTSEXE}
	@echo "Installing $@ ..."
	@${INSTALLEXE} $< $@
endif

uninstall:
	@echo "Removing library from ${LIBDIR} ..."
	@rm -f ${LIBTI} ${LIBLI} ${LIBSI} ${LIBAI} ${TTSEXEI}
endif

################ Maintenance ###########################################

clean:
	@[ ! -d ./$O ] || rm -rf ./$O

html:	${SRCS} ${INCS} ${NAME}doc.in
	@${DOXYGEN} ${NAME}doc.in

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
	@rm -f Config.mk config.h config.status ${NAME}

maintainer-clean: distclean
	@if [ -d docs/html ]; then rm -f docs/html/*; rmdir docs/html; fi

INPLACE_INCS := $(addprefix ${NAME}/,$(filter-out config.h,${INCS}))
${INPLACE_INCS}: ${NAME}/%:	${NAME}/config.h
${NAME}/config.h:	config.h
	@echo "    Linking inplace header location ..."
	@rm -f ${NAME}; ln -s . ${NAME}

${OBJS}:		Makefile Config.mk config.h
Config.mk:		Config.mk.in
config.h:		config.h.in
Config.mk config.h:	configure
	@if [ -x config.status ]; then echo "Reconfiguring ..."; ./config.status; \
	else echo "Running configure ..."; ./configure; fi

-include ${OBJS:.o=.d}
