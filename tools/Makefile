include ../Common.mk

SUBDIRS	= txt2strt

.PHONY:		all install
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

