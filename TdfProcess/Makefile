RELATIVEROOTDIR=..

include $(RELATIVEROOTDIR)/Makefile.head

MK_REL_BINPATH=$(MODULESPATH)

LIBSRCS=IdaDomErrorHandler \
	IdaTdfChannel \
	IdaTdfChannelMgr \
	IdaRequestContainer \
	IdaRequestList \
	IdaStringToEnum \
	IdaTdfAccess \
   IdaTdfProcess

LIBNAME=libidatdf

BINARIES= \
	IdaTdfProcess 

include $(RELATIVEROOTDIR)/Makefile.tail

LIBRARIES=



ifeq ($(UNAME), Windows)
$(MK_REL_BINPATH)/IdaTdfProcess$(EXE): $(PLATFORMDIR)/IdaTdfProcessMain.o	
	$(LINKOPS) libidabase.lib \
	libidatdf.lib libtdframe.lib \
	libtdsearch.lib libapibase.lib $(OBJCOMLIBS) $(NETLIBS)
else
$(MK_REL_BINPATH)/IdaTdfProcess$(EXE): $(PLATFORMDIR)/IdaTdfProcessMain.o	
	$(LINKOPS) -lidabase -lidatdf -ltdframe -ltdsearch \
	-lapibase -lseslogin $(OBJCOMLIBS) $(NETLIBS)
endif






