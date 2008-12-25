include ../Config.mk

SRCS	= $(wildcard *.cc gob/*.cc con/*.cc x11/*.cc)
INCS	= $(wildcard *.h gob/*.h con/*.h x11/*.h)
OBJS	= $(SRCS:.cc=.o)
LIBS	= -L /usr/X11R6/lib64 -lutio -lustl -lsupc++ -lX11 -lXxf86vm

########################################################################

.PHONY:	all install uninstall install-incs uninstall-incs

ifdef BUILD_SHARED
ifdef BUILD_STATIC
all:		${LIBA} ${LIBSOBLD}
install:	install-static install-shared
uninstall:	uninstall-static uninstall-shared
else
all:		${LIBSOBLD}
install:	install-shared
uninstall:	uninstall-shared
endif
else
all:		${LIBA}
install:	install-static
uninstall:	uninstall-static
endif

${LIBA}:	${OBJS}
	@echo "Linking $@ ..."
	@${AR} r $@ $?
	@${RANLIB} $@

${LIBSOBLD}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} ${SHBLDFL} -o $@ $^ ${LIBS}

.PHONY: install-static install-shared uninstall-static uninstall-shared

install-shared: ${LIBSOBLD} install-incs
	@echo "Installing ${LIBSOBLD} to ${LIBDIR} ..."
	@${INSTALLDIR} ${LIBDIR}
	@${INSTALLLIB} ${LIBSOBLD} ${LIBDIR}
	@(cd ${LIBDIR}; ${LN} -sf ${LIBSOBLD} ${LIBSO}; ${LN} -sf ${LIBSOBLD} ${LIBSOLNK})

uninstall-shared: uninstall-incs
	@echo "Removing ${LIBSOBLD} from ${LIBDIR} ..."
	@${RM} -f ${LIBDIR}/${LIBSO} ${LIBDIR}/${LIBSOLNK} ${LIBDIR}/${LIBSOBLD}

install-static: ${LIBA} install-incs
	@echo "Installing ${LIBA} to ${LIBDIR} ..."
	@${INSTALLDIR} ${LIBDIR}
	@${INSTALLLIB} ${LIBA} ${LIBDIR}

uninstall-static: uninstall-incs
	@echo "Removing ${LIBA} from ${LIBDIR} ..."
	@${RM} -f ${LIBDIR}/${LIBA}

install-incs: ${INCS}
	@echo "Installing headers to ${INCDIR} ..."
	@${INSTALLDIR} ${INCDIR}/${LIBNAME}
	@${INSTALLDIR} ${INCDIR}/${LIBNAME}/con
	@${INSTALLDIR} ${INCDIR}/${LIBNAME}/x11
	@${INSTALLDIR} ${INCDIR}/${LIBNAME}/gob
	@for i in ${INCS}; do					\
	    ${INSTALLDATA} $$i ${INCDIR}/${LIBNAME}/$$i;	\
	done;
	@${INSTALLDATA} ${LIBNAME}.h ${INCDIR}

uninstall-incs:
	@echo "Removing headers from ${INCDIR} ..."
	@${RM} -rf ${INCDIR}/${LIBNAME} ${INCDIR}/${LIBNAME}.h


%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S ${OPTIMIZATION} -g0 -Os -DNDEBUG=1 -fomit-frame-pointer -o $@ -c $<

.PHONY:	clean depend check

check:	install
	@+make -C bvt run

depend: ${SRCS}
	@${RM} -f .depend
	@for i in ${SRCS}; do	\
	    ${CXX} ${CXXFLAGS} -M -MT $${i%%.cc}.o $$i >> .depend;	\
	done
	@+make -C bvt depend

clean:
	@echo "Removing generated files ..."
	@${RM} -f ${OBJS} ${TOCLEAN}
	@+make -C bvt clean

-include .depend
 
