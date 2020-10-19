//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  WebProcess/IdaWebProcessMain.cc 1.0 12-APR-2008 18:52:13 DMSYS
//
//   File:      WebProcess/IdaWebProcessMain.cc
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:13
//
//   DESCRIPTION:
//     IDA.plus Web process main program.
//
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaWebProcess_cc = "@(#) WebProcess/IdaWebProcessMain.cc 1.0 12-APR-2008 18:52:13 DMSYS";


#include <IdaDecls.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <syspar.h>
#include <syspargrp.h>
#include <idatraceman.h>
#include <pcptime.h>
#include <IdaTraceMgr.h>
#include <IdaTypes.h>
#include <IdaDatabaseList.h>
#include <IdaWebProcess.h>
#include <IdaFunctionTrace.h>
#include <pcpstring.h>

#ifdef ALLOW_STDOUT
	#define MONITORING
#endif


/** =============================================================== **/
/** ==                  M a i n   p r o g r a m                  == **/
/** =============================================================== **/

int main(int argc, char** argv, char** envp)
{
	cerr << "WebProcess started ..." << endl;
    
    String parameter1 ( argv[1] );
	// Check parameter
	if ( (argc < 2 ) || ( (argc < 4) && (parameter1 == "115") ) )
	{
		cerr << "error: missing arguments" << endl;
		cerr << "Usage: " << argv[0] << " <CommSlot> <ParFile> "
			 << "[ -tr <TraceLevel> -tf <TraceFile> ]" << endl;
		exit(1);
	}

   String parFileName;
   

	// -------------------------------------------------------------------------
	// Als erstes versuchen wir den Trace-Mechanismus zu aktivieren,
	// damit wir sofort Fehler ausgeben können:
    
	String			traceFileName;
    int				traceLevel = 0;
    char			pidStr[256];

	// Parameter holen, wenn verfügbar
    TraceMgr::getTraceSettings(argc, argv, traceFileName, traceLevel);
    sprintf(pidStr, "%d", getpid());
	
	// Wenn kein Filename angegeben wurde, nehmen wir den Prozeßnamen
    if (traceFileName.isEmpty())
	 {
		traceFileName.assign(argv[0]);
		
   	// ... und hängen die PID hinten an
		traceFileName = traceFileName + pidStr;
	 }
	 

# ifndef CLASSLIB_03_00
    // Trace-System initialisieren fuer classlib 2.20
    TraceMan::traceInit(traceFileName);
    TraceMan::setTraceLevel(traceLevel);

    if (traceLevel)
    {
      cout<<"Startup WebProcess"<<endl;
    }
	idaTrackData(("TraceLevel = %d", traceLevel));
# else

    // Trace-System initialisieren fuer classlib 3.0

    TraceManager::getTraceManager ()-> setFileOutput ( traceFileName );
    TraceManager::getTraceManager ( ) -> setCategoryTraceLevel ( "", traceLevel );

# endif

	#ifdef MONITORING
		cout << "traceLevel = " << traceLevel << endl;
	#endif


      
	// -------------------------------------------------------------------------
	// Extract Parameter- and Declaration-Filename
    int commSlot;	
    for (int i = 0; i < argc; i++)
    {
      idaTrackData (("IdaWebProcess called with arg %d '%s'", i, argv[i] ));
    }
    
   if ( parameter1 == "115" )
   {
     parFileName = String(argv[3]);
     commSlot    = atoi(argv[4]);
     idaTrackData (("Ida Parameter file found in 3rd arg %s, arg 1 is %s", parFileName.cString(),argv[1] ));
   }
   else
   {
     parFileName = String(argv[2]);
     commSlot    = atoi(argv[1]);
     idaTrackData (("Ida Parameter file found in 2rd arg %s, arg 1 is %s", parFileName.cString(),argv[1] ));
   }
	
	// -------------------------------------------------------------------------
    // Jetzt versuchen wir mal das Lesen der Parameter-Datei (typisch "ida.par") 
	// vorzubereiten:
    SysParam sysParam;
	// Wir geben dem Parameterfile-Reader ein Template ("IdaDecls.h") der 
	// Struktur der Parameterdatei 
    if (sysParam.readDeclaration(idaDecls) == isNotOk)
    {

		// Ohne Parameter geht nichts !
        idaTrackFatal(("Declaration section IdaDecls invalid. Reason: %s", sysParam.getErrorText().cString()));
		  cerr<<"Declaration section IdaDecls invalid. Reason:" << sysParam.getErrorText().cString() << endl;
		exit(1);
    }

    // Parameterfile lesen
    ifstream parFile(parFileName.cString());
    if (!parFile)
    {
		// Parameterfile konnte nicht geöffnet werden
		idaTrackFatal(("Parameter file %s not found", parFileName.cString()));
        cerr<<"Parameter file %s not found"<<parFileName<<endl;
		exit(1);
    }
    if (sysParam.readAllParams(parFile) == isNotOk)
    {
		// Parameterfile vorhanden, aber syntaktisch nicht korrekt
		idaTrackFatal(("Parameter file %s invalid. Reason: %s", parFileName.cString(), sysParam.getErrorText().cString()));
        cerr<<"Parameter file"<<parFileName<<"invalid. Reason: "<<sysParam.getErrorText().cString()<<endl;
		exit(1);
    }
    parFile.close();


	// -------------------------------------------------------------------------
	// Basis OID für den WebProcess holen. Auf diesen wird später für jede
	// Prozeß-Instanz ein entsprechender Offset (aus shared memory) hinzu addiert.
    SysParamGroup webProcessGroup("WebProcessGroup", true);
    if (sysParam.getFirstParamGroup("WebProcessGroup", webProcessGroup) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : WebProcessGroup missing",parFileName.cString()));
          cerr<<"Parameter file is invalid!! Reason : WebProcessGroup missing"<<endl;
		exit(1);
    }

	// BaseCommOID
    RefId baseOid;
    if (webProcessGroup.getParameter("base_objectid", baseOid) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"base_objectid\" missing", parFileName.cString()));
          cerr<<"parameter file invalid. Reason : \"base_objectid\" missing"<<endl;
		exit(1);
    }
    idaTrackData(("Base Object-ID for WebProcess: %d", baseOid));
    cout<<"Base Object-ID for WebProcess: "<<baseOid<<endl;

	// BasePort für die Socket Kommunikation
    int socketPort;
    if (webProcessGroup.getParameter("base_socket_port", socketPort) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"base_socket_port\" missing", parFileName.cString()));
          cerr<<"Parameter file invalid. Reason : \"base_socket_port\" missing"<<endl;
        exit(1);
    }
      idaTrackData(("Base Socket-Port for WebProcess: %d", socketPort));
      cout<<"Base Socket-Port for WebProcess:" <<socketPort<<endl;


	// Jetzt brauchen wir noch die OID des TdfClient
    // Extract base object ID for query manager process(es)
    SysParamGroup tdfProcessGroup("TdfProcessGroup", true);
	
	ReturnStatus returnStatus;
	returnStatus = sysParam.getFirstParamGroup("TdfProcessGroup", tdfProcessGroup);
    if (returnStatus == isNotOk)
    {
		// Einen TdfClient brauchen wir mindestens, also Fehler und Abbruch
        idaTrackFatal(("Parameter file %s invalid. Reason : TdfProcessGroup missing", parFileName.cString()));
          cerr<<"Parameter file %s invalid. Reason : TdfProcessGroup missing"<<endl;
		exit(1);
    }

	// Wir iterieren über alle Subgroups von TdfProcessGroup und "sammeln" die OID's
	// der einzelnen TdfClients, damit diese später unter diesen OID's angesprochen werden können
	DatabaseList databaseList;
	while (returnStatus == isOk)
	{
		RefId objectid;
		if (tdfProcessGroup.getParameter("objectid", objectid) == isNotOk)
		{
			idaTrackFatal(("Parameter file %s invalid. Reason : \"objectid\" missing", parFileName.cString()));
            cerr<<"Parameter file invalid. Reason : \"objectid\" missing"<<endl;
			exit(1);
		}
		RefId dbid;
		if (tdfProcessGroup.getParameter("dbid", dbid) == isNotOk)
		{
			idaTrackFatal(("Parameter file %s invalid. Reason : \"dbid\" missing", parFileName.cString()));
            cerr<<"Parameter file invalid. Reason : \"dbid\" missing"<<endl;
			exit(1);
		}
		idaTrackData(("Found DB with OID: %d and DBID: %d", objectid, dbid));
        cout<<"Found DB with OID: "<<objectid<<" and DBID: "<<dbid<<endl;

        RefId backupobjectid;
                if (tdfProcessGroup.getParameter("backup_objectid", backupobjectid) == isNotOk)
                {
                        idaTrackExcept(("Parameter file %s invalid. Reason : \"backup_objectid\" missing ", parFileName.cString()));
            cerr<<"Parameter file invalid. Reason : \"backup_objectid\" missing"<<endl;
                        //exit(1);
                        backupobjectid = -1;
                }

		
		// Werte in die Liste eintragen
		databaseList.addDb(dbid, objectid, backupobjectid);

		// Nächste Subgroup holen
		returnStatus = sysParam.getNextParamGroup("TdfProcessGroup", tdfProcessGroup);
	}


	// Timeout Werte holen
    UShort searchTimeout = 0;
    SysParamGroup timerAndMaxValueGroup("TimerAndMaxValueGroup", true);
    if (sysParam.getFirstParamGroup("TimerAndMaxValueGroup", timerAndMaxValueGroup) == isNotOk)
    {
        idaTrackExcept(("Parameter file %s invalid. Reason : TimerAndMaxValueGroup missing", parFileName.cString()));
        cerr<<"Parameter file invalid. Reason : TimerAndMaxValueGroup missing"<<endl;
    }

    if (timerAndMaxValueGroup.getParameter("search_timeout", searchTimeout) == isNotOk)
    {
		searchTimeout = 10000;
    }
    idaTrackData(("searchTimeout value: %d", searchTimeout));
    cout<<"searchTimeout value: "<<searchTimeout<<endl;


    // Determine own object-ID for classlib object communication.
    long ownOID;

    if (commSlot < 0)
    {
	idaTrackFatal(("No communication slot available"));
        cerr<<"No communication slot available"<<endl;
		exit(1);
    }
    baseOid    += commSlot;
    socketPort += commSlot;
    idaTrackData(("Own object ID: %d", ownOID));
    idaTrackData(("Own socket: %d", socketPort));
    cout<<"Own object ID: "<<ownOID<<endl;
    cout<<"Own socket: "<<socketPort<<endl;
    // Instantiate WebProcess 
	WebProcess webProcess(argc, argv, envp,
						  baseOid,				// eigene OID
						  commSlot,				// SlotId = Offset zu der Basis-ObjektID
						  databaseList,			// Liste der OID's der ansprechbaren Datenbanken
						  socketPort,			// Socket Portnummer
						  searchTimeout		// Timeout für Suchanfragen
						  );

   if ( webProcess. init () == isNotOk )
   {
     idaTrackFatal(("webProcess initialization failed"));
     cerr<<"webProcess initialization failed"<<endl;
     exit(1);
   }

	if (webProcess.getProcessState() == Process::initialized)
	{
		webProcess.run();
	}
	else
	{
	   idaTrackFatal(("webProcess initialization failed"));
      cerr<<"webProcess initialization failed"<<endl;
		exit(1);
	}
    
	// Cleanup
    idaTrackTrace(("End WebProcess"));
    cout<<"End WebProcess"<<endl;

    return 0;
}

// *** EOF ***

