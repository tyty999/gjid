include Config.mk

SUBDIRS	= src tools

.PHONY:		all install uninstall clean distclean
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

