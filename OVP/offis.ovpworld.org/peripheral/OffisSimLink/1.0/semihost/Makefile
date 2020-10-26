IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
CFLAGS=-I$(IMPERAS_LIB)/External/include/SDL -I$ "C:\Program Files (x86)\MATLAB\R2010bSP1\extern\include"
LDFLAGS=-L$(IMPERAS_LIB)/External/lib -lSDL -L"C:\Program Files (x86)\MATLAB\R2010bSP1\bin\win32" -lmx -leng
include $(IMPERAS_HOME)/ImperasLib/source/buildutils/Makefile.host
