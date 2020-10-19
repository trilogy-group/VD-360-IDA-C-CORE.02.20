RELATIVEROOTDIR=..
include $(RELATIVEROOTDIR)/Makefile.head

MK_REL_BINPATH=$(UTILPATH)

LIBSRCS=	IdaTraceMgr


LIBNAME=libidabase


include $(RELATIVEROOTDIR)/Makefile.tail

ifeq ($(UNAME), Windows)
$(MK_REL_BINPATH)/ftok$(EXE): $(PLATFORMDIR)/ftok.o	
	$(LINKOPS) libidabase.lib 
else
$(MK_REL_BINPATH)/ftok$(EXE): $(PLATFORMDIR)/ftok.o	
	$(LINKOPS) -lidabase
endif

