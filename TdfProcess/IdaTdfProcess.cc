
//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaTdfProcess.cc 1.1
//
//   File:      IdaTdfProcess.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:13:48
//
//   DESCRIPTION:
//
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTdfProcess_cc = "@(#) IdaTdfProcess.cc 1.1";






//--------------------- include files ------------------------------------
#include <pcpdefs.h>
#include <IdaDecls.h>
#include <idatraceman.h>
#include <pcpprocess.h>
#include <pcptime.h>
#include <pcpstring.h>
#include <tdsresponse.h>
#include <fstream.h>
#include <syspar.h>
#include <stdlib.h>
#include <iostream.h>
#include <unistd.h>
#include <ioformat.h>
#include <toolbox.h>
#include <reporterclient.h>
#include <sys/resman.h>

#include <IdaRequestContainer.h>

#ifdef _HPUX_11
#include <locale.h>
#endif

#ifdef OS_AIX_4
  #define bool int
#endif

#include <sys/ipc.h>


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <IdaTdfProcess.h>
#include <IdaTdfAccess.h>
#include <osalimits.h>
#include <IdaDecls.h>

# ifdef CLASSLIB_03_00
  #include <cltracecategory.h> 
  #include <pcpdispatcher.h>
  #include <sys/tracemanager.h>
# else
  #include <dispatcher.h>
# endif




#ifdef ALLOW_STDOUT
	#define MONITORING
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////
//
TdfProcess::TdfProcess(Int argc, Char * argv[], Char * envp[])
    : Process (argc, argv, envp)
{

    // Traceausgabe initialisieren
    String traceFileName;
    int traceLevel;
    char pidStr[256];


// trace file name ist set by classlib Process constructor!
// trace level is set by classlib constructor


    char *locale;
    locale = setlocale( LC_ALL, "");
    if (NULL == locale) {
        idaTrackExcept(( "unable to read locale\n"));
    }
    else {
        idaTrackData(("Locale = %s", locale));
    }



    // Reporterausgabe initialisieren
    ReporterClient* repClient = Process::getToolBox()->getReporter();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
ReturnStatus TdfProcess::initialize(const String& parFileName)
{
#ifdef CLASSLIB_03_00
  TRACE_FUNCTION("TdfProcess::initialize(...)");
#endif
	idaTrackTrace(("TdfProcess::initialize"));


    ReporterClient* repClient = Process::getToolBox()->getReporter();

	// der OSA-API vorgaukeln, dass "osaApiInit()" aufgerufen wurde
    OsaLimits::getInstance()->setOsaComErrorCode(0);

	// register ohne login erlauben
	OsaLimits::getInstance()->setDoCheck(false);


	// -------------------------------------------------------------------------
    SysParam sysParam;

    // Read declaration file
    if (sysParam.readDeclaration(idaDecls) == isNotOk)
    {
        idaTrackFatal(("TdfProcess::initialize : Error in Decl :\n%s\n%s",
                   sysParam.getErrorText().cString(),
                   sysParam.getErrorEnvironment().cString()));
		
		#ifdef MONITORING
			cout << "sysParam.readDeclaration(idaDecls) failed" << endl;
		#endif
		
		return isNotOk;
    }

    // Read parameter file

	#ifdef MONITORING
		cout << "parFileName = " << parFileName.cString() << endl;
	#endif

	idaTrackData(("parFileName = %s", parFileName.cString()));

	ifstream parFile(parFileName.cString());
    if (!parFile)
    {
		idaTrackFatal(("TdfProcess::initialize : creating infilestream failed"));
		
		#ifdef MONITORING
			cout << "creation of \"ifstream parFile()\" failed" << endl;
		#endif
		
		return isNotOk;
    }
    if (sysParam.readAllParams(parFile) == isNotOk)
    {
        idaTrackFatal(("Error reading par file %s:\n%s\n%s",
                   parFileName.cString(), sysParam.getErrorText().cString(),
                   sysParam.getErrorEnvironment().cString()));
		
		#ifdef MONITORING
			cout << "sysParam.readAllParams(parFile) failed" << endl;
		#endif
		
		return isNotOk;
    }
    parFile.close();


	// -------------------------------------------------------------------------
    // Read timer values and system limits
    ULong  registerTimer		= 0;
    ULong  deRegisterTimer		= 0;
    ULong  searchIntervalTimer	= 0;
    ULong  searchTimeout		= 0;
    ULong  statusReportTimer	= 0;
    long maxRegistration		= 0;
    UShort maxSearchRequest		= 0;
	String regressionTestValue;
	Bool   regressionTest		= false;

    SysParamGroup timerAndMaxValueGroup("TimerAndMaxValueGroup", true);
    if (sysParam.getFirstParamGroup("TimerAndMaxValueGroup", timerAndMaxValueGroup) == isOk)
    {
        if (timerAndMaxValueGroup.getParameter("register_timer", registerTimer) == isNotOk)
			registerTimer = 0;
		idaTrackData(("register_timer = %d", registerTimer));
        
		if (timerAndMaxValueGroup.getParameter("deregister_timer", deRegisterTimer) == isNotOk)
			deRegisterTimer = 0;
        
        if (timerAndMaxValueGroup.getParameter("search_interval_timer", searchIntervalTimer) == isNotOk)
			searchIntervalTimer = 0;
        
        if (timerAndMaxValueGroup.getParameter("search_timeout", searchTimeout) == isNotOk)
			searchTimeout = 0;
		idaTrackData(("search_timeout = %d", searchTimeout));
        
        if (timerAndMaxValueGroup.getParameter("statusreport_timer", statusReportTimer) == isNotOk)
			statusReportTimer = 0;
        
        if (timerAndMaxValueGroup.getParameter("max_registration", maxRegistration) == isNotOk)
			maxRegistration = 0;
		idaTrackData(("max_registration = %d", maxRegistration));
        
        if (timerAndMaxValueGroup.getParameter("max_search_request", maxSearchRequest) == isNotOk)
			maxSearchRequest = 0;
        
//        if (timerAndMaxValueGroup.getParameter("regression_test", regressionTestValue) == isNotOk)
//			maxSearchRequest = 0;
//		if (regressionTestValue == "yes" || regressionTestValue == "Yes" || regressionTestValue == "YES")
//			regressionTest = true;
    }
	else
	{
		idaTrackTrace(("TdfProcess::initialize : Cannot access group \"TimerAndMaxValueGroup\""));
		#ifdef MONITORING
			cout << "Cannot access group \"TimerAndMaxValueGroup\"" << endl;
		#endif
	}




	// -------------------------------------------------------------------------
	// Basis OID für den WebProcess holen. Auf diesen wird später für jede
	// Prozeß-Instanz ein entsprechender Offset hinzu addiert.
    SysParamGroup webProcessGroup("WebProcessGroup", true);
    if (sysParam.getFirstParamGroup("WebProcessGroup", webProcessGroup) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : WebProcessGroup missing",
                   parFileName.cString()));
		#ifdef MONITORING
			cout << "Cannot access group \"WebProcessGroup\"" << endl;
		#endif
		return isNotOk;
    }
	// BaseCommOID
    RefId baseOid;
    if (webProcessGroup.getParameter("base_objectid", baseOid) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"base_objectid\" missing",
                   parFileName.cString()));
		return isNotOk;
    }
	// maxWebProcs
    int maxWebProcs;
    if (webProcessGroup.getParameter("max_web_procs", maxWebProcs) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"max_web_procs\" missing",
                   parFileName.cString()));
		return isNotOk;
    }


	// -------------------------------------------------------------------------
	// Und jetzt wird fuer jede TdfProcessGroup ein TdfAccess-Objekt angelegt
    // Initialize attribute mapping handler
	ReturnStatus returnStatus;
    int clientCount = 1;
	SysParamGroup tdfProcessGroup("TdfProcessGroup", true);
	returnStatus = sysParam.getFirstParamGroup("TdfProcessGroup", tdfProcessGroup);
    if (returnStatus == isNotOk)
    {
		idaTrackFatal(("TdfProcess::initialize : There must exist at least one \"TdfProcessGroup\""));
		#ifdef MONITORING
			cout << "Cannot access group \"TdfProcessGroup\"" << endl;
		#endif
		return isNotOk;
    }

    while (returnStatus == isOk)
    {
        long	ownOid				= 0;
	long	dbId				= 0;
	long	backupOwnOid			= 0;
        long	dbServerOid			= 0;
        long	dbServerCtlOid		= 0;
        
		UShort	noOfChannels;
        String	countryCode;
        String	serviceName;
        String	applName;
        String	osaTicket;

        int		dataMissing = 0;

        if (tdfProcessGroup.getParameter("objectid", ownOid) == isNotOk)
        {

          idaTrackFatal (("TdfProcess::initialize : parameter 'objectid' missing or syntax error in definition"));
        }
        
        if (tdfProcessGroup.getParameter("dbid", dbId) == isNotOk)
        {

          idaTrackFatal (("TdfProcess::initialize : parameter 'dbId' missing or syntax error in definition"));
        }

	if (tdfProcessGroup.getParameter("backup_objectid", backupOwnOid) == isNotOk)
           idaTrackTrace(("No tdf backup object configured!"));
        if (tdfProcessGroup.getParameter("dbserver_objectid", dbServerOid) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'dbserver_objectid' missing or syntax error in definition"));
          dataMissing = 1;
        }

        if (tdfProcessGroup.getParameter("dbserverctl_objectid", dbServerCtlOid) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'dbserverctl_objectid' missing or syntax error in definition"));
          dataMissing = 1;
        }
        if (tdfProcessGroup.getParameter("service_name", serviceName) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'service_name' missing or syntax error in definition"));
          dataMissing = 1;
        }
        if (tdfProcessGroup.getParameter("application_name", applName) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'application_name' missing or syntax error in definition"));
          dataMissing = 1;
        }
        if (tdfProcessGroup.getParameter("nrof_channels", noOfChannels) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'nrof_channels' id missing or syntax error in definition"));
          dataMissing = 1;
        }
        if (tdfProcessGroup.getParameter("osa_ticket", osaTicket) == isNotOk)
        {
          idaTrackFatal (("TdfProcess::initialize : parameter 'osa_ticket' id missing or syntax error in definition"));
          dataMissing = 1;
        }

        if (dataMissing)
        {
          idaTrackFatal(("TdfProcess::initialize : Data for TDF client group missing exiting"));
          return isNotOk;
        }
        else
        {
          SesConfig sesConfig(SesConfig::NONE);

            // Anlegen der TDF_Client Instanzen
            TdfAccess* tdfAccess = new TdfAccess(ownOid,
												 dbId,
												 ObjectId(dbServerOid),
												 serviceName,
												 applName,
												 noOfChannels,
												 osaTicket,
												 registerTimer,
												 searchTimeout,
												 maxRegistration,
												 regressionTest,
												 sesConfig);

			// Aktivieren der des Dispatchers zur Message-Übermittlung
            tdfAccess->enableApplicationMessageBox();
            idaTrackData(("enableApplicationMessageBox() ...  done"));

			// Wir merken uns die Instanz in einer Liste, damit sie später
			// wieder abgebaut werden kann
            tdfAccessList.push_back(tdfAccess);
            idaTrackTrace(("TDF client #%d created, OID:%d, DBID:%d",
                                         clientCount, ownOid, dbId));
        }

        ++clientCount;
        returnStatus = sysParam.getNextParamGroup("TdfProcessGroup", tdfProcessGroup);
    }

    initComplete();

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
TdfProcess::~TdfProcess()
{
    idaTrackTrace(("Destructor TdfProcess"));

    // Freigeben der TdfAccess Instanzen
    s_vector<TdfAccess*>::iterator cursor = tdfAccessList.begin();
    while (cursor != tdfAccessList.end())
    {
        delete *cursor;
        cursor++;
    }

	// Den eigenen Prozeß auschecken
# ifdef CLASSLIB_03_00
     Long ownOid;
    if (Process::getResMan()->getOwnProcRefId(ownOid) == isNotOk)
# else
    ObjectId ownOid;
    if (Process::getResMan()->getOwnObjectId(ownOid) == isNotOk)
# endif
    {
//        ownOid = noKnownId;
    }
    Process::getComMan()->dispatcherCheckOut(ownOid);
    Process::getComMan()->checkOut(ownOid);
    shutdownComplete(ownOid);


    ReporterClient* repClient = Process::getToolBox()->getReporter();
//    repClient->reportEvent(noKnownId, iDAMinRepClass, 202, " TdfProcess");
}




/*CB>---------------------------------------------------------------------

  DESCRIPTION:
        Main process loop

--------------------------------------------------------------------------
<CE*/
void TdfProcess::run()
{
#ifdef CLASSLIB_03_00
    TRACE_FUNCTION("TdfProcess::run(...)");
#endif    
	idaTrackTrace(("TdfProcess::run"));
    ComMan::getDispatcher()->dispatchForever();
}




// *** EOF ***





