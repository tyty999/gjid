include ../Common.mk

SUBDIRS	= 

.PHONY:		all install
all:		subdirs-all
install:	subdirs-install

subdirs-%:
	@for i in ${SUBDIRS}; do	\
	    (echo "Running $* in $$i";	\
		${MAKE} -s -C $$i $*;	\
		echo "Leaving $$i")	\
	    || exit;			\
	done

