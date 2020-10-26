IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
include $(IMPERAS_HOME)/bin/Makefile.include

LIB_ROOT = $(IMPERAS_LIB)/ImperasLib

ARM7_NAME = arm
ARM7_VENDOR = arm.ovpworld.org
ARM7_PROC = $(LIB_ROOT)/$(ARM7_VENDOR)/processor/$(ARM7_NAME)/1.0/model.$(SHRSUF)
ARM7_SEMI = $(LIB_ROOT)/$(ARM7_VENDOR)/semihosting/armNewlib/1.0/model.$(SHRSUF)

ATTR=modelAttrs
CROSS=ARM7
PROC=$(ARM7_PROC)
SEMI=$(ARM7_SEMI)
NAME=$(ARM7_NAME)

PSRC = platform.c
PEXE = $(foreach s,$(PSRC),$(subst .c,.$(CROSS).$(IMPERAS_ARCH).exe,$(s)))

all: $(PEXE)

%.$(CROSS).$(IMPERAS_ARCH).exe: %.c
	@ echo "Linking Platform"
	$(V) gcc -o offisVP $< $(SIM_CFLAGS) $(SIM_LDFLAGS) \
	  -DTYPE_NAME=\"$(NAME)\" \
	  -DMORPHER_FILE=\"$(PROC)\" \
	  -DMORPHER_ATTR=\"$(ATTR)\" \
	  -DSEMIHOST_FILE=\"$(SEMI)\" 

clean:
	- rm -f offisVP.exe
