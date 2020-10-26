IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
CFLAGS=-DENDIANBIG
include $(IMPERAS_HOME)/ImperasLib/source/buildutils/Makefile.pse

cleanall: clean
	make -C platform    clean NOVLNV=1
	make -C application clean
	-rm -f *.igen.*
