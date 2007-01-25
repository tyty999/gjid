include Config.mk

SUBDIRS	= src tools

.PHONY:		all install uninstall clean distclean dist
all:		subdirs-all
install:	subdirs-install
uninstall:	subdirs-uninstall
clean:		subdirs-clean

subdirs-%:
	@for i in ${SUBDIRS}; do	\
	    (echo "Running $* in $$i";	\
		${MAKE} -s -C $$i $*;	\
		echo "Leaving $$i")	\
	    || exit;			\
	done

distclean:	clean
	@rm -f Config.mk config.h bsconf.o bsconf `find . -name .depend`

TMPDIR	= /tmp
DISTDIR	= ${HOME}/stored
DISTNAM	= ${LIBNAME}-${MAJOR}.${MINOR}
DISTTAR	= ${DISTNAM}.${BUILD}.tar.bz2

dist:
	mkdir ${TMPDIR}/${DISTNAM}
	cp -r . ${TMPDIR}/${DISTNAM}
	+${MAKE} -C ${TMPDIR}/${DISTNAM} distclean
	(cd ${TMPDIR}/${DISTNAM}; rm -rf `find . -name .svn`)
	(cd ${TMPDIR}; tar jcf ${DISTDIR}/${DISTTAR} ${DISTNAM}; rm -rf ${DISTNAM})

