include Common.mk

SUBDIRS	= src tools

.PHONY:		all install uninstall
all:		subdirs-all
install:	subdirs-install
uninstall:	subdirs-uninstall

subdirs-%:
	@for i in ${SUBDIRS}; do	\
	    (echo "Running $* in $$i";	\
		${MAKE} -s -C $$i $*;	\
		echo "Leaving $$i")	\
	    || exit;			\
	done

dist-clean:	clean
	@rm -f Common.mk config.h bsconf.o bsconf
	@rm -f `find . -name .depend`

