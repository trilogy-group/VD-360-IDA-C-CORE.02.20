//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaWebProcessMain.cc 1.1
//
//   File:      IdaWebProcessMain.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:13:51
//
//   DESCRIPTION:
//     IDA.plus Web process main program.
//
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaWebProcess_cc = "@(#) IdaWebProcessMain.cc 1.1";


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
	std::cerr << "WebProcess started ..." << std::endl;
    
    String parameter1 ( argv[1] );
	// Check parameter
	if ( (argc < 2 ) || ( (argc < 4) && (parameter1 == "115") ) )
	{
		std::cerr << "error: missing arguments" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <CommSlot> <ParFile> "
			 << "[ -tr <TraceLevel> -tf <TraceFile> ]" << std::endl;
		exit(1);
	}

   String parFileName;
   

	// -------------------------------------------------------------------------
	// Als erstes versuchen wir den Trace-Mechanismus zu aktivieren,
	// damit wir sofort Fehler ausgeben k�nnen:
    
	String			traceFileName;
    int				traceLevel = 0;
    char			pidStr[256];

	// Parameter holen, wenn verf�gbar
    TraceMgr::getTraceSettings(argc, argv, traceFileName, traceLevel);
    sprintf(pidStr, "%d", getpid());
	
	// Wenn kein Filename angegeben wurde, nehmen wir den Proze�namen
    if (traceFileName.isEmpty())
	 {
		traceFileName.assign(argv[0]);
		
   	// ... und h�ngen die PID hinten an
		traceFileName = traceFileName + pidStr;
	 }
	 

# ifndef CLASSLIB_03_00
    // Trace-System initialisieren fuer classlib 2.20
    TraceMan::traceInit(traceFileName);
    TraceMan::setTraceLevel(traceLevel);

    if (traceLevel)
    {
      std::cout<<"Startup WebProcess"<<std::endl;
    }
	idaTrackData(("TraceLevel = %d", traceLevel));
# else

    // Trace-System initialisieren fuer classlib 3.0

    TraceManager::getTraceManager ()-> setFileOutput ( traceFileName );
    TraceManager::getTraceManager ( ) -> setCategoryTraceLevel ( "", traceLevel );

# endif

	#ifdef MONITORING
		std::cout << "traceLevel = " << traceLevel << std::endl;
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
		  std::cerr<<"Declaration section IdaDecls invalid. Reason:" << sysParam.getErrorText().cString() << std::endl;
		exit(1);
    }

    // Parameterfile lesen
    std::ifstream parFile(parFileName.cString());
    if (!parFile)
    {
		// Parameterfile konnte nicht ge�ffnet werden
		idaTrackFatal(("Parameter file %s not found", parFileName.cString()));
        std::cerr<<"Parameter file %s not found"<<parFileName<<std::endl;
		exit(1);
    }
    if (sysParam.readAllParams(parFile) == isNotOk)
    {
		// Parameterfile vorhanden, aber syntaktisch nicht korrekt
		idaTrackFatal(("Parameter file %s invalid. Reason: %s", parFileName.cString(), sysParam.getErrorText().cString()));
        std::cerr<<"Parameter file"<<parFileName<<"invalid. Reason: "<<sysParam.getErrorText().cString()<<std::endl;
		exit(1);
    }
    parFile.close();


	// -------------------------------------------------------------------------
	// Basis OID f�r den WebProcess holen. Auf diesen wird sp�ter f�r jede
	// Proze�-Instanz ein entsprechender Offset (aus shared memory) hinzu addiert.
    SysParamGroup webProcessGroup("WebProcessGroup", true);
    if (sysParam.getFirstParamGroup("WebProcessGroup", webProcessGroup) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : WebProcessGroup missing",parFileName.cString()));
          std::cerr<<"Parameter file is invalid!! Reason : WebProcessGroup missing"<<std::endl;
		exit(1);
    }

	// BaseCommOID
    RefId baseOid;
    if (webProcessGroup.getParameter("base_objectid", baseOid) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"base_objectid\" missing", parFileName.cString()));
          std::cerr<<"parameter file invalid. Reason : \"base_objectid\" missing"<<std::endl;
		exit(1);
    }
    idaTrackData(("Base Object-ID for WebProcess: %d", baseOid));
    std::cout<<"Base Object-ID for WebProcess: "<<baseOid<<std::endl;

	// BasePort f�r die Socket Kommunikation
    int socketPort;
    if (webProcessGroup.getParameter("base_socket_port", socketPort) == isNotOk)
    {
        idaTrackFatal(("Parameter file %s invalid. Reason : \"base_socket_port\" missing", parFileName.cString()));
          std::cerr<<"Parameter file invalid. Reason : \"base_socket_port\" missing"<<std::endl;
        exit(1);
    }
      idaTrackData(("Base Socket-Port for WebProcess: %d", socketPort));
      std::cout<<"Base Socket-Port for WebProcess:" <<socketPort<<std::endl;


	// Jetzt brauchen wir noch die OID des TdfClient
    // Extract base object ID for query manager process(es)
    SysParamGroup tdfProcessGroup("TdfProcessGroup", true);
	
	ReturnStatus returnStatus;
	returnStatus = sysParam.getFirstParamGroup("TdfProcessGroup", tdfProcessGroup);
    if (returnStatus == isNotOk)
    {
		// Einen TdfClient brauchen wir mindestens, also Fehler und Abbruch
        idaTrackFatal(("Parameter file %s invalid. Reason : TdfProcessGroup missing", parFileName.cString()));
          std::cerr<<"Parameter file %s invalid. Reason : TdfProcessGroup missing"<<std::endl;
		exit(1);
    }

	// Wir iterieren �ber alle Subgroups von TdfProcessGroup und "sammeln" die OID's
	// der einzelnen TdfClients, damit diese sp�ter unter diesen OID's angesprochen werden k�nnen
	DatabaseList databaseList;
	while (returnStatus == isOk)
	{
		RefId objectid;
		if (tdfProcessGroup.getParameter("objectid", objectid) == isNotOk)
		{
			idaTrackFatal(("Parameter file %s invalid. Reason : \"objectid\" missing", parFileName.cString()));
            std::cerr<<"Parameter file invalid. Reason : \"objectid\" missing"<<std::endl;
			exit(1);
		}
		RefId dbid;
		if (tdfProcessGroup.getParameter("dbid", dbid) == isNotOk)
		{
			idaTrackFatal(("Parameter file %s invalid. Reason : \"dbid\" missing", parFileName.cString()));
            std::cerr<<"Parameter file invalid. Reason : \"dbid\" missing"<<std::endl;
			exit(1);
		}
		idaTrackData(("Found DB with OID: %d and DBID: %d", objectid, dbid));
        std::cout<<"Found DB with OID: "<<objectid<<" and DBID: "<<dbid<<std::endl;

        RefId backupobjectid;
                if (tdfProcessGroup.getParameter("backup_objectid", backupobjectid) == isNotOk)
                {
                        idaTrackExcept(("Parameter file %s invalid. Reason : \"backup_objectid\" missing ", parFileName.cString()));
            std::cerr<<"Parameter file invalid. Reason : \"backup_objectid\" missing"<<std::endl;
                        //exit(1);
                        backupobjectid = -1;
                }

		
		// Werte in die Liste eintragen
		databaseList.addDb(dbid, objectid, backupobjectid);

		// N�chste Subgroup holen
		returnStatus = sysParam.getNextParamGroup("TdfProcessGroup", tdfProcessGroup);
	}


	// Timeout Werte holen
    UShort searchTimeout = 0;
    SysParamGroup timerAndMaxValueGroup("TimerAndMaxValueGroup", true);
    if (sysParam.getFirstParamGroup("TimerAndMaxValueGroup", timerAndMaxValueGroup) == isNotOk)
    {
        idaTrackExcept(("Parameter file %s invalid. Reason : TimerAndMaxValueGroup missing", parFileName.cString()));
        std::cerr<<"Parameter file invalid. Reason : TimerAndMaxValueGroup missing"<<std::endl;
    }

    if (timerAndMaxValueGroup.getParameter("search_timeout", searchTimeout) == isNotOk)
    {
		searchTimeout = 10000;
    }
    idaTrackData(("searchTimeout value: %d", searchTimeout));
    std::cout<<"searchTimeout value: "<<searchTimeout<<std::endl;


    // Determine own object-ID for classlib object communication.
    long ownOID;

    if (commSlot < 0)
    {
	idaTrackFatal(("No communication slot available"));
        std::cerr<<"No communication slot available"<<std::endl;
		exit(1);
    }
    baseOid    += commSlot;
    socketPort += commSlot;
    idaTrackData(("Own object ID: %d", ownOID));
    idaTrackData(("Own socket: %d", socketPort));
    std::cout<<"Own object ID: "<<ownOID<<std::endl;
    std::cout<<"Own socket: "<<socketPort<<std::endl;
    // Instantiate WebProcess 
	WebProcess webProcess(argc, argv, envp,
						  baseOid,				// eigene OID
						  commSlot,				// SlotId = Offset zu der Basis-ObjektID
						  databaseList,			// Liste der OID's der ansprechbaren Datenbanken
						  socketPort,			// Socket Portnummer
						  searchTimeout		// Timeout f�r Suchanfragen
						  );

   if ( webProcess. init () == isNotOk )
   {
     idaTrackFatal(("webProcess initialization failed"));
     std::cerr<<"webProcess initialization failed"<<std::endl;
     exit(1);
   }

	if (webProcess.getProcessState() == Process::initialized)
	{
		webProcess.run();
	}
	else
	{
	   idaTrackFatal(("webProcess initialization failed"));
      std::cerr<<"webProcess initialization failed"<<std::endl;
		exit(1);
	}
    
	// Cleanup
    idaTrackTrace(("End WebProcess"));
    std::cout<<"End WebProcess"<<std::endl;

    return 0;
}

// *** EOF ***

