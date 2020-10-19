RELATIVEROOTDIR=..

include $(RELATIVEROOTDIR)/Makefile.head

MK_REL_BINPATH=$(MODULESPATH)

LIBSRCS=IdaWebProcess

LIBNAME=libidaweb

BINARIES= \
	IdaWebProcess 

include $(RELATIVEROOTDIR)/Makefile.tail

LIBRARIES=



ifeq ($(UNAME), Windows)
$(MK_REL_BINPATH)/IdaWebProcess$(EXE): $(PLATFORMDIR)/IdaWebProcessMain.o	
	$(LINKOPS) libidabase.lib \
	libidaweb.lib libtdframe.lib \
	libtdsearch.lib libapibase.lib $(OBJCOMLIBS) $(NETLIBS)
else
$(MK_REL_BINPATH)/IdaWebProcess$(EXE): $(PLATFORMDIR)/IdaWebProcessMain.o	
	$(LINKOPS) -lidabase -lidaweb -ltdframe -ltdsearch \
	-lapibase -lseslogin $(OBJCOMLIBS) $(NETLIBS)
endif






