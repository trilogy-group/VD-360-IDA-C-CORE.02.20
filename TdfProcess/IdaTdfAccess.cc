//CB>-------------------------------------------------------------------
//
//
//   File:      IdaTdfAccess.cc
//   Revision:  1.2
//   Date:      18-NOV-2010 14:20:52
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------
static const char * SCCS_Id_TdfAccess_cc = "@(#) IdaTdfAccess.cc 1.2";


#include <IdaDecls.h>
#include <idatraceman.h>
# ifdef CLASSLIB_03_00
  #include <pcpdispatcher.h>
# else
  #include <dispatcher.h>
# endif
#include <pcpprocess.h>
#include <messagetypes.h>
#include <message.h>
#include <stdlib.h>
#include <syspar.h>
#include <syspargrp.h>
#include <toolbox.h>
#include <pcpstring.h>
#include <tdfderegisterarg.h>
#include <tdfregisterarg.h>
#include <tdfregisterres.h>
#include <tdfresponse.h>
#include <tdfargument.h>
#include <tdfcancelsearcharg.h>
#include <tdftypes.h>
#include <tdfstatusreport.h>
#include <tdferrorarg.h>
#include <tdsrequest.h>
#include <tdsresponse.h>
#include <modifyrequest.h>
#include <modifyresponse.h>
#include <osaapidefs.h>
#include <decstring.h>
#include <pcpostrstream.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <fstream.h>
#include <iostream.h>


#include <IdaTypes.h>
#include <IdaTdfChannel.h>
#include <IdaStringToEnum.h>
#include <IdaCpd.h>
#include <IdaRequestContainer.h>

// Das folgende Makro ist notwendig um unter AIX die XERCES header mit einbinden zu können.
// Diese benutzen leider auch einen boolean Typ der mit dem der STL kollidiert
// In allen Dateien des OSA-Treibers darf nur der Typ "Bool" verwendet werden, damit es
// kein Durcheinander gibt !
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>


//just for test purposes
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/BinMemInputStream.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <IdaDomErrorHandler.h>

#include <IdaTdfAccess.h>




////////////////////////////////////////////////////////////////////////////////////////////////////
//

#ifdef ALLOW_STDOUT
	#define MONITORING
#endif



#define DELETE(p) if(p){delete p;p=0;}
#include <IdaXmlTags.h>

class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        delete [] fUnicodeForm;
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
     		return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Konstruktor
//
TdfAccess::TdfAccess(const ObjectId&  				myId,
					 Long				dbId,
					 const ObjectId&	serverService,
					 const String&		service,
					 const String&		applName,
					 UShort				nChannels,
					 const String&		osaTick,
					 ULong				regTimer,
					 ULong				searchTimeout,
					 long				maxReg,
					 Bool				regTest,
					 SesConfig			sesConfigPars)
:
#ifdef CLASSLIB_03_00
	TdfClient					(myId, serverService, worldScope),
# else
	TdfClient                                       (myId, serverService, serverService, worldScope),
# endif
	status						(start),	// Startzustand
	pReporterClient				(0),
	myObjectId					(myId),
	databaseId					(dbId),
	serviceName					(service),
	applicationName				(applName),
	numberOfChannels			(nChannels),
	osaTicketFileName			(osaTick),
	registerTimer				(regTimer > 0 ? regTimer : 180000),
	searchTimeOut				(searchTimeout > 0 ? searchTimeout : 10000),
	maxRegistrationAttempts		(maxReg > -2 ? maxReg : 10),
	regressionTest				(regTest),
	sesConfig					(sesConfigPars),
	applicationId				(0),
	referenceId					(0),
	searchId					(0),
	curOperationId				(1),
	registrationRetryCounter	(0),
	registerTimerId				(0),
	retryTimerFlag				(false),
	registerTimerFlag			(false)

{
	countryCode = DecString(databaseId).getString();
	
	
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::TdfAccess(...)");
#endif
	EVENT(myObjectId, iDAMinRepClass, 209, countryCode);


	sprintf(dbIdString, "[%d]: ", databaseId);


	#ifdef MONITORING
		cout << dbIdString << "applicationName = " << applicationName << endl;
		cout << dbIdString << "myObjectId      = " << myObjectId.getIdentifier() << endl;
		cout << dbIdString << "databaseId      = " << databaseId << endl;
    	cout << dbIdString << "serverControl   = " << serverControl.getIdentifier() << endl;
    	cout << dbIdString << "serverService   = " << serverService.getIdentifier() << endl;
		cout << dbIdString << "ses mode        = ";
		switch (sesConfig.enumMode)
		{
			case SesConfig::NONE :      cout << szSesConfigModeNONE;    	break;
			case SesConfig::DEFAULT :   cout << szSesConfigModeDEFAULT; 	break;
			case SesConfig::NORMAL :    cout << szSesConfigModeNORMAL;  	break;
		}
		cout << endl;
		if (sesConfig.enumMode == SesConfig::DEFAULT)
		{
			cout << dbIdString << "default user    = " << sesConfig.stDefaultUser << endl;
			cout << dbIdString << "default pwd     = " << sesConfig.stDefaultPwd << endl;
		}
	#endif
	
	
	ReturnStatus returnStatus = isOk;
	
	// Initialize OSA ticket
	// Hier wird ein Ticket aus einer Datei geladen, das Default-Ticket
	// Verwendung findet, wenn SES deaktiviert ist
	if (prepareOsaTicket() == isNotOk)		returnStatus = isNotOk;

	// Lokale Tdf-Adresse ermitteln
	if (initLocalTdfAddress() == isNotOk)	returnStatus = isNotOk;

    // Initialize the XML4C2 system
	// Notwendig, damit der Xerces-XML-Parser benutzt werden kann
	if (initXMLPlatform() == isNotOk)		returnStatus = isNotOk;

	// Create new XercesDOMParser Instance
	parser = new XercesDOMParser();

	
	// SES Manager initialisieren
	// Ermöglicht den Zugriff auf den SES-Prozess über eine 
	// Singleton-Instanz
    //if (initSesMgr() == isNotOk)			returnStatus = isNotOk;

	// Status der Initialisierungen anzeigen
	if (returnStatus == isNotOk)
	{
		idaTrackExcept(("TdfAccess::TdfAccess: Initialization NOT successful"));
		changeStatus(initializationFailed);
	}
	else
	{
		idaTrackData(("TdfAccess::TdfAccess: Initialization successful"));
		changeStatus(initialized);
	}

	// Hier wird der Registrierungs-Mechanismus (inkl. Retry) angestoßen
	startRegistration();

	// Initialisierung einiger Variablen für die Performance-Messung
	requestCounter = 0;
	requestDauer = 0;
	requestDauerMin = 1000000;
	requestDauerMax = 0;
	responseCounter = 0;
	responseDauer = 0;
	responseDauerMin = 1000000;
	responseDauerMax = 0;
	dbCounter = 0;
	dbDauer = 0;
	dbDauerMin = 1000000;
	dbDauerMax = 0;

}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Destruktor
//
TdfAccess::~TdfAccess()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("Destructor TdfAccess(...)");
#endif
	terminateXMLPlatform();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Shutdown handler
//
Void TdfAccess::shutdown(ObjectId& myId)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::shutdown(...)");
#endif
	EVENT(myObjectId, iDAMinRepClass, 210, countryCode);
	
	idaTrackExcept(("Shutdown request received"));

//	ComMan::getDispatcher()->stopDispatchForever();
//	ComMan* comMan = Process::getComMan();          
//	comMan->checkOut(myId);
//	comMan->dispatcherCheckOut(myId);

	cancelRegisterTimer();
	sendDeRegisterRequest();
	cancelSendRetryTimer();

	checkOut();

	ReturnStatus rs = Process::shutdownComplete(myId);

}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode kontrolliert zentral alle Änderungen des internen Zustandes.
//	Der interne Zustand zeigt an, ob das System ordnungsgemäß initialisiert
//	und registriert wurde.
//	Die Zustands-Variable "status" sollte also in keinem Fall direkt gesetzt
//  werden. Illegale Zustandsübergänge werden hier erkannt und angezeigt.
//
Void TdfAccess::changeStatus(TdfAccessStatus newStatus)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::changeStatus(...)");
#endif	
	
	// Nur echte Änderungen behandeln
	if (status == newStatus) return;
	
	// Alten Zustand sichern
	TdfAccessStatus oldStatus = status;
	
	// Flag, das anzeigt, ob eine Zustandsänderung stattgefunden hat
	int changed = 0;

	switch (status)
	{
		// -----------------------------------------------------------
		case start :
			if (newStatus == initializationFailed || newStatus == initialized)
			{ status = newStatus; changed = 1; }
			break;
		// -----------------------------------------------------------
		case initializationFailed :
			// Nichts geht mehr !
			break;
		// -----------------------------------------------------------
		case initialized :
			if (newStatus == tryingToRegister)
			{ status = newStatus; changed = 1; }
			break;
		// -----------------------------------------------------------
		case tryingToRegister :
			if (newStatus == registered || newStatus == unAvailable)
			{ status = newStatus; changed = 1; }
			break;
		// -----------------------------------------------------------
		case unAvailable :
			if (newStatus == initialized)
			{ status = newStatus; changed = 1; }
			break;
		// -----------------------------------------------------------
		case registered :
			if (newStatus == initialized || newStatus == tryingToRegister)
			{ status = newStatus; changed = 1; }
			break;
	}

	if (changed == 1)
	{
		idaTrackData(("Internal status changed : %d --> %d", oldStatus, newStatus));
	}
	else
	{
		idaTrackExcept(("Illegal status changed was attempted : %d --> %d", oldStatus, newStatus));
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
ReturnStatus TdfAccess::initLocalTdfAddress()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::initLocalTdfAddress(...)");
#endif

	Char tmpHostName[256];
	gethostname(tmpHostName, 256);

	idaTrackData(("initLocalTdfAddress(): gethostname = %s", tmpHostName));

	ULong localIpAddress = resolveClientIp(tmpHostName);
	if (localIpAddress)
	{
		localTdfAddress.setIpAddress(localIpAddress);
	}
	else
	{
		localTdfAddress.setObjectId(myObjectId.getIdentifier());
	}

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
ULong TdfAccess::resolveClientIp(String clientIp)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::resolveClientIp(...)");
#endif

	idaTrackData(("resolveClientIp for IpAddress : %s", clientIp.cString()));

	struct hostent*  host;
	static struct in_addr saddr;

	// First try it as "aaa.bbb.ccc.ddd"
	saddr.s_addr = inet_addr(clientIp.cString());
	if (saddr.s_addr != -1)
	{
                // 2008-01-17, cp
		return ntohl(saddr.s_addr);
	}

	// ... or try it as name
	host = gethostbyname(clientIp.cString());
	if (host != NULL)
	{
		struct in_addr *inAddr;
		inAddr =  (struct in_addr *) *host->h_addr_list;
                // 2008-01-17, cp
		return ntohl(inAddr->s_addr);
	}

	idaTrackExcept(("resolveClientIp() failed !"));

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Hier wird ein universelles OsaTicket aus einer Datei eingelesen
// 
ReturnStatus TdfAccess::prepareOsaTicket()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::prepareOsaTicket(...)");
#endif	
	
	ifstream osaTicketFile(osaTicketFileName.cString());
	if (osaTicketFile)
	{
		idaTrackData(("prepareOsaTicket(): Reading osaTicket file."));
		osaTicketFile >> osaTicket;
		osaTicketFile.close();
		idaTrackData(("Ticket:\n%s", osaTicket.dumpString()));
		return isOk;
	}
	else
	{
		idaTrackFatal(("prepareOsaTicket(): Cannot open osaTicket file \"%s\" !",
			  osaTicketFileName.cString()));
		return isNotOk;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Bevor der Xerces XML-Parser benutzt werden kann, muß die XML-Platform
//	initialisiert werden
//
ReturnStatus TdfAccess::initXMLPlatform()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::initXMLPlatform(...)");
#endif

	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		idaTrackFatal(("XMLPlatformUtils::Initialize() failed !"));
		#ifdef MONITORING
			cout << dbIdString << "XMLPlatformUtils::Initialize() failed !" << endl;
		#endif
		
		return isNotOk;
	}

//    impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));
//	static XMLTransService::Codes resValue;
//    idaTrackData(("XMLPlatformUtils::Initialize() ok")); 

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode bereitet den egistrierungs-Mechanismus vor und startet ihn
//
ReturnStatus TdfAccess::startRegistration()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::startRegistration(...)");
#endif	
	
	// Wenn die Registrierung bereits läuft, wird nicht gemacht
	if (getStatus() == tryingToRegister) return isOk;

	// Status ändern
	changeStatus(initialized);
	changeStatus(tryingToRegister);
	
		
	// Wir setzen den Registrierungs-Zähler zurück
	registrationRetryCounter = 0;

	EVENT(myObjectId, iDAMinRepClass, 211, "");

	// Nachricht senden
	return sendRegisterRequest();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Hier wird ein Request-Objekt für die Registrierung aufgebaut und gesendet. Anschießend wird
//	der Registrierungs-Timer gestartet. Die Überprüfung der maximalen Registrier-Wiederholungen
//	wird ebenfalls an dieser Stelle überwacht und behandelt.
//
ReturnStatus TdfAccess::sendRegisterRequest()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::sendRegisterRequest(...)");
#endif

	#ifdef MONITORING
		cout << dbIdString << "sendRegisterRequest() " << endl;
	#endif
	
	// -------------------------------------------------------------------------
	// Der richtige Status muß vorliegen
	if (getStatus() != tryingToRegister) return isNotOk;


	if ((registrationRetryCounter >= maxRegistrationAttempts) && (maxRegistrationAttempts !=-1) )
	{
		idaTrackFatal(("Max registration (#%d) count exceeded", maxRegistrationAttempts));
		ALARM(myObjectId, iDAMinRepClass, 4, countryCode);

		#ifdef MONITORING
			cout << dbIdString << "registrationRetryCounter >= maxRegistrationAttempts" << endl;
			cout << dbIdString << "registration attempt stopped ! *** SERVICE NOT AVALABLE ***" << endl;
		#endif

		changeStatus(unAvailable);

		return isNotOk;
	}
	else
	{
		// RegisterCounter inkrementieren
                ++registrationRetryCounter;
		if (maxRegistrationAttempts == -1 ) {
		  idaTrackData(("max registration #%d, registration attempt #%d", maxRegistrationAttempts, registrationRetryCounter));
                }
                else {
		  idaTrackData(("registration attempt #%d", registrationRetryCounter));
                }
	}

	// ---------------------------------------------------------------------
	// OsaLimits::getInstance()->setOsaComErrorCode(0);
	TdfRegisterArg tdfRegisterArg;
	tdfRegisterArg.setSourceApplication(applicationName);
	tdfRegisterArg.setRequestedChannels(numberOfChannels);
	tdfRegisterArg.setOsaTicket(osaTicket);
	tdfRegisterArg.setDataFormat(tdsDataFormat);
	tdfRegisterArg.setReference(getNextReferenceId());
	// Validate data
	if (tdfRegisterArg.areMandatoryItemsSet() != true)
	{
		idaTrackFatal(("Error: TdfRegisterArg not all mandatory Items are set!"));

		#ifdef MONITORING
			cout << dbIdString << "tdfRegisterArg.areMandatoryItemsSet() != true" << endl;
		#endif

		return isNotOk;
	}

	// ---------------------------------------------------------------------
	// Send register request to TDS server
    idaTrackData(("We are before registerRequest!!"));
	if (registerRequest(tdfRegisterArg) == isOk)
	{
		idaTrackData(("registerRequest() ok !"));
	}
	else
	{
		OsaComError error = getLastError();
		#ifdef MONITORING
			cout << "registerRequest() failed: " << error.getErrorCode() << "|"
				 << error.getErrorSource() << "|" 
				 << error.getErrorText() << endl;
		#endif
		idaTrackExcept(("registerRequest() failed ! %s", error.getErrorText()));
		PROBLEM(myObjectId, iDAMinRepClass, 120, "");

		#ifdef MONITORING
			cout << dbIdString << "registerRequest() failed !" << endl;
		#endif
	}

	// Start registration timer
	return startRegisterTimer();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Vorbereitung und Durchführung der De-Registrierung von der Datenbank
//
ReturnStatus TdfAccess::sendDeRegisterRequest()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::sendDeRegisterRequest(...)");
#endif

	TdfDeRegisterArg tdfDeRegisterArg;
	tdfDeRegisterArg.setApplicationId(applicationId);
	if (deRegisterRequest(tdfDeRegisterArg) == isNotOk)
	{
		idaTrackData(("Error sending deRegisterRequest!"));
		return isNotOk;
	}

	EVENT(myObjectId, iDAMinRepClass, 213, "");
	idaTrackData(("deRegisterRequest sent!"));

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "TdfClient".
//
//	Diese Methode wird vom Framework der ODAAPI aufgerufen, als Antwort auf eine Registrierung.
//
Void TdfAccess::registerConfirmation(const TdfRegisterRes& tdfRegisterRes)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::registerConfirmation(...)");
#endif
	EVENT(myObjectId, iDAMinRepClass, 212, "");


	// -------------------------------------------------------------------------
	// Check register reference ID
	UShort refId = 0;
	if (tdfRegisterRes.getRegisterRef(refId) == isNotOk)
	{
		idaTrackExcept(("Error: tdfRegisterRes.getRegisterRef(refId) failed !"));
		return;
	}
	if (refId != getActualReferenceId())
	{
		idaTrackExcept(("Error: refId != getActualReferenceId()"));
		return;
	}

	// -------------------------------------------------------------------------
	// Cancel registration timer
	cancelRegisterTimer();


	// -------------------------------------------------------------------------
	// Check TDS response error code
	if (tdfRegisterRes.isErrorArgSet())
	{
		// Der TdfServer hat zwar geantwortet, aber irgendetwas ist schief gegangen
		TdfErrorArg errorArg;
		errorArg = tdfRegisterRes.getErrorArg();
		idaTrackExcept(("registerConfirmation(): ErrorText = %s", errorArg.getErrorText()));
		#ifdef MONITORING
			cout << "registerConfirmation(): ErrorText = " << errorArg.getErrorText() << endl;
		#endif

		// Wir versuchen eine erneute Registrierung
		// (aber nicht sofort, sonst würde der Server mit Nachrichten überflutet werden)
		startRegisterTimer();
		return;
	}

	// Extract relevant data
	tdfRegisterRes.getApplicationId(applicationId);
	UShort confirmedChannels = 0;
	tdfRegisterRes.getConfirmedChannels(confirmedChannels);
	if (confirmedChannels != numberOfChannels)
	{
		idaTrackExcept(("Requested %d channels, but %d channels have been confirmed", 
					 numberOfChannels, confirmedChannels));
	}
	

	idaTrackData(("TDS applId:%d, #channels:%d", applicationId, confirmedChannels));
	#ifdef MONITORING
		cout << dbIdString
			 << "Number of registered channels = "
			 << numberOfChannels
			 << endl;
	#endif

	// Initialize all channels
	channelMgr.setNoOfChannels(confirmedChannels);


	// Setup status
	changeStatus(registered);


	idaTrackData(("Registration of TdfAccess with OID <%d> successful", myObjectId.getIdentifier()));

}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "TdfClient".
//
//  Diese Methode wird vom TDF-Server aufgerufen wenn er den Client (also uns)
//	deregistriert
//
Void TdfAccess::deRegisterReportIndication(const TdfDeRegisterReport & arg)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::deRegisterReportIndication(...)");
#endif	
	PROBLEM(myObjectId, iDAMinRepClass, 215, "");


	idaTrackExcept(("deRegisterReportIndication() received"));

	#ifdef MONITORING
		cout << dbIdString
			 << "deRegisterReportIndication message received,  "
			 << PcpTime().formatTime().cString()
			 << endl;
	#endif

	// Wir stoppen einen evtl. laufenden Registrierungs-Timer ...
	cancelRegisterTimer();

	// ... und weisen alle neuen Anfragen zurück mit:
	changeStatus(initialized);

	// Anzahl verfügbarer Channels auf Null setzen
//	channelMgr.setNoOfChannels(0);			// !!! eigentlich nur, wenn alle wartenden Requests gelöscht wurden

	
	// -------------------------------------------------------------------------
	// Und wir versuchen eine erneute Registrierung
	idaTrackData(("TdfAccess::deRegisterReportIndication starts registration() "));
	
	// Ausgehend von der Annahme, das die Deregistrierung nur kurzzeitig ist, wird hier
	// eine erneute Registrierung angstoßen
	startRegistration();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Startet den Registertimer. Die Methode ist Flag-gesteuert und kann somit auch mehrfach 
//	hintereinander aufgerufen werden, ohne das es zu Störungen kommt
//
ReturnStatus TdfAccess::startRegisterTimer()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::startRegisterTimer(...)");
#endif
	
	if (!registerTimerFlag)
	{
		// Starten eines "Einmal"-Timers
		if (startTimer(registerTimerId, registerTimer, PcpTimerInterface::once) == isNotOk)
		{
			idaTrackFatal(("Could not start register timer"));
			#ifdef MONITORING
				cout << dbIdString << "startRegisterTimer() failed" << endl;
			#endif
			return isNotOk;
		}
		idaTrackData(("register timer started"));
		registerTimerFlag = true;
	}
	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Löscht den evtl. laufenden Register-Timer (auch mehrfach aufrufbar)
//	
ReturnStatus TdfAccess::cancelRegisterTimer()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::cancelRegisterTimer(...)");
#endif
	if (registerTimerFlag)
	{
		if (stopTimer(registerTimerId) == isNotOk)
		{
			idaTrackExcept(("Register timer could not be canceled"));
			return isNotOk;
		}
		idaTrackData(("register timer canceled"));
		registerTimerFlag = false;
	}
	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Startet einen Timer für die zeitgesteuerte Reaktivierung der Methode "processSendQueue", 
//	welche die Liste der Requests (requestPool) abarbeitet.
//	(mehrfach aufrufbar, nicht nachtriggernd,
//	d.h. durch Mehrfachaufruf verlängert sich die Wartezeit nicht)
//
ReturnStatus TdfAccess::startSendRetryTimer()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::startSendRetryTimer(...)");
#endif
	if (!retryTimerFlag)
	{
		if (startTimer(retryTimerId, 500, PcpTimerInterface::once) == isNotOk)
		{
			idaTrackFatal(("Could not start send retry timer"));
			#ifdef MONITORING
				cout << dbIdString << "startSendRetryTimer() failed" << endl;
			#endif
			return isNotOk;
		}
		idaTrackData(("send retry timer started"));
		retryTimerFlag = true;
	}
	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Inverse Methode zu "startSendRetryTimer()", mit gleichen Eigenschaften
//
ReturnStatus TdfAccess::cancelSendRetryTimer()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::cancelSendRetryTimer(...)");
#endif	
	if (retryTimerFlag)
	{
		if (stopTimer(retryTimerId) == isNotOk)
		{
			idaTrackExcept(("Send retry timer could not be canceled"));
			return isNotOk;
		}
		idaTrackData(("send retry timer canceled"));
		retryTimerFlag = false;
	}
	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "TdfClient".
//
//
Void TdfAccess::statusReportIndication(const TdfStatusReport& arg)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::statusReportIndication(...)");
#endif
	
	#ifdef MONITORING
		cout << dbIdString << "statusReportIndication received" << endl;
	#endif
	
	TdfStatusReport::Status dbStatus = arg.getStatus();
	switch (dbStatus)
	{
		case TdfStatusReport::available :
			idaTrackData(("DB Server available"));
			EVENT(myObjectId,iDAMinRepClass,215,countryCode);
			break;
		case TdfStatusReport::notAvailable :
			idaTrackExcept(("DB Server *NOT* available"));
			ALARM(myObjectId, iDAMinRepClass, 6, countryCode);
			// Wie im Handbuch beschrieben deregistrieren wir uns
			sendDeRegisterRequest();
			changeStatus(initialized);
			startRegistration();
			break;
	}
}


Void TdfAccess::modifyConfirmation( const TdfResponse & arg )
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::modifyConfirmation(...)");
#endif
  searchConfirmation( arg );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "TdfClient".
//
//	Für jeden Request (Suchanfrage) an einen Datenbank-Server (NDIS) wird diese Methode vom
//	OSAAPI-Framework aufgerufen, um die Antwort zurück zu liefern
//
Void TdfAccess::searchConfirmation(const TdfResponse& tdfResponse)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::searchConfirmation(...)");
#endif
	{
		PcpTime time;
		idaTrackData(("*** Response received from Gateway   at %d:%d:%d,%d",
				   time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
	}

	#ifdef MONITORING
		timeStamp2 = Time();
	#endif
	
	EVENT(myObjectId, iDAMinRepClass, 218, "");

	idaTrackData(("TdfResponse:\n%s", tdfResponse.dumpString()));

	// -------------------------------------------------------------------------
	// Zuerst muß die Empfänger-OID aus dem zugehörigen Request-Objekt
	// ermittelt werden:
	RequestContainer	requestContainer;
	UShort				requestId	= tdfResponse.getReference();
	// wir holen die Request-Daten aus der Request-Liste
	if (!requestPool.getRequest(requestId, requestContainer))
	{
		// Wenn der Request nicht gefunden wurde ...

		idaTrackExcept(("TdfAccess::searchConfirmation: requestPool.getRequest failed"));

		#ifdef MONITORING
			cout << dbIdString << "requestPool.getRequest() failed" << endl;
		#endif
		
		// Eigentlich müßten wir ein Fehler-Dokument zurück senden.
		// Da wir den Adressaten aber nicht ermitteln konnten,
		// können wir hier nicht tun !

		return;
	}

	TdfArgument*	tmpTdfArg	= requestContainer.getTdfArgument();
	UShort			channel		= requestContainer.getChannel();
	ObjectId		objectId	= requestContainer.getSenderOid();
	RefId			timerId		= requestContainer.getTimerId();
	PcpTime			birth		= requestContainer.getBirth();



	#ifdef MONITORING
		{
			++dbCounter;
			RelTime delta = PcpTime() - birth;
			ULong dauer = delta.inMilliSeconds();
			cout << dbIdString << "(" << requestId << ")DB searchtime : " << dauer << " [ms]" << endl;
			dbDauer += dauer;
			if (dauer < dbDauerMin) dbDauerMin = dauer;
			if (dauer > dbDauerMax) dbDauerMax = dauer;
			if (dbCounter >= 100)
			{
				cout << dbIdString << "\t\t\t\t\t\tDatabase min/avg/max [ms] : " 
					 << dbDauerMin << "/"
					 << dbDauer / dbCounter << "/"
					 << dbDauerMax
					 << endl;
				dbDauerMin = 1000000;
				dbDauerMax = 0;
				dbDauer = 0;
				dbCounter = 0;
			}
		}
	#endif

	// --------------------------------------------------------------------------
	// Resourcen freigeben:
	// Timer löschen
	stopTimer(timerId);
	// Channel freigeben
	channelMgr.releaseChannel(channel);
	// TdfArgument-Objekt löschen !
	tmpTdfArg->reset();
	DELETE(tmpTdfArg);
	// Request aus der Liste löschen
	requestPool.removeRequest(requestId);


	String xmlDocType;							// für den Dokument-Typ Bezeichner
	String responseString;						// für die XML-gewandelten Antwortdaten
	responseString.setReallocBlockSize(0x8000); // verhindert zu haefiges Reallozieren

	// -------------------------------------------------------------------------
	// abhaengig vom DataFormat entscheiden wir, was wir der Payload des
	// TdfResponse-Objektes entnehmen koennen

	TdsResponse	tdsResponse;
	tdsResponse.reset();
	ModifyResponse	modifyResponse;
	modifyResponse.reset();
	char modifyRespOpen [] = "<OsaModifyResponse ";  // an attribute will be added
	char modifyRespClose [] = "</OsaModifyResponse>";
	const char * rootTagOpen = tagoOsaResponse.cString();
	const char * rootTagClose = tagcOsaResponse.cString();

	TdfErrorArg	tdfErrorArg;

	switch (tdfResponse.getDataFormat())
	{
		// ---------------------------------------------------------------------
		// Alles ist gut gegangen, und ein TdsResponse Objekt wurde geliefert
		case tdsDataFormat:
			// 1.: TdsResponse-Objekt holen
			if (tdfResponse.isDataSet())
			{
				if (tdfResponse.getData(tdsResponse) == isNotOk)
				{
					idaTrackFatal(("tdfResponse.getData(tdsResponse) failed"));
					// Die Daten des TdfResponse-Objektes konnten nicht geholt werden
				}
				idaTrackData(("TdsResponse received:\n%s", tdsResponse.dumpString()));
			}
			else
			{
				idaTrackExcept(("tdfResponse.isDataSet() was false"));
				// Das TdfResponse-Objekt enthaelt keine Daten
				return;
			}

			// TdsResponse-Objekt umwandeln
			convTdsResponseToXml(tdsResponse, responseString, xmlDocType);


			break;

		// ---------------------------------------------------------------------
		// Alles ist gut gegangen, und ein ModifyResponse Objekt wurde geliefert
		case tdsModifyDataFormat:
			rootTagOpen = modifyRespOpen;
			rootTagClose = modifyRespClose;

                        if (tdfResponse.isDataSet())
                        {
                                if (tdfResponse.getData(modifyResponse) == isNotOk)
                                {
                                        idaTrackFatal(("tdfResponse.getData(modifyResponse) failed"));
                                        // Die Daten des TdfResponse-Objektes konnten nicht geholt werden
                                }
                                idaTrackData(("ModifyResponse received:\n%s", modifyResponse.dumpString()));
                        }
                        else
                        {
                                idaTrackExcept(("modifyResponse.isDataSet() was false"));
                                // Das ModifyResponse-Objekt enthaelt keine Daten
                                return;
                        }

                        // ModifyResponse-Objekt umwandeln
                        convModifyResponseToXml(modifyResponse, responseString, xmlDocType);

			break;

		// ---------------------------------------------------------------------
		// Bei der Abfrage ist ein TDF-Fehler aufgetreten
		case errorArgDataFormat:
			tdfResponse.getData(tdfErrorArg);
			idaTrackData(("TdfErrorArg received:\n%s", tdfErrorArg.dumpString()));
			handleErrorArg(tdfErrorArg);

			// kein break !

		// ---------------------------------------------------------------------
		default:
			// Protokoll-Ausgabe

			// Fehlerbeschreibung generieren (in XML)
			convTdfErrorArgToXml(tdfErrorArg, responseString, xmlDocType);

			break;

	}

	// -------------------------------------------------------------------------
	// Das Ergebnis der NDIS-Abfrage wird in einen XML-String gewandelt
	// Der String wird in folgender Variablen aufgebaut:
	String xmlString;
	xmlString.setReallocBlockSize(0x8000); // verhindert zu haefiges Reallozieren
	
	// XML-Dokument beginnen
	// Zeichensatz Encoding / Version usw. angeben

	xmlString.cat(tagHEADER);

	xmlString.cat(rootTagOpen);		//   '<OsaResponse'
	xmlString.cat(" type=\"");			// + ' type="'
	xmlString.cat(xmlDocType);			// + 'residential'		(z.B.)
	xmlString.cat("\">\n");				// + '">' + Zeilenumbruch


	// TdfDaten einbauen

	if (tdfResponse.isApplicationIdSet())
		createXmlNode(xmlString, tagApplicationId, (Int)tdfResponse.getApplicationId());

	if (tdfResponse.isSearchRefSet())
		createXmlNode(xmlString, tagSearchRef, (ULong)tdfResponse.getSearchRef());

	if (tdfResponse.isSourceIdSet())
	{
		ULong  sourceId  = tdfResponse.getSourceId();
		createXmlNode(xmlString, tagSourceId, sourceId);
	}

	if (tdfResponse.isLinkContextSet())
	{
		String result;
		String byteString;
		tdfResponse.getLinkContext(byteString);
		dataToHexString(byteString, result);
		createXmlNode(xmlString, tagTdfLinkContext, result.cString());
	}

	// TdsDaten anhängen
	xmlString.cat(responseString);

	// XML-Dokument abschließen
	xmlString.cat(rootTagClose);

	// -> der XML-String ist fertig

	#ifdef MONITORING
		{	// DEBUG
	//		ofstream ofs("../test/TdsResponse.xml");
	//		ofs << xmlString;
	//		ofs.close();
		}
	#endif


	// -------------------------------------------------------------------------
	// XML-String an den WebProzess senden
	sendBigString(
		objectId.getIdentifier(),
		requestId,
		myObjectId.getIdentifier(),
		Types::XML_RESPONSE_MSG,
		xmlString);


	#ifdef MONITORING
		{
			++responseCounter;
			RelTime delta = Time() - timeStamp2;
			ULong dauer = delta.inMilliSeconds();
			cout << dbIdString << "(" << requestId << ")Response      : " << dauer << " [ms]" << endl;
			responseDauer += dauer;
			if (dauer < responseDauerMin) responseDauerMin = dauer;
			if (dauer > responseDauerMax) responseDauerMax = dauer;
			if (responseCounter >= 100)
			{
				cout << dbIdString << "\t\t\t\t\t\tResponse min/avg/max [ms] : " 
					 << responseDauerMin << "/"
					 << responseDauer / responseCounter << "/"
					 << responseDauerMax
					 << endl;
				responseDauerMin = 1000000;
				responseDauerMax = 0;
				responseDauer = 0;
				responseCounter = 0;
			}
		}
	#endif

	
		
	// -------------------------------------------------------------------------
	// Der Response ist bearbeitet und ein ein Channel ist frei geworden
	// Wir können nun versuchen eine Request aus der "waitingList" abzusenden.
//	processSendQueue();
		
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn ein TdfErrorArg vom Server geschickt wird müssen wir untersuchen, ob
//	z. B. die Registrierung wiederholt werden muß
// 
Void TdfAccess::handleErrorArg(TdfErrorArg tdfErrorArg)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::handleErrorArg(...)");
#endif	
	
	if (tdfErrorArg.isErrorClassSet())
	{

		VisibleString vsSource;
		VisibleString vsClass;
		enumToString(tdfErrorArg.getErrorSource(), vsSource);
		enumToString(tdfErrorArg.getErrorClass(), vsClass);

		idaTrackData(("TdfErrorArg: ErrorSource = %s, ErrorClass = %s", vsSource.dumpString(), vsClass.dumpString()));

		#ifdef MONITORING
			cout << dbIdString << "ErrorClass = " 
				 << vsClass.dumpString() << endl;
		#endif

		switch (tdfErrorArg.getErrorClass())
		{
			case TdfErrorArg::undetailedError:
				// ...
				break;
			case TdfErrorArg::protocol :
				// ...
				break;
			case TdfErrorArg::connection :
				// ...
				break;
			case TdfErrorArg::systemCallFailed :
				// ...
				break;
			case TdfErrorArg::tableOverflow :
				// ...
				break;
			case TdfErrorArg::noSuchConverterAvailable :
				// ...
				break;
			case TdfErrorArg::applicationIdUnknown :
				// ...
				changeStatus(initialized);
				startRegistration();
				break;
			case TdfErrorArg::registrationFailure :
				// ...
				changeStatus(initialized);
				startRegistration();
				break;
			case TdfErrorArg::corruptData :
				// ...
				break;
			case TdfErrorArg::shutdownRequested :
				// ...
				break;
			case TdfErrorArg::timeout :
				// ...
				break;
			case TdfErrorArg::serverInDryMode :
				// ...
				break;
			case TdfErrorArg::deRegisterForcedByAdministration :
				// ...
				break;
			case TdfErrorArg::permissionDenied :
				// ...
				break;
			case TdfErrorArg::congestion :
				// ...
				break;
			case TdfErrorArg::permanentOutOfChannels :
				// ...
				break;
			case TdfErrorArg::functionNotSupported :
				// ...
				break;
			case TdfErrorArg::contextNotAvailable :
				// ...
				break;
			case TdfErrorArg::compatibilityError :
				// ...
				break;
			case TdfErrorArg::ticketValidationFailed :
				// ...
				break;
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "TdfClient".
//
//	Diese Methode erfüllt folgende Aufgaben:
//
//		1. Aufbau eines RequestContainer-Objektes mit allen für den Request
//		   nötigen Context-Informationen
//		2. Eintragen des RequestContainer-Objektes in eine Request-Queue
//		3. Aufruf der Behandlungsroutine für die Abwicklung des Sendevorgangs
//
Void TdfAccess::applicationMessageBox(Message& message)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::applicationMessageBox(...)");
#endif


	// -------------------------------------------------------------------------
	// Anhand des Typs der Nachricht verteilen:
	switch (message.getMsgType())
	{
		case Types::XML_REQUEST_MSG :
			handleRequest(message);
			break;
		default:
			// Hier wurde eine Nachricht mit falschem Typ empfangen
			idaTrackExcept(("applicationMessageBox(): TdfProcess received unknown requesti %ld", message.getMsgType()));
			#ifdef MONITORING
				cout << dbIdString << "unknown request received" << endl;
			#endif
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode behandelt einen Request gleich welcher Art (Login, Logout, Such-Request, ...) der
//	von einem WebProcess kommt. Ihre Aufgabe ist, ein RequestContainer-Objekt anzulegen und
//	mit allen benötigten Informationen zu füllen um den den Request "sende-fertig" in die Sende-Liste
//	(= requestPool) einzutragen. Dazu gehört auch die Wandlung vom XML- in das proprietäre TDS-Format.
//	Das eigentliche Senden (bzw. zuvor Authentifizieren) übernimmt dann die Methode processSendQueue.
//
Void TdfAccess::handleRequest(Message& message)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::handleRequest(...)");
#endif	

	{
		PcpTime time;
		idaTrackData(("*** Request from WebProcess received at %d:%d:%d,%d",
				   time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
	}


	idaTrackData(("request received from WebProcess"));
	EVENT(myObjectId, iDAMinRepClass, 205, "");

	#ifdef MONITORING
		timeStamp1 = PcpTime();
	#endif

	UShort requestId = message.getMsgRef();
	String xmlString;
	String errorText;
	
		
	// ---------------------------------------------------------------------
	// Zum Speichern des gesamten Request-Kontextes legen wir ein
	// RequestContainer-Objekt an :
	RequestContainer requestContainer(requestId, message.getSourceOid());


	// -------------------------------------------------------------------------
	// Die payload aus der Nachricht holen
	if (message.getData(xmlString) == isNotOk)
	{
		idaTrackFatal(("could not retreive data from message"));
		#ifdef MONITORING
			cout << dbIdString << "getData() failed" << endl;
		#endif
		refuseRequest(requestContainer, "getDataError", "could not retreive data from message");
		return;
	}

	idaTrackData(("XML-Request:\n%s", xmlString.cString()));


	{
		ofstream ofs("xmlrequest.dump");
		ofs << xmlString;
		ofs.close();
	}

	
	if (!parser)
	{
		idaTrackFatal(("new DOMParser failed"));
		#ifdef MONITORING
			cout << "\t\t*** new DOMParser failed" << endl;
		#endif
	}


	parser->reset();
	//parser->setValidationScheme(gValScheme);
	//parser->setDoValidation(doValidation);
        //parser->setDoNamespaces(doNamespaces);
	DomErrorHandler errorHandler(errorText);
	parser->setErrorHandler(&errorHandler);
	//parser->setCreateEntityReferenceNodes(gDoCreate);
	//parser->setToCreateXMLDeclTypeNode(true);


	// ------------------------------------------------------------------------
	// Aus der payload versuchen wir ein DOM-Dokument aufzubauen
	DOMDocument* doc = NULL;
	if (parseXmlRequestToDom(xmlString, parser, doc, errorText) != isOk)
	{
		idaTrackExcept(("XML Parse Error"));
		#ifdef MONITORING
			cout << dbIdString << "parser error" << endl;
		#endif
		// Beim Parsen des XML-Requests ist ein Fehler aufgetreten.
		// Das Fehler-Reporting hat bereits in parseXmlRequestToDom()
		// stattgefunden.
		// Die Anfrage kann nicht weiter bearbeitet werden und
		// wird zurückgewiesen.
		refuseRequest(requestContainer, "parserError", errorText);
		return;
	}

	// -------------------------------------------------------------------------
	// Wir holen den Wert für den timeout aus dem DOM und tragen diesen
	// in den RequestContainer ein.
	ULong timeout = searchTimeOut;			// default aus der Konfigurationsdatei
	getTimeoutFromDom(doc, timeout);		// Wenn dem Request ein eigener Timeout-Wert mit gegeben
											// wurde, wird der Default-Wert in "timeout" hier
											// überschrieben
	requestContainer.setTimeout(timeout);	// ... und den Wert im Container speichern
	idaTrackData(("timeout = %d [ms]", timeout));

	// -------------------------------------------------------------------------
	// Wir starten den Timer. Dieser bestimmt den Zeitpunkt, wann der Request
	// unabhängig davon, wie weit er bearbeitet wurde, zurückgewiesen wird
	RefId timerId = 0;
	if (startTimer(timerId, timeout, PcpTimerInterface::once) == isNotOk)
	{
		idaTrackFatal(("startTimer failed"));
		#ifdef MONITORING
			cout << dbIdString << "startTimer failed" << endl;
		#endif
		refuseRequest(requestContainer, "internalError", "startTimer failed");
		return;
	}
	requestContainer.setTimerId(timerId);
	
	// -------------------------------------------------------------------------
	#ifdef MONITORING
		cout << dbIdString << "no authentication" << endl;
	#endif
	requestContainer.setStatus(RequestContainer::readyToSend);

	// -------------------------------------------------------------------------
	// Als nächstes muß festgestellt werden um welche Art von Request es sich handelt.
	// Möglichkeiten sind: Login, Logout, ChangPwd und Request
	// Der Typ wird über das Wurzel-Element bestimmt
	bool isModifyRequest = false;
	String requestType;
	getRootElementName(doc, requestType);
	// Je nach Type muß anders vorgegangen werden
	if (requestType == String("Login"))
	{
		idaTrackData(("request type: Login"));
		requestContainer.setMode(RequestContainer::login);
		requestPool.addRequest(requestContainer);
		processSendQueue();
		return;
	}
	else if (requestType == String("Logout"))
	{
		idaTrackData(("request type: Logout"));
		requestContainer.setMode(RequestContainer::logout);
		requestPool.addRequest(requestContainer);
		processSendQueue();
		return;
	}
	else if (requestType == String("ChangePwd"))
	{
		idaTrackData(("request type: ChangePwd"));
		requestContainer.setMode(RequestContainer::changePwd);
		requestPool.addRequest(requestContainer);
		processSendQueue();
		return;
	}
	else if (requestType == String("Request"))
	{
		idaTrackData(("request type: Request"));
		requestContainer.setMode(RequestContainer::request);
		#ifdef MONITORING
//			cout << dbIdString << "requestType is \"Request\"" << endl;
		#endif
	}
	else if (requestType == String("ModifyRequest"))
	{
		idaTrackData(("request type: ModifyRequest"));
		requestContainer.setMode(RequestContainer::request);
		isModifyRequest = true;
		#ifdef MONITORING
//			cout << dbIdString << "requestType is \"ModifyRequest\"" << endl;
		#endif
	}
	else
	{
		// Wenn dieser Fall ein tritt, liegt ein Fehler vor
		idaTrackExcept(("Unknown request type: %s", requestType.cString()));
		refuseRequest(requestContainer, "formatError", "unknown request type (see XML root element)");
		return;
	}


	
	// -------------------------------------------------------------------------
	// Wir prüfen die Bereitschaft.
	// Wenn wir nicht registriert sind, nehmen wir keine Requests an
	if (getStatus() != registered)
	{
		if (getStatus() != tryingToRegister || registerTimerFlag == false)
		{
			startRegistration();
		}
		switch (getStatus())
		{
			case initializationFailed :
				idaTrackExcept(("Initialization error --> request refused"));
				refuseRequest(requestContainer, "notReady", "Initialization error");
        			try{
          				parser->resetDocumentPool();
          				parser->reset();
        			}
        			catch (const XMLException& e)
        			{
                			// Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
                			idaTrackExcept(("TdfAccess::parseXml failed"));

                			errorText.cat("\nError during Xerces cleanup:\n");
                			errorText.cat("Exception message is:  \n");
                			errorText.cat(String((const char*)(e.getMessage())));
        			}
				return;
			case unAvailable :
				idaTrackExcept(("System is not available --> request refused"));
				refuseRequest(requestContainer, "unAvailable", "System is not available");
        			try{
          				parser->resetDocumentPool();
          				parser->reset();
        			}
        			catch (const XMLException& e)
        			{
                			// Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
                			idaTrackExcept(("TdfAccess::parseXml failed"));

                			errorText.cat("\nError during Xerces cleanup:\n");
                			errorText.cat("Exception message is:  \n");
                			errorText.cat(String((const char*)(e.getMessage())));
        			}
				return;
			case tryingToRegister :
				idaTrackExcept(("System is trying to register at DB --> request refused"));
				refuseRequest(requestContainer, "tryingToRegister", "System is trying to register at DB");
        			try{
          				parser->resetDocumentPool();
          				parser->reset();
        			}
        			catch (const XMLException& e)
        			{
                			// Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
                			idaTrackExcept(("TdfAccess::parseXml failed"));

                			errorText.cat("\nError during Xerces cleanup:\n");
                			errorText.cat("Exception message is:  \n");
                			errorText.cat(String((const char*)(e.getMessage())));
        			}
				return;
		}
	}


	// -------------------------------------------------------------------------
	// DOM traversieren und dabei ein TdsRequest-Objekt aufbauen
	TdsRequest tdsRequest;
	ModifyRequest modifyRequest;
	ReturnStatus result;

	if ( isModifyRequest )
	{
		result = createModifyRequestFromDom(modifyRequest, doc, errorText);
	}
	else
	{
		result = createTdsRequestFromDom(tdsRequest, doc, errorText);
	}

	if (result != isOk)
	{
		idaTrackExcept(("Could not create TdsRequest/ModifyRequest from DOM"));
		#ifdef MONITORING
			cout << dbIdString << "error while creating tdsrequest/modifyrequest(" << errorText.cString() << ")" << endl;
		#endif
		// Fehler beim Aufbau des TdsRequest-Objektes
		// Also wieder ein Fehler-Dokument senden
		refuseRequest(requestContainer, "requestCreationError", errorText);

   			try{
       				parser->resetDocumentPool();
       				parser->reset();
       			}
       			catch (const XMLException& e)
       			{
               			// Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
               			idaTrackExcept(("TdfAccess::parseXml failed"));

                		errorText.cat("\nError during Xerces cleanup:\n");
                		errorText.cat("Exception message is:  \n");
                		errorText.cat(String((const char*)(e.getMessage())));
        		}
		return;
	}

	if ( isModifyRequest )
	{
		idaTrackData(("ModifyRequest:\n%s", modifyRequest.dumpString()));
	}
	else
	{
		idaTrackData(("TdsRequest:\n%s", tdsRequest.dumpString()));
	}
/*
	{
		ofstream ofs("tdsrequest.dump");
		tdsRequest.dump(ofs);
		ofs.close();
	}
*/




	// -------------------------------------------------------------------------
	// TdfArgument anlegen und im RequestContainer ablegen
	TdfArgument* tdfArgument = new TdfArgument;

	if ( isModifyRequest )
	{
		result = createTdfArgument(tdfArgument, modifyRequest, doc, requestId, errorText);
	}
	else
	{
		result = createTdfArgument(tdfArgument, tdsRequest, doc, requestId, errorText);
	}

	if (result == isNotOk)
	{
		idaTrackExcept(("Could not create TdfArgument-Object"));
		#ifdef MONITORING
			cout << dbIdString << "error while creating tdfargument" << endl;
		#endif
		refuseRequest(requestContainer, "requestCreationError", errorText);
   			try{
       				parser->resetDocumentPool();
       				parser->reset();
       			}
       			catch (const XMLException& e)
       			{
               			// Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
               			idaTrackExcept(("TdfAccess::parseXml failed"));

                		errorText.cat("\nError during Xerces cleanup:\n");
                		errorText.cat("Exception message is:  \n");
                		errorText.cat(String((const char*)(e.getMessage())));
        		}
		DELETE(tdfArgument);
		return;
	}
	requestContainer.setTdfArgument(tdfArgument);
	
	idaTrackData(("TdfArgument:\n%s", tdfArgument->dumpString()));
	//Gibt das Dokument und alle damit referenzierten DOM-Objekte frei

	try{
	  parser->resetDocumentPool();
	  parser->reset();
	}
        catch (const XMLException& e)
    	{
                // Ein Fehler ist beim Aufräumen der XML Platform aufgetreten
                idaTrackExcept(("TdfAccess::parseXml failed"));

                errorText.cat("\nError during Xerces cleanup:\n");
                errorText.cat("Exception message is:  \n");
                errorText.cat(String((const char*)(e.getMessage())));
    	}



/*
	{
		ofstream ofs("tdfargument.dump");
		tdfArgument->dump(ofs);
		ofs.close();
	}
*/
	
	// -------------------------------------------------------------------------
	// Jetzt sollte der Request bereit zum Senden sein. Wir prüfen das nach
//	requestContainer.checkStatus();		// Dieser Call setzt auch den internen Status
//	if (requestContainer.getStatus() != RequestContainer::readyToSend)
//	{
//		#ifdef MONITORING
//			cout << dbIdString << "Request was not prepared correctly" << endl;
//		#endif
//	}
	
	// -------------------------------------------------------------------------
	// Jetzt muß der RequestContainer noch in die SendQueue eingetragen werden
	requestPool.addRequest(requestContainer);

	
	// -------------------------------------------------------------------------
	// Als letztes wird der Sendevorgang angstoßen
	processSendQueue();

	#ifdef MONITORING
		{
			++requestCounter;
			RelTime delta = PcpTime() - timeStamp1;
			ULong dauer = delta.inMilliSeconds();
			cout << dbIdString << "(" << requestId << ")Request       : " << dauer << " [ms]" << endl;
			requestDauer += dauer;
			if (dauer < requestDauerMin) requestDauerMin = dauer;
			if (dauer > requestDauerMax) requestDauerMax = dauer;
			if (requestCounter >= 100)
			{
				cout << dbIdString << "\t\t\t\t\t\tRequest  min/avg/max [ms] : " 
					 << requestDauerMin << "/"
					 << requestDauer / requestCounter << "/"
					 << requestDauerMax
					 << endl;
				requestDauerMin = 1000000;
				requestDauerMax = 0;
				requestDauer = 0;
				requestCounter = 0;
			}
		}
	#endif

	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
Void TdfAccess::refuseRequest(RequestContainer& requestContainer,
							  String errorCode,
							  String errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::refuseRequest(...)");
#endif	
	
	idaTrackExcept(("request refused: %s", errorCode.cString()));
	PROBLEM(myObjectId, iDAMinRepClass, 121, errorCode.cString());
	
	String xmlString;
	
	// XML Response-Dokument erzeugen ...
	formatXMLErrorResponse(xmlString, errorCode, errorText);

	// ... und senden
	sendBigString(
		requestContainer.getSenderOid().getIdentifier(),
		requestContainer.getRequestId(),
		myObjectId.getIdentifier(),
		Types::XML_RESPONSE_MSG,
		xmlString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
Void TdfAccess::sendAuthenticationAcknowledge(RequestContainer& requestContainer)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::sendAuthenticationAcknowledge(...)");
#endif	

	idaTrackData((""));
	EVENT(myObjectId, iDAMinRepClass, 236, "");
	
	
	String xmlString;
	
	// XML Response-Dokument erzeugen ...
	xmlString.cat(tagHEADER);
	xmlString.cat(tagoOsaResponse);
	
	switch (requestContainer.getMode())
	{
		case RequestContainer::login :		xmlString.cat(" type=\"loginAcknowledge\">\n");     break;
		case RequestContainer::logout :		xmlString.cat(" type=\"logoutAcknowledge\">\n");	break;
		case RequestContainer::changePwd :	xmlString.cat(" type=\"changePwdAcknowledge\">\n");	break;
	}
	
	xmlString.cat(tagcOsaResponse);

	idaTrackData(("sending SES autentication aknowledge:\n%s", xmlString.cString()));

	// ... und senden
	sendBigString(
		requestContainer.getSenderOid().getIdentifier(),
		requestContainer.getRequestId(),
		myObjectId.getIdentifier(),
		Types::XML_RESPONSE_MSG,
		xmlString);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Implementierung einer Methode der abstrakten Superklasse "PcpTimerInterface".
//
//	Alle Timer-Events lösen einen Aufruf dieser Methode aus.
//	
Void TdfAccess::timerBox(const RefId id)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::timerBox(...)");
#endif

	UShort requestId;
	
	// -------------------------------------------------------------------------
	// Wenn der RegistrierungsTimer zugeschlagen hat ...
	if (id == registerTimerId)
	{
		// Da registerTimerFlag immer anzeigen soll, ob der Timer aktiv ist
		// oder nicht, muß das Flag hier gelöscht werden
		registerTimerFlag = false;
		
		idaTrackExcept(("Handling register timeout"));
		PROBLEM(myObjectId, iDAMinRepClass, 109, countryCode);
		
		#ifdef MONITORING
			cout << dbIdString << "TdfAccess::timerBox() : register timer event" << endl;
		#endif


		// Wiederholung der Registrierung
		sendRegisterRequest();

		return;
	}
	// -------------------------------------------------------------------------
	// Wenn der SendRetry Timer zugeschlagen hat ...
	if (id == retryTimerId)
	{
		retryTimerFlag = false;

		idaTrackExcept(("send retry timer timeout"));
		#ifdef MONITORING
			cout << dbIdString << "TdfAccess::timerBox() : send retry timer timeout" << endl;
		#endif
		
		// Es wird Zeit zu versuchen evtl. Request erneut zu senden, die zuvor nicht
		// gesendet werden konnten
		processSendQueue();	

		return;
	}
	// -------------------------------------------------------------------------
	// Timeout eines Requestes
	if (requestPool.findTimerId(id, requestId))
	{
		idaTrackExcept(("Request timeout, item is removed from sendqueue"));
		PROBLEM(myObjectId, iDAMinRepClass, 110, "");
		#ifdef MONITORING
			cout << dbIdString << "\t\t\t\t*** Request removed from requestPool (timeout)" << endl;
		#endif

		// wir holen die Request-Daten aus der Request-Liste
		RequestContainer requestContainer;
		requestPool.getRequest(requestId, requestContainer);
		
		// Die DB bekommt eine cancleRequest Nachricht
		if (requestContainer.getStatus() == RequestContainer::sendAndWaiting)
		{
			#ifdef MONITORING
				cout << dbIdString << "\t\t\t\t*** sending cancelSearchRequest()" << endl;
			#endif
			TdfCancelSearchArg tdfCancelSearchArg;
			tdfCancelSearchArg.setApplicationId(requestContainer.getTdfArgument()->getApplicationId());
			tdfCancelSearchArg.setSearchRef(requestContainer.getTdfArgument()->getSearchRef());
			tdfCancelSearchArg.setSourceId(requestContainer.getTdfArgument()->getSourceId());
			ReturnStatus res = cancelSearchRequest(tdfCancelSearchArg);
			#ifdef MONITORING
				if (res == isOk)
				{
					cout << dbIdString << "\t\t\t\t*** cancelSearchRequest() == isOk" << endl;
				}
				else
				{
					cout << dbIdString << "\t\t\t\t*** cancelSearchRequest() == isNotOk" << endl;
					OsaComError error = getLastError();
					#ifdef MONITORING
						cout << "cancelSearchRequest() failed: " << error.getErrorCode() << "\n\t"
							 << error.getErrorSource() << "\n\t"
							 << error.getErrorText() << endl;
					#endif

				}
			#endif
		}

		// Wir schicken ein Fehler-Dokument
		refuseRequest(requestContainer, "timeout", "request timeout, item is removed from sendqueue");

		// TdfArgument-Objekt löschen !
		TdfArgument* tdfArgument = requestContainer.getTdfArgument();
		DELETE(tdfArgument);

		channelMgr.releaseChannel(requestContainer.getChannel());

		// Request aus der Liste löschen
		requestPool.removeRequest(requestContainer.getRequestId());

		return;
	}
	
	idaTrackFatal(("Ignoring unrelated timer message"));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
Void TdfAccess::assembleTdfOriginId(TdfOriginId* orId, const char* clientAddress)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::assembleTdfOriginId(...)");
#endif
	
	orId->setProduct(OsaApiConstants::ida);
	orId->setLocalAddress(localTdfAddress);
	orId->setProtocol(OsaApiConstants::httpProt);

	TdfAddress clientTdfAddress;
	ULong clientIpAddress = resolveClientIp(clientAddress);
	idaTrackData(("clientIpAddress: %d", clientIpAddress));

	if (clientIpAddress)
	{
		clientTdfAddress.setIpAddress(clientIpAddress);
	}
	orId->setRemoteAddress(clientTdfAddress);
	orId->setServiceName(serviceName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	write application data like values of class member variables into a string
//
String TdfAccess::dumpReadable()
{
//	TRACE_FUNCTION("TdfAccess::dumpReadable(...)");

	
	char oidString[32];
	char appString[32];
	char idString[32];

	sprintf(oidString, "%ld", myObjectId.getIdentifier());
	sprintf(appString, "%lu", applicationId);
	sprintf(idString, "%lu", searchId);

	String temp = String("\n") +
				  "      -------------------------------------------------------------------- \n" +
				  "                           Country Code      : " + countryCode + "\n" +
				  "                           Application Name  : " + applicationName + "\n" +
				  "                           Service Name      : " + serviceName + "\n" +
				  "                           Object Id         : " + String(oidString) + "\n" +
				  "                           Application Id    : " + String(appString) + "\n" +
				  "                           Search Id         : " + String(idString) + "\n" +
				  "      -------------------------------------------------------------------- " + "\n";

	return temp;
}


/////////////////////////////////////////////////////////////////////////
//
ReturnStatus TdfAccess::createXmlNode(String& xmlString, const String& tag, const char* val)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createXmlNode(...)");
#endif

	xmlString.cat(tagStart);
	xmlString.cat(tag);
	xmlString.cat(tagEnd);

	// an dieser Stelle muessen wir einige Zeichen maskieren
	Int saveCounter = LONG_MAX;
	for (char* value = (char *)val; *value; value++)
	{
		switch (*value)
		{
			case '<':
				xmlString.cat(tagLT);
				break;
			case '>':
				xmlString.cat(tagGT);
				break;
			case '&':
				xmlString.cat(tagAMP);
				break;
			case '\'':
				xmlString.cat(tagAPOS);
				break;
			case '\"':
				xmlString.cat(tagQUOT);
				break;
			default:
				xmlString.cat(*value);
		}
		// Zur Sicherheit, dass keine Endlosschleife enstehen kann
      // Can this really happen? Since I'n not sure I will use the maximum
      // possible number. 255 characters are definitely not enough and
      // will lead to data loss in some cases.
		if (!--saveCounter)	break;
	}

	xmlString.cat(tagcStart);
	xmlString.cat(tag);
	xmlString.cat(tagEndNL);

	return isOk;
}


/////////////////////////////////////////////////////////////////////////
//
ReturnStatus TdfAccess::createXmlNode(String& xmlString, const String& tag, const Int val)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createXmlNode(...)");
#endif

	static char buffer[100];
	sprintf(buffer, "%d", val);
	createXmlNode(xmlString, tag, (const char*)buffer);

	return isOk;
}


/////////////////////////////////////////////////////////////////////////
//
ReturnStatus TdfAccess::createXmlNode(String& xmlString, const String& tag, const ULong val)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createXmlNode(...)");
#endif

	static char buffer[100];
	sprintf(buffer, "%u", val);
	createXmlNode(xmlString, tag, (const char*)buffer);

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Direkte Wandlung des TdsResponse-Objektes in einen XML-String
//
ReturnStatus TdfAccess::convTdsResponseToXml(TdsResponse& tdsResponse,
												  String& xmlString,
												  String& typeString)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::convTdsResponseToXml(...)");
#endif

	VisibleString visibleString;

	typeString = "error";

	Bool errorFlag	= false;		// Zeigt an, ob Fehler gemeldet wurden
	Int  records	= 0;			// Anzahl Records, wir später gebraucht um den typeString richtig zu setzen

	// -----------------------------------------------------------------------------
	// SearchResult
	xmlString.cat(tagoSearchResult);

	const SearchResult& searchResult = tdsResponse.getSearchResult();

	if (searchResult.isResultSpecifierSet())
	{
		enumToString(searchResult.getResultSpecifier(), visibleString);
		createXmlNode(xmlString, tagResultSpecifier, visibleString.getDataString());	
	}
	
	if (searchResult.isErrorSourceSet())
	{
		enumToString(searchResult.getErrorSource(), visibleString);
		createXmlNode(xmlString, tagErrorSource, visibleString.getDataString());
	}
	
	if (searchResult.isErrorCodeSet())
	{
		errorFlag = true;
		enumToString(searchResult.getErrorCode(), visibleString);
		createXmlNode(xmlString, tagErrorCode, visibleString.getDataString());
	}
	

	if (searchResult.isErrorAttributeSet())
	{
		enumToString(searchResult.getErrorAttribute(), visibleString);
		createXmlNode(xmlString, tagErrorAttribute, visibleString.getDataString());
	}
	

	if (searchResult.isErrorWordSet())
	{
		createXmlNode(xmlString, tagErrorWord, (const char*)searchResult.getErrorWord());
	}
	
	if (searchResult.isErrorTextSet())
	{
		createXmlNode(xmlString, tagErrorText, (const char*)searchResult.getErrorText());
	}
	
	if (searchResult.isResultSourceSet())
	{
		visibleString = searchResult.getResultSource();
		createXmlNode(xmlString, tagResultSource, visibleString.getDataString());
	}
	

	xmlString.cat(tagcSearchResult);


	// ----------------------------------------------------------------------------
	// SearchSpec(ification)
	if (tdsResponse.isSearchSpecificationSet())	// Das Attribut ist optional
	{
		String result;
		String byteString;
		
		
		xmlString.cat(tagoSearchSpec);

		const SearchSpec& searchSpec = tdsResponse.getSearchSpecification();

		if (searchSpec.isOperationSet())
		{
			enumToString(searchSpec.getOperation(), visibleString);
			createXmlNode(xmlString, tagOperation, visibleString.getDataString());
		}

		if (searchSpec.isLinkContextSet())
		{
			searchSpec.getLinkContext(byteString);
			dataToHexString(byteString, result);
			createXmlNode(xmlString, tagLinkContext, result.cString());
		}

		if (searchSpec.isUsedCharacterSetSet())
			createXmlNode(xmlString, tagUsedCharacterSet, (Int)searchSpec.getUsedCharacterSet());

		// VectorOfLinkContext
		if (searchSpec.isVectorOfLinkContextSet())
		{
			xmlString.cat(tagoVectorOfLinkContext);

			const VectorOfData & vectorOfLinkContext = searchSpec.getVectorOfLinkContext();

			for (int index = 0; index < vectorOfLinkContext.size(); index++)
			{
				byteString = vectorOfLinkContext[index].getDataString();
				dataToHexString(byteString, result);
				createXmlNode(xmlString, tagLinkContext, result.cString());
			}

			xmlString.cat(tagcVectorOfLinkContext);
		}

		xmlString.cat(tagcSearchSpec);

	}	// if (tdsResponse.isSearchSpecificationSet())


	// ----------------------------------------------------------------------------
	// RequestedRes(ponse)
	if (tdsResponse.isRequestedResponseSet())	// Das Attribut ist optional
	{
		xmlString.cat(tagoRequestedRes);

		const RequestedRes& requestedRes = tdsResponse.getRequestedResponse();

		if (requestedRes.isRequestedFormatSet())
		{
			enumToString(requestedRes.getRequestedFormat(), visibleString);
			createXmlNode(xmlString, tagRequestedFormat, visibleString.getDataString());
		}

		if (requestedRes.isLinesSet())
			createXmlNode(xmlString, tagLines, (Int)requestedRes.getLines());

		if (requestedRes.isColumnsSet())
			createXmlNode(xmlString, tagColumns, (Int)requestedRes.getColumns());

		if (requestedRes.isMaxRecordsSet())
			createXmlNode(xmlString, tagMaxRecords, (Int)requestedRes.getMaxRecords());

		if (requestedRes.isOrderingSet())
			createXmlNode(xmlString, tagOrdering, requestedRes.getOrdering());

		// SearchFilter
		if (requestedRes.isSearchFilterSet())
		{
			xmlString.cat(tagoSetOfAttributeId);

			// leider muessen wir hier das "const" weg-casten, sonst geht's nicht !
			SetOfAttributeId& setOfAttributeId
			= (SetOfAttributeId &)(requestedRes.getSearchFilter());

			// Und schon wieder muessen wir ueber eine Liste iterieren:
			SetOfAttributeId::iterator sIterator = setOfAttributeId.begin();
			while (sIterator != setOfAttributeId.end())
			{
				// hier gehts um Objekte vom Typ "DbAttribute::AttributedId"
				//  - das ist ein enum
				// Die Bezeichnungen entsprechen Feldern der Suchmaske
				// Den Bezeichner in String-Form bekommen wir wie folgt:
				VisibleString visibleString;
				enumToString(*sIterator, visibleString);
				// Und jetzt noch unter dem Tag "AttributeId" eintragen
				createXmlNode(xmlString, tagAttributeId, visibleString.getDataString());
				sIterator++;
			}

			xmlString.cat(tagcSetOfAttributeId);
		}

		// ExpRecordLines
		if (requestedRes.isExpRecordLinesSet())
			createXmlNode(xmlString, tagExpRecordLines, (Int)requestedRes.getExpRecordLines());

		// SourceLanguage
		if (requestedRes.isSourceLanguageSet())
			createXmlNode(xmlString, tagSourceLanguage, (Int)requestedRes.getSourceLanguage());

		xmlString.cat(tagcRequestedRes);

	} // if (tdsResponse.isRequestedResponseSet())


	// ----------------------------------------------------------------------------
	// ResultData
	if (tdsResponse.isResultDataSet())	// Das Attribut ist optional
	{
		xmlString.cat(tagoResultData);

		const ResultData& resultData = tdsResponse.getResultData();

		if (resultData.isFormatIdSet())
		{
			// ***********************************************************
			// ACHTUNG: Hier wird nur "indentedRecordFormat" unterstützt !
			// ***********************************************************

			if (resultData.getFormatId() == ResultData::indentedRecordFormat)
			{
				createXmlNode(xmlString, tagFormatId, "indentedRecordFormat");
			}
			else
			{
				// Fehlerfall
				idaTrackFatal(("\t*** only indentedRecordFormat is supported ***"));
				return isNotOk;
			}
		}

		if (resultData.isTotalNumberOfRecordsSet())
			createXmlNode(xmlString, tagTotalNumberOfRecords,
						  (Int)resultData.getTotalNumberOfRecords());

		if (resultData.isNumberOfRecordsReturnedSet())
		{
			records = (Int)resultData.getNumberOfRecordsReturned();
			createXmlNode(xmlString, tagNumberOfRecordsReturned,
						  (Int)resultData.getNumberOfRecordsReturned());
		}

		if (resultData.isTotalNumberOfMarkedRecordsSet())
			createXmlNode(xmlString, tagTotalNumberOfMarkedRecords,
						  (Int)resultData.getTotalNumberOfMarkedRecords());

		if (resultData.isNumberOfMarkedRecordsReturnedSet())
			createXmlNode(xmlString, tagNumberOfMarkedRecordsReturned,
						  (Int)resultData.getNumberOfMarkedRecordsReturned());

		if (resultData.isDataTypeSet())
		{
			VisibleString visibleString;
			enumToString(resultData.getDataType(), visibleString);
			createXmlNode(xmlString, tagDataType, visibleString.getDataString());

			// Wir entnehmen hier den Typ:
			typeString = visibleString.getDataString();
		}

		if (resultData.isIndentedRecordsSet())
		{
			xmlString.cat(tagoListOfRecords);

			// und wieder mal casten wir das "const" weg !
			VectorOfIndentRecord & vectorOfIndentRecord
			= (VectorOfIndentRecord &)(resultData.getIndentedRecords());

			// -----------------------------------------------------------------
			// An dieser Stelle wird die implizit per IndentLevel gegebene
			// Hierarchie der Records in eine explizite XML-Struktur gewandelt
			// D.h. die Records kommen nicht mehr einfach hintereinander, sondern
			// sind ihrem IndentLevel entsprechend baumartig verknüpft
			//
			// Bei der Iteration über die Records wird dazu immer der Pegel
			// an ausstehenden "schließenden XML-Tags" verwaltet
			// Kommt 
			int actLevel = 0;
			int pendingTags = 0;
			for (int index = 0; index < vectorOfIndentRecord.size(); index++)
			{
				IndentRecord& indentRecord = vectorOfIndentRecord[index];
				// IndentLevel ermitteln
				UChar indentLevel = 0;
				if (indentRecord.isIndentLevelSet())
				{
					indentLevel = indentRecord.getIndentLevel();
				}

				// Bei Folgeanfragen (Blättern) kann es vorkommen, dass der Record
				// nicht den Level 0 hat. In diesem Fall müssen zusätzliche 
				// öffnende Tags eingebaut werden.
				if (index == 0)
				{
					for (int i = 0; i < indentLevel; i++)
					{
						xmlString.cat(tagoRecord);
						++pendingTags;
					}
				}

				// Wenn noch ausstehende schliessende Tags fehlen ...
				if (pendingTags)
				{
					if (indentLevel <= actLevel)
					{
						for (int x = 0; x < actLevel-indentLevel+1; x++)
						{
							xmlString.cat(tagcRecord);
							--pendingTags;
						}
					}
				}

				// Öffnendes Tag
				xmlString.cat(tagoRecord);
				++pendingTags;

				convIndentRecordToXml(xmlString,
									  indentRecord);

				actLevel = indentLevel;
			}

			// Wenn jetzt noch schiessende Tags offen sind ...
			while (pendingTags > 0)
			{
				xmlString.cat(tagcRecord);
				--pendingTags;
			}

			// -----------------------------------------------------------------

			xmlString.cat(tagcListOfRecords);
		}


		// QstScreenData        NOT SUPPORTED !!!


		// UsedCharacterSet
		if (resultData.isUsedCharacterSetSet())
		{
			VisibleString visibleString;
			enumToString((OsaApiConstants::CharacterSet)resultData.getUsedCharacterSet(),
						 visibleString);
			createXmlNode(xmlString, tagUsedCharacterSet, visibleString.getDataString());
		}

		xmlString.cat(tagcResultData);
	}

	// ---------------------------------------------------------------------------------------------
	// Evtl. muß der typeString noch korrigiert werden:
	// Wenn keine Records geliefert wurden, eine Fehler sowie der RekordType gesetzt wurden,
	// dann soll "error" zurück gegeben werden !
	if ((records == 0) && errorFlag)
	{
		typeString = "error";
	}

	return isOk;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die "indented" Records werden in dieser Methode in das XML-Format umgewandelt und an den
//	übergebenen "xmlString" angehängt
//
ReturnStatus TdfAccess::convIndentRecordToXml(String& xmlString,
										  IndentRecord& indentRecord)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::convIndentRecordToXml(...)");
#endif

	VisibleString visibleString;

	// IndenLevel Tag
	if (indentRecord.isIndentLevelSet())
		createXmlNode(xmlString, tagIndentLevel, (const Int)indentRecord.getIndentLevel());

	// Flags
	if (indentRecord.isFlagsSet())
	{
		ULong flags = indentRecord.getFlags();
		for (int i = 0; i < 32; i++)
		{  // added first bit! NewRecordFlag was missing!
			ULong bit = (ULong(1) << i) & flags;
			if (bit)
			{
				enumToString((IndentRecord::Flags)bit, visibleString);
				createXmlNode(xmlString, tagFlag, visibleString.getDataString());
			}
		}
	}

	// OldRecordType
	if (indentRecord.isOldRecordTypeSet())
	{
		enumToString(indentRecord.getOldRecordType(), visibleString);
		createXmlNode(xmlString, tagFlag, visibleString.getDataString());
	}

	// LinkContext
	if (indentRecord.isLinkContextSet())
	{
		String byteString;
		String result;
		indentRecord.getLinkContext(byteString);
		dataToHexString(byteString, result);
		createXmlNode(xmlString, tagLinkContext, result.cString());
	}

	// isRecordTypeSet
	if (indentRecord.isRecordTypeSet())
	{
		createXmlNode(xmlString, tagRecordType, (Int)indentRecord.getRecordType());
	}

	// RecordSource
	if (indentRecord.isRecordSourceSet())
	{
		createXmlNode(xmlString, tagRecordSource, indentRecord.getRecordSource().getDataString());
	}

	// RecordId
	if (indentRecord.isRecordIdSet())
	{
		visibleString = indentRecord.getRecordId();
		createXmlNode(xmlString, tagRecordId, visibleString.getDataString());
	}
	
	// RecordVersion
	if (indentRecord.isRecordVersionSet())
	{
		visibleString = indentRecord.getRecordVersion();
		createXmlNode(xmlString, tagRecordVersion, visibleString.getDataString());
	}

	// VectorOfFormatInfo
	if (indentRecord.isVectorOfFormatInfoSet())
	{
		const VectorOfFormatInfo& vectorOfFormatInfo = indentRecord.getVectorOfFormatInfo();
		FormatInfo formatInfo;
		for (UInt i = 0; i < vectorOfFormatInfo.size(); i++)
		{
			formatInfo = vectorOfFormatInfo[i];

			// Flags
			if (formatInfo.isFlagsSet())
			{
				// hier haben wir den Aufwand etwas reduziert !!!
				if (formatInfo.getFlags() & FormatInfo::column)
				{
					createXmlNode(xmlString, tagFlag, "column");
				}
				if (formatInfo.getFlags() & FormatInfo::fixed)
				{
					createXmlNode(xmlString, tagFlag, "fixed");
				}
			}

			// Width
			if (formatInfo.isWidthSet())
			{
				createXmlNode(xmlString, tagWidth, formatInfo.getWidth());
			}
		}
	}


	// Datenbankfelder
	if (indentRecord.isSetOfDbAttributeSet())
	{
		SetOfDbAttribute& setOfDbAttribute
		= (SetOfDbAttribute &)(indentRecord.getSetOfDbAttribute());

		// Iteration über die DbAttribute Objekte
		SetOfDbAttribute::iterator sIterator = (SetOfDbAttribute::iterator)setOfDbAttribute.begin();
		while (sIterator != setOfDbAttribute.end())
		{
			DbAttribute& dbAttribute = *sIterator;

			// AttributeId
			if (dbAttribute.isAttributeIdSet() && dbAttribute.isDataSet())
			{
				enumToString(dbAttribute.getAttributeId(), visibleString);
				createXmlNode(
							 xmlString,
							 visibleString.getDataString(),
							 dbAttribute.getData().getDataString());
			}

			sIterator++;
		}

	}

	return isOk;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Im Falle eines Tdf-Fehlers wird von der OSA-API ein TdfErrorArg Objekt geliefert. Dieses wird
//	hier in ein XML-Dokument umgewandelt, das einer Teilstruktur entspricht, die aus dem normalen
//	Suchergebnis erzeugt wird.
//
ReturnStatus TdfAccess::convTdfErrorArgToXml(TdfErrorArg& tdfErrorArg,
												  String& xmlString,
												  String& typeString)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::convTdfErrorArgToXml(...)");
#endif

	typeString = "error";

	xmlString.cat(tagoSearchResult);	// öffnender Tag

	// ---------------------------------------------------------------

	VisibleString vString;

	if (tdfErrorArg.isErrorSourceSet())
	{
		enumToString(tdfErrorArg.getErrorSource(), vString);
		createXmlNode(xmlString, tagErrorSource, vString.getDataString());
	}

	if (tdfErrorArg.isErrorClassSet())
	{
		enumToString(tdfErrorArg.getErrorClass(), vString);
		createXmlNode(xmlString, tagErrorCode, vString.getDataString());
		#ifdef MONITORING
			cout << "\t\t\t\t*** ErrorClass=" << vString.getDataString() << endl;
		#endif
	}

	if (tdfErrorArg.isErrorTextSet())
	{
		createXmlNode(xmlString, tagErrorText, tdfErrorArg.getErrorText());
	}

	if (tdfErrorArg.isErrorDetailSet())
	{
		createXmlNode(xmlString, tagDetail, (Int)tdfErrorArg.getErrorDetail());
	}

	// ---------------------------------------------------------------

	xmlString.cat(tagcSearchResult);	// schließender Tag

	return isOk;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Die vorliegende Methode zerlegt einen grossen String in kleine
//  Teile und sendet diese ueber die Objekt-Kommunikation der CLASSLIB
//
ReturnStatus TdfAccess::sendBigString(RefId			serverOid,	// OID des Servers
									  Short			requestId,	// Message Referenznummer
									  RefId			myOid,		// eigene OID
									  const Short	msgType,	// Nachrichten Typ
									  const String&	string)	 	// zu sendende Daten
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::sendBigString(...)");
#endif

	// -------------------------------------------------------------------------
	// Zerlegen & Senden:
	Types::TransferHeader header;

	// Länge der Payload berechnen
	int         maxBlockSize    = Types::MAX_CLASSLIB_MSG_SIZE - sizeof(header) - 1;

	// Länge des gesamten zu uebertragenen Strings holen
	int         restStringSize  = string.cStringLen() + 1;

	// Speicher fuer message anlegen
	static char msg[Types::MAX_CLASSLIB_MSG_SIZE];

	// Zeiger auf den String holen dieser wird Block-weise inkrementiert
	char*       source          = (char*)string.cString();

	// header initialisieren
	header.messageId   = 0;
	header.blockNumber = 0;
	header.endFlag     = 0;
	header.messageSize = restStringSize;	// inkl. \0

	Message    message;

	while (restStringSize)
	{
		// CLASSLIB message vorbereiten
		message.setDestinationOid(serverOid);
		message.setMsgRef(requestId);
		message.setSourceOid(myOid);
		message.setMsgType(msgType);

		char* destination = msg;

		// Wenn die Rest-Stringlaenge kleiner als die Blockgroesse ist,
		// handelt es sich um den letzten Block
		if (restStringSize <= maxBlockSize) header.endFlag = 1;

		// header kopieren
		memcpy(destination, &header, sizeof(header));
		destination += sizeof(header);

		// entweder wir haben noch mehr als einen Block uebrig ...
		if (restStringSize > maxBlockSize)
		{
			memcpy(destination, source, maxBlockSize);
			destination[maxBlockSize]   =  0;
			message.setData(msg, sizeof(header) + maxBlockSize + 1);
			source                      += maxBlockSize;
			restStringSize              -= maxBlockSize;
		}
		// ... oder wir muessen nur noch den Rest verschicken.
		else
		{
			memcpy(destination, source, restStringSize);
			destination[restStringSize] =  0;
			message.setData(msg, sizeof(header) + restStringSize + 1);
			restStringSize              =  0;	// terminiert die Schleife
		}

		// Block senden ...........................................
		if (Process::getComMan()->sendDup(message) == isOk)
		{
		}
		else
		{
			idaTrackExcept(("TdfAccess::sendBigString(): Failed to send message."));
			return isNotOk;
		}

		// Blocknummer inkrementieren
		++(header.blockNumber);
	}

	{
		PcpTime time;
		idaTrackData(("*** Response send to WebProcess      at %d:%d:%d,%d\n",
				   time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
	}

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Umwandlung einer Binär-Zeichnkette in einen Hex-String damit dieser problemloser in ein
//	XML-Dokument integriert werden kann
//
Void TdfAccess::dataToHexString(String byteSequence, String& result)
{
	TRACE_FUNCTION("TdfAccess::dataToHexString(...)");
	UInt length = byteSequence.len();

	// DE_MR_6075, cp, 2010-10-19
	char* buf = new char [(2*length)+1];
	for (int h = 0; h < length; h ++)
	{
		sprintf(buf + (h << 1), "%02X", (unsigned int) (unsigned char) byteSequence[h]);
	}
	buf[(2*length)] = '\0';
	result = String(buf);
	delete [] buf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	In dieser Methode wird der XML-Parser "XERCES" angelegt und aufgerufen. Er erzeugt aus dem 
//	XML-Dokument (als String übergeben) eine mit den Daten gefüllte DOM-Struktur
//
ReturnStatus TdfAccess::parseXmlRequestToDom(String& xmlString,
											 XercesDOMParser* parser,
											 DOMDocument*& doc,
											 String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::parseXmlRequestToDom(...)");
#endif
	errorText = "";

	#ifdef MONITORING
//		cout << xmlString << endl << endl << endl;
	#endif

	// Aus dem String wird zunächst eine Speicherstruktur erzeugt, die vom Parser
	// als Input benutzt werden kann
	XMLCh* xmlChar = (XMLCh*)"tdfProcess";

#ifdef OS_AIX_4
	MemBufInputSource* memBuffer = new MemBufInputSource((const XMLByte*)xmlString.cString(),
								xmlString.cStringLen(), xmlChar, (int)0);
#else
	MemBufInputSource* memBuffer = new MemBufInputSource((const XMLByte*)xmlString.cString(),
								  xmlString.cStringLen(), xmlChar, false);
	
#endif	

	if (!memBuffer)
	{
		idaTrackFatal(("new MemBufInputSource failed"));
		#ifdef MONITORING
			cout << "\t\t*** new MemBufInputSource failed" << endl;
		#endif
		return isNotOk;
	}	

	// Der eigentliche Parser-Lauf kann Exceptions werfen, wenn
	// ein (Syntax-, etc.) Fehler auftritt.
    try
    {
		// Eingabe parsen und DOM-Tree aufbauen
        parser->parse(*memBuffer);
	}
	catch (const XMLException& e)
    {
   	  	#ifdef MONITORING
			cout << dbIdString << "Fehler beim Parsen" << endl;
		#endif
		
		// Ein Fehler ist beim Parsen aufgetreten
		idaTrackExcept(("TdfAccess::parseXmlRequestToDom failed"));

		errorText.cat("\nError during parsing memory stream:\n");
		errorText.cat("Exception message is:  \n");
		errorText.cat(String((const char*)(e.getMessage())));

		DELETE(memBuffer);

        return isNotOk;
    }

	if (!errorText.isEmpty())
	{
   	  	#ifdef MONITORING
			cout << dbIdString << "Fehler beim Parsen" << endl;
		#endif
		
		// Der Fehlertext (errorText) wird bereits vom errorHandler gesetzt

		// Ein Fehler ist beim Parsen aufgetreten
		idaTrackExcept(("TdfAccess::parseXmlRequestToDom failed"));

		DELETE(memBuffer);

        return isNotOk;
	}


	// Dokument übergeben
	doc = parser->getDocument();
//	doc = parser->adoptDocument();

	DELETE(memBuffer);

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Methode erzeugt aus einem als Text übergebenen Fehler ein gültiges 
//	XML Dokument
//
ReturnStatus TdfAccess::formatXMLErrorResponse(String& xmlString,
											   const String& errorCode,
											   const String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::formatXMLErrorResponse(...)");
#endif

	xmlString.cat(tagHEADER);

	xmlString.cat(tagoOsaResponse);
	xmlString.cat(" type=\"error\">\n");
	
	xmlString.cat(tagoSearchResult);
	// -------------------------------------------------------------------------
	createXmlNode(xmlString, tagErrorSource, "tdfAccess");
	createXmlNode(xmlString, tagErrorCode, errorCode.cString());
	createXmlNode(xmlString, tagErrorText, errorText.cString());
	// ---------------------------------------------------------------
	xmlString.cat(tagcSearchResult);
	xmlString.cat(tagcOsaResponse);

	return isOk;
}




ReturnStatus TdfAccess::createModifyRequestFromDom(ModifyRequest& modifyRequest,
                                                DOMDocument* dom,
                                                String& errorText)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::createModifyRequestFromDom(...)");
#endif


        #ifdef XML_REQ_MON
                cout << dbIdString << "TdsRequest" << endl;
        #endif

        errorText = "error while creating TdsRequest";


        DOMElement* root = dom->getDocumentElement();
        if(root==null)
        {
                #ifdef MONITORING
                        cout << dbIdString << "\t\t\t\t\t\t*** getDocumentElement() failed" << endl;
                #endif
                errorText = "getDocumentElement() failed -> no document root element found";
                return isNotOk;
        }

        DOMNodeList* childsFromRoot = root->getChildNodes();
        DOMNode* child;

        ReturnStatus result = isOk;


        // -------------------------------------------------------------------------
        // Iteration ueber alle Kinder des Root-Knoten
	// der TdfLinkContext steht im TdfArgument
        for (int i = 0; i < childsFromRoot->getLength(); i++)
        {
                // Knoten i holen
                child = childsFromRoot->item(i);

                // Knoten-Typ prüfen
                if (DOMNode::ELEMENT_NODE == child->getNodeType())
                {
                        // Namen des Knoten holen
                        const XMLCh* childElement = child->getNodeName();
                        char* transChildElement = XMLString::transcode(childElement);
                        Cpd tagString(transChildElement);

                        if (0 == strcmp((char*)tagString, "ModifySpecification"))
                        {
                                // ... entsprechende Unterfunktion aufrufen
                                result = createModifySpecification(modifyRequest, child, errorText);
                                if (result == isNotOk) break;
                        }

                        if (0 == strcmp((char*)tagString, "ModifyAttributes"))
                        {
                                // ... entsprechende Unterfunktion aufrufen
                                result = createModifyAttributes(modifyRequest, child, errorText);
                                if (result == isNotOk) break;
                        }
                }
        }

	if ( result == isOk )
	{
        	// ... entsprechende Unterfunktion aufrufen
        	result = createModifyFlags(modifyRequest, root, errorText);
	}

        if (result == isNotOk)
        {
                idaTrackExcept(("Error creating TdsRequest-Object from ModifyRequest!"));

		if ( errorText.len() <= 1 )
		{
		   errorText.assign("error creating modify request");
		}
        }

	// indentLevel and recordType are ignored by NDIS for modify requests
	modifyRequest.setIndentLevel( 0 );

        return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode füllt aus den Daten, übergeben als DOM-Struktur, ein TdsRequest-Objekt.
//
//
//	*****************************************************************************
//	Mit diesem Macro kann die Ausgabe des Requestes ein-/ausgeschaltet werden
//	#define XML_REQ_MON
//	*****************************************************************************
//
ReturnStatus TdfAccess::createTdsRequestFromDom(TdsRequest& tdsRequest,
												DOMDocument* dom,
												String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createTdsRequestFromDom(...)");
#endif

	
	#ifdef XML_REQ_MON
		cout << dbIdString << "TdsRequest" << endl;
	#endif
	
	errorText = "error while creating TdsRequest";
    

	DOMElement* root = dom->getDocumentElement();
	if(root==null)
	{
		#ifdef MONITORING
			cout << dbIdString << "\t\t\t\t\t\t*** getDocumentElement() failed" << endl;
		#endif
		errorText = "getDocumentElement() failed -> no document root elemente found";
		return isNotOk;
	}
	
	DOMNodeList* childsFromRoot = root->getChildNodes();
	DOMNode* child;
	
	ReturnStatus result = isOk;


	// -------------------------------------------------------------------------
	// Iteration über alle Kinder des Root-Knoten
	for (int i = 0; i < childsFromRoot->getLength(); i++)
	{
		// Knoten i holen
		child = childsFromRoot->item(i);

		// Knoten-Typ prüfen
		if (DOMNode::ELEMENT_NODE == child->getNodeType())
		{
			// Namen des Knoten holen
			const XMLCh* childElement = child->getNodeName();
			char* transChildElement = XMLString::transcode(childElement);
			Cpd tagString(transChildElement);
			
			// SearchSpecification:
			if (0 == strcmp((char*)tagString, "SearchSpecification"))
			{
				// ... entsprechende Unterfunktion aufrufen
				result = createSearchSpecification(tdsRequest, child, errorText);
				if (result == isNotOk) break;
			}

			// SearchVariation:
			if (0 == strcmp(tagString, "SearchVariation"))
			{
				// ... entsprechende Unterfunktion aufrufen
				result = createSearchVariation(tdsRequest, child, errorText);
				if (result == isNotOk) break;
			}

			// RequestedResponse:
			if (0 == strcmp(tagString, "RequestedResponse"))
			{
				// ... entsprechende Unterfunktion aufrufen
				result = createRequestedResponse(tdsRequest, child, errorText);
				if (result == isNotOk) break;
			}

			// SearchAttributes:
			if (0 == strcmp(tagString, "SearchAttributes"))
			{
				// ... entsprechende Unterfunktion aufrufen
				result = createSearchAttributes(tdsRequest, child, errorText);
				if (result == isNotOk) break;
			}

		}
	}

	if (result == isNotOk)
	{
		idaTrackExcept(("Error creating TdsRequest-Object !"));
	}



	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SearchSpec erzeugen:
//
//	Ein Validierung wird dem Parser beim Einlesen des XML-Strings überlassen !
//
ReturnStatus TdfAccess::createSearchSpecification(TdsRequest& tdsRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createSearchSpecification(...)");
#endif	
	
	ReturnStatus result = isOk;
	SearchSpec searchSpec;

	DOMNode* child;
	DOMNode* textNode;
	XMLCh* value;
	ULong enumVal;

	#ifdef XML_REQ_MON
		cout << dbIdString << "\tSearchSpec" << endl;
	#endif

	errorText = "XML:Request/SearchSpecification: missing parameter";
	DOMNodeList* nodeList = node->getChildNodes();
	
	if (nodeList->getLength() <= 0) return result;

	for (int i = 0; i < nodeList->getLength(); i++)
	{
		child = nodeList->item(i);
		const XMLCh* childElement = child->getNodeName();
		char* transChildElement = XMLString::transcode(childElement);	
		Cpd tagString(transChildElement);
		
		String stringValue("");

		// -------------------------------------------------------------------------
		// SearchSpecification/Operation:
		if (0 == strcmp(tagString, "Operation"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfOperation(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tOperation = " << stringValue << endl;
					#endif
					searchSpec.setOperation((SearchSpec::Operation)enumVal);
					continue;
				}
			}
			idaTrackExcept(("XML:Request/SearchSpecification/Operation: bad or missing value"));
			errorText = "XML:Request/SearchSpecification/Operation: bad or missing value";
			return isNotOk;
		}
	
		// -------------------------------------------------------------------------
		// SearchSpecification/LinkContext:
		if (0 == strcmp(tagString, "LinkContext"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				#ifdef XML_REQ_MON
					cout << dbIdString << "\t\tLinkContext = " << stringValue << endl;
				#endif
				searchSpec.setLinkContext(hexToData(stringValue.cString()));
			}
			continue;
		}
	
		// -------------------------------------------------------------------------
		// SearchSpecification/OsaCharacterSet:
		if (0 == strcmp(tagString, "OsaCharacterSet"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfCharacterSet(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tOsaCharacterSet = " << stringValue << endl;
					#endif
					searchSpec.setUsedCharacterSet((SearchSpec::CharacterSet)enumVal);
					continue;
				}
			}
			idaTrackExcept(("XML:Request/SearchSpecification/OsaCharacterSet: bad or missing value"));
			errorText = "XML:Request/SearchSpecification/OsaCharacterSet: bad or missing value";
			return isNotOk;
		}
	}



	// -------------------------------------------------------------------------
	// ... und jetzt noch das ganze ... in's TdsRequest-Object einhängen
	tdsRequest.setSearchSpecification(searchSpec);
	
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SearchVariation erzeugen:
//
ReturnStatus TdfAccess::createSearchVariation(TdsRequest& tdsRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createSearchVariation(...)");
#endif	
	
	ReturnStatus result = isOk;
	SearchVar searchVar;

	DOMNode* child;
	ULong enumVal;
	String stringValue;

	#ifdef XML_REQ_MON
		cout << dbIdString << "\tSearchVariation" << endl;
	#endif
	
	DOMNodeList* nodeList = node->getChildNodes();
	if (nodeList->getLength() <= 0) return result;


	ULong variation = 0;

	for (int i = 0; i < nodeList->getLength(); i++)
	{
		child = nodeList->item(i);
		const XMLCh* childName  = child->getNodeName();
		char* transChildName = XMLString::transcode(childName);
		Cpd tagString(transChildName);
	
		// -------------------------------------------------------------------------
		// SearchVariation/SearchType:
		if (0 == strcmp(tagString, "SearchType"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfSearchType(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tSearchType = " << stringValue << endl;
					#endif
					searchVar.setSearchType((SearchVar::SearchType)enumVal);
					continue;
				}
			}
			idaTrackExcept(("XML:Request/SearchVariation/SearchType: bad or missing value"));
			errorText = "XML:Request/SearchVariation/SearchType: bad or missing value";
			return isNotOk;
		}
	
		// -------------------------------------------------------------------------
		// SearchVariation/Expansion:
		if (0 == strcmp(tagString, "Expansion"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfExpansion(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tExpansion = " << stringValue << endl;
					#endif
					searchVar.setExpansion((SearchVar::Expansion)enumVal);
					continue;
				}
			}
			idaTrackExcept(("XML:Request/SearchVariation/Expansion: bad or missing value"));
			errorText = "XML:Request/SearchVariation/Expansion: bad or missing value";
			return isNotOk;
		}
	
		// -------------------------------------------------------------------------
		// SearchVariation/ExpansionRange:
		if (0 == strcmp(tagString, "ExpansionRange"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				#ifdef XML_REQ_MON
					cout << dbIdString << "\t\tExpansionRange = " << stringValue << endl;
				#endif
				searchVar.setExpansionRange(VisibleString(stringValue.cString()));
				continue;
			}
		}
	
		// -------------------------------------------------------------------------
		// SearchVariation/IndentLevelFilter:
		if (0 == strcmp(tagString, "IndentLevelFilter"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				#ifdef XML_REQ_MON
					cout << dbIdString << "\t\tIndentLevelFilter = " << stringValue << endl;
				#endif
				searchVar.setIndentLevelFilter(stringValue.cString());
				continue;
			}
		}
	
		// -------------------------------------------------------------------------
		// SearchVariation/Variation:
		if (0 == strcmp(tagString, "Variation"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfSearchVar(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tVariation = " << stringValue << endl;
					#endif
					variation += enumVal;
					continue;
				}
			}
			idaTrackExcept(("XML:Request/SearchVariation/Variation: bad or missing value"));
			errorText = "XML:Request/SearchVariation/Variation: bad or missing value";
			return isNotOk;
		}
	}
	
	if (variation) searchVar.setHelper1(variation);

	// -------------------------------------------------------------------------
	// ... und jetzt noch das ganze ... in's TdsRequest-Object einhängen
	tdsRequest.setSearchVariation(searchVar);
	
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	RequestedResponse erzeugen:
//
ReturnStatus TdfAccess::createRequestedResponse(TdsRequest& tdsRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createRequestedResponse(...)");
#endif	
	
	ReturnStatus result = isOk;
	RequestedRes requestedRes;

	DOMNode* child;
	ULong enumVal;
	String stringValue;

	#ifdef XML_REQ_MON
		cout << dbIdString << "\tRequestedResponse" << endl;
	#endif
	
	// -------------------------------------------------------------------------
	// SearchVariation/RequestedFormat:
	// Diesen Parameter setzen wir fest ein, da es keine andere Möglichkeit gibt
	requestedRes.setRequestedFormat(RequestedRes::indentedRecordFormat);


	DOMNodeList* nodeList = node->getChildNodes();
	if (nodeList->getLength() <= 0) return result;

	SetOfAttributeId setOfAttributeId;

	Bool attrFlag = false;

	for (int i = 0; i < nodeList->getLength(); i++)
	{
		child = nodeList->item(i);
		const XMLCh* childName = child->getNodeName();
		char* transChildName = XMLString::transcode(childName);
		Cpd tagString(transChildName);

		// -------------------------------------------------------------------------
		// RequestedResponse/MaxRecords:
		if (0 == strcmp(tagString, "MaxRecords"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				#ifdef XML_REQ_MON
					cout << dbIdString << "\t\tMaxRecords = " << stringValue << endl;
				#endif
				requestedRes.setMaxRecords(ushortOfString(stringValue.cString()));
				continue;
			}
		}
	
		// -------------------------------------------------------------------------
		// RequestedResponse/Ordering:
		if (0 == strcmp(tagString, "Ordering"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				#ifdef XML_REQ_MON
					cout << dbIdString << "\t\tOrdering = " << stringValue << endl;
				#endif
				requestedRes.setOrdering(stringValue.cString());
				continue;
			}
		}
	
		// -------------------------------------------------------------------------
		// RequestedResponse/SearchFilter:
		if (0 == strcmp(tagString, "DbAttribute"))
		{
			if (getValueOfDomNode(stringValue, child) == isOk)
			{
				if (StringToEnum::enumOfDbAttribute(enumVal, stringValue.cString()) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\tDbAttribute = " << stringValue << endl;
					#endif
					setOfAttributeId.insert((DbAttribute::AttributeId)enumVal);
					attrFlag = true;
					continue;
				}
			}
			idaTrackExcept(("XML:Request/RequestedResponse/DbAttribute: bad or missing value"));
			errorText = "XML:Request/RequestedResponse/DbAttribute: bad or missing value";
			return isNotOk;
		}
	}

	// -------------------------------------------------------------------------
	// ... und jetzt noch das ganze ... in's TdsRequest-Object einhängen
	if (attrFlag) requestedRes.setSearchFilter(setOfAttributeId);
	tdsRequest.setRequestedResponse(requestedRes);
	
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SearchAttributes erzeugen:
//
ReturnStatus TdfAccess::createSearchAttributes(TdsRequest& tdsRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createSearchAttributes(...)");
#endif	
	
	ReturnStatus result = isOk;
	VectorOfSearchAttr vectorOfSearchAttr;

	#ifdef XML_REQ_MON
		cout << dbIdString << "\tSearchAttributes" << endl;
	#endif
	
	DOMNode* child;
	ULong enumVal;

	idaTrackTrace(("TdfAccess::createSearchAttributes after DOMNode*"));

	// den ersten Unterknoten holen ("SearchAttr")
	child = node->getFirstChild();
	if (child==null)
	{
		errorText = "missing search attributes";
		return isNotOk;
	}
	idaTrackTrace(("TdfAccess::createSearchAttributes after child = node->getFirstChild()"));
	
	// -------------------------------------------------------------------------
	// Iteration über alle SearchAttr-Knoten
	while (!child==null)
	{
	  idaTrackTrace(("TdfAccess::createSearchAttributes after !child==null"));
		// ---------------------------------------------------------------------
		// Für alle Fälle prüfen wir mal, ob wir nicht etwas falsches bekommen haben
		const XMLCh* name = child->getNodeName();
		idaTrackTrace(("TdfAccess::createSearchAttributes after child->getNodeName()"));
		char* transName = XMLString::transcode(name);
		idaTrackTrace(("TdfAccess::createSearchAttributes after XMLString::transcode(name)"));
		if (0 != strcmp(Cpd(transName), "SearchAttr"))
		{
		  idaTrackTrace(("TdfAccess::createSearchAttributes after strcmp(Cpd(transName)"));
			// wenn doch, dann mit dem nächsten Knoten weiter machen
			child = child->getNextSibling();
			continue;
		}
		else{
		  idaTrackTrace(("TdfAccess::createSearchAttributes after else"));
#ifndef _LINUX
#ifndef _HP
        // leads to core on HP
        delete [] transName;
#endif
#endif

		}

		idaTrackTrace(("TdfAccess::createSearchAttributes before DOMNodeList* nodeList = child->getChildNodes()"));
		// ---------------------------------------------------------------------
		// Wenn der Knoten keine Kinder hat, machen wir auch mit dem nächsten weiter
		DOMNodeList* nodeList = child->getChildNodes();
		if (0 == nodeList->getLength())
		{
		  child = child->getNextSibling();
		  continue;
		}

		#ifdef XML_REQ_MON
		cout << dbIdString << "\t\tSearchAttr" << endl;
		#endif
		
		idaTrackTrace(("TdfAccess::createSearchAttributes before  for (int i = 0; i < nodeList->getLength(); i++)"));
		// ---------------------------------------------------------------------
		// Wir haben eine SearchAttr Knoten gefunden, der auch Kinder hat.
		// Es folgt eine Iteration über alle Kinder
		SearchAttr searchAttr;
		searchAttr.reset();
		ULong variation = 0;			// für die Variation
		Bool legalAttribut = false;		// Flag zur Schleifensteuerung
		
		for (int i = 0; i < nodeList->getLength(); i++)

		{
			DOMNode* childChild = nodeList->item(i);
			const XMLCh* childName = childChild->getNodeName();
			char* transChildName = XMLString::transcode(childName);
			Cpd tagString(transChildName);
			String stringValue("");

			// -----------------------------------------------------------------
			// Attribut:
			if (0 == strcmp(tagString, "Attribute"))
			{
				if (getValueOfDomNode(stringValue, childChild) == isOk)
				{
					if (StringToEnum::enumOfAttributeId(enumVal, stringValue.cString()) == isOk)
					{
						#ifdef XML_REQ_MON
							cout << dbIdString << "\t\t\tAttribute = " << stringValue << endl;
						#endif
						searchAttr.setAttributeId((SearchAttr::AttributeId)enumVal);
						 idaTrackData(("XML:Request/SearchAttributes/Attribute: "));	
						// Der Attribut-Name war gültig
						legalAttribut = true;
						continue;
					}
				}
				idaTrackExcept(("XML:Request/SearchAttributes/Attribute: bad or missing value"));
				break;
			}
			// -----------------------------------------------------------------
			// Value:
			if (0 == strcmp(tagString, "Value"))
			{
				if (getValueOfDomNode(stringValue, childChild) == isOk)
				{
					#ifdef XML_REQ_MON
						cout << dbIdString << "\t\t\tValue = " << stringValue.cString() << endl;
					#endif
					searchAttr.setAttributeValue(stringValue.cString());
				 	idaTrackData(("XML:Request/SearchAttributes/Attribute/Value : %s", stringValue.cString() ));	
					continue;
				}
				idaTrackExcept(("XML:Request/SearchAttributes/Attribute/Value: bad or missing value"));

			}
			// -----------------------------------------------------------------
			// Variation:
			if (0 == strcmp(tagString, "Variation"))
			{
				if (getValueOfDomNode(stringValue, childChild) == isOk)
				{
					if (StringToEnum::enumOfSearchAttr(enumVal, stringValue.cString()) == isOk)
					{
						#ifdef XML_REQ_MON
							cout << dbIdString << "\t\t\tVariation = " << stringValue << endl;
						#endif
						variation += enumVal;
						idaTrackData(("XML:Request/SearchAttributes/Variation : %s", stringValue.cString() ));
						continue;
					}
				}
				idaTrackExcept(("XML:Request/SearchAttributes/Variation: bad or missing value"));
				errorText = "XML:Request/SearchAttributes/Variation: bad or missing value";
				return isNotOk;
			}
		}
		// ---------------------------------------------------------------------
		// Wenn ein gültiges Attribut gefunden wurde ...
		if (legalAttribut)
		{
			// ... dann muß noch geprüft werden, ob variations gesetzt wurden ...
			if (variation != 0) searchAttr.setHelper1(variation);

			// ... bevor man das SearchAttr-Objekt in den Vector eintragen kann
			vectorOfSearchAttr.push_back(searchAttr);
		}


		child = child->getNextSibling();
	}
	 
	tdsRequest.setSearchAttributes(vectorOfSearchAttr);

	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Holt den Wert eines DOM-Knotens. Das Ergebnis der Methode ist nur dann
//	"isOk", wenn ein geeigneter Unterknoten vom Typ TEXT_NODE existiert und
//	dieser einen String mit einer Länge größer Null beinnhaltet.
//
ReturnStatus TdfAccess::getValueOfDomNode(String& textValue, DOMNode* node)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::getValueOfDomNode(...)");
#endif	
	const DOMNode* textNode = node->getFirstChild();
	// Das erste Kind des Knoten muß existieren und ein Text-Knoten sein
	if (!textNode==null)
	{

		if (textNode->getNodeType() == DOMNode::TEXT_NODE)
		{

			const XMLCh* value = textNode->getNodeValue();
			char* transValue = XMLString::transcode(value);
			textValue = (char*)Cpd(transValue);
			if (!textValue.isEmpty())
			{
				return isOk;
			}
		}
		else
		{
			idaTrackData(("textNode->getNodeType() != DOMNode::TEXT_NODE !"));
		}
	}
	
	idaTrackData(("getValueOfDomNode(): could not retreive node value, or it was empty !"));

	return isNotOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Extrahiert die Werte für die Authentifizierung (User, Password, etc.) aus einem DOM-Dokument
//
ReturnStatus TdfAccess::getAuthenticationParameter(DOMDocument*	dom,
												   String&			usr,
												   String&			pwd,
												   String&			npw,
												   String&			errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::getAuthenticationParameter(...)");
#endif	
	
	// Wir holen das Wurzel-Element
	DOMElement* root = dom->getDocumentElement();
	if (root==null)
	{
		errorText = "could not retreive Data from DOM, no root element";
		return isNotOk;
	}

	// Liste aller Kinder der Wurzel holen
	DOMNodeList* childsFromRoot = root->getChildNodes();
	// Iteration über die Liste der Kinder
	for (int i = 0; i < childsFromRoot->getLength(); i++)
	{
		// Knoten i holen
		DOMNode* child = childsFromRoot->item(i);

		// Koten-Typ prüfen
		if (DOMNode::ELEMENT_NODE == child->getNodeType())
		{
			// Namen des Knoten holen
			const XMLCh* childNodeName = child->getNodeName();
			char* transChildNodeName = XMLString::transcode(childNodeName);
			Cpd tagString(transChildNodeName);

			// <Authentication> gefunden ?
			if (0 == strcmp(tagString, "Authentication"))
			{
				// Liste der KindKnoten von Authentication holen ...
				DOMNodeList* childsFromAuthNode = child->getChildNodes();
				// ... und drüber iterieren
				for (int k = 0; k < childsFromAuthNode->getLength(); k++)
				{
					// Knoten i holen
					DOMNode* authChild = childsFromAuthNode->item(k);
					if (DOMNode::ELEMENT_NODE == authChild->getNodeType())
					{
						// Namen des Knoten holen
						const XMLCh* paramName = authChild->getNodeName();
						char* transParamName = XMLString::transcode(paramName);
						Cpd tagString(transParamName);
						
						if (0 == strcmp(tagString, "UserID"))
						{
							if (getValueOfDomNode(usr, authChild) == isNotOk)
							{
								errorText = "missing user name";
								return isNotOk;
							}
							idaTrackData(("user name: %s", usr.cString()));
						}
						if (0 == strcmp(tagString, "Password"))
						{
							if (getValueOfDomNode(pwd, authChild) == isNotOk)
							{
								errorText = "missing password";
								return isNotOk;
							}
							idaTrackData(("password: %s", pwd.cString()));
						}
						if (0 == strcmp(tagString, "NewPassword"))
						{
							if (getValueOfDomNode(npw, authChild) == isNotOk)
							{
								errorText = "missing new password";
								return isNotOk;
							}
							idaTrackData(("new password: %s", npw.cString()));
						}
					}
				}
			}
		}
	}
	return isOk;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Erzeugt ein TdfArgument-Objekt 
//
ReturnStatus TdfAccess::createTdfArgument(TdfArgument*	tdfArgument,
										  TdsRequest&	tdsRequest,
										  DOMDocument* doc,
										  Short         requestId,
										  String&       errorText)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::createTdfArgument(...)");
#endif
	
	ReturnStatus result = isOk;


	// -------------------------------------------------------------------------
	// Einige Parameter aus dem DOM holen
	String clientIpAddressString("127.0.0.1");		// Default 
	String linkContextString;
	getParameterFromDom(doc, clientIpAddressString, linkContextString);

	TdfOriginId tdfOriginId;
	assembleTdfOriginId(&tdfOriginId, clientIpAddressString.cString());


	// -------------------------------------------------------------------------
	// TdfArgument Objekt anlegen
	if (tdfArgument == null)
	{
		idaTrackFatal(("out of memory while creating TdfArgument"));
		errorText = "out of memory";
		return isNotOk;
	}
	
	// Attribute des TdfArgument-Objektes setzen
	tdfArgument->setApplicationId(applicationId);	// Kennung des OSA-Clients
	tdfArgument->setOsaTicket(osaTicket);			// Zugriffskontrolle
	tdfArgument->setReference(requestId);			// eindeutige Nummer des Requestes
	tdfArgument->setDataFormat(tdsDataFormat);		// zeigt an, daß die Daten im TDS-Format kommen
	tdfArgument->setData(tdsRequest);				// die eigentlichen Daten
													// die Daten des Objektes werden hier kopiert !
	tdfArgument->setSourceId(0);					// Hier nur ein Dummy für die Channel Nummer
													// damit der Test gut geht
	tdfArgument->setRequestAddress(tdfOriginId);	// Client-Info für MIS Statistik
	if (!linkContextString.isEmpty())
	{
		tdfArgument->setLinkContext(hexToData(linkContextString.cString()));	// TDF LinkContext setzen
	}
	
	// Test, ob alle obligatorischen Daten vorhanden sind !
	if (tdfArgument->areMandatoryItemsSet() == false)
	{
		idaTrackExcept(("TdfAccess::handleRequest: tdfArgument - args missing"));

		#ifdef MONITORING
			cout << dbIdString << "\nTdfArgument: args missing !" << endl;
		#endif

		errorText = "TdfArgument: args missing";

		return isNotOk;
	}

	return isOk;		
}


ReturnStatus TdfAccess::createTdfArgument(TdfArgument*  tdfArgument,
                                                                                  ModifyRequest&   modifyRequest,
                                                                                  DOMDocument* doc,
                                                                                  Short         requestId,
                                                                                  String&       errorText)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::createTdfArgument(...)");
#endif

        ReturnStatus result = isOk;


        // -------------------------------------------------------------------------
        // Einige Parameter aus dem DOM holen
        String clientIpAddressString("127.0.0.1");              // Default
        String linkContextString;
        getParameterFromDom(doc, clientIpAddressString, linkContextString);

        TdfOriginId tdfOriginId;
        assembleTdfOriginId(&tdfOriginId, clientIpAddressString.cString());


        // -------------------------------------------------------------------------
        // TdfArgument Objekt anlegen
        if (tdfArgument == null)
        {
                idaTrackFatal(("out of memory while creating TdfArgument"));
                errorText = "out of memory";
                return isNotOk;
        }

        // Attribute des TdfArgument-Objektes setzen
        tdfArgument->setApplicationId(applicationId);   // Kennung des OSA-Clients
        tdfArgument->setOsaTicket(osaTicket);                   // Zugriffskontrolle
        tdfArgument->setReference(requestId);                   // eindeutige Nummer des Requestes
        tdfArgument->setDataFormat(tdsModifyDataFormat);              // zeigt an, daß die Daten im TDS-ModifyFormat kommen
        tdfArgument->setData(modifyRequest);                               // die eigentlichen Daten
                                                                          // die Daten des Objektes werden hier kopiert !
        tdfArgument->setSourceId(0);                                    // Hier nur ein Dummy für die Channel Nummer
                                                                                                        // damit der Test gut geht
        tdfArgument->setRequestAddress(tdfOriginId);    // Client-Info für MIS Statistik
        if (!linkContextString.isEmpty())
        {
                tdfArgument->setLinkContext(hexToData(linkContextString.cString()));    // TDF LinkContext setzen
        }

        // Test, ob alle obligatorischen Daten vorhanden sind !
        if (tdfArgument->areMandatoryItemsSet() == false)
        {
                idaTrackExcept(("TdfAccess::handleRequest: tdfArgument - args missing"));

                #ifdef MONITORING
                        cout << dbIdString << "\nTdfArgument: args missing !" << endl;
                #endif

                errorText = "TdfArgument: args missing";

                return isNotOk;
        }

        return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode hat die Aufgabe die Liste der vorbereiteten Request-Objekte (Sendqueue)
//	abzuarbeiten. Sie prüft, ob Requests darauf warten verarbeitet zu werden und entscheidet
//	dann anhand ihres Status, ob z.B. zunächst eine Authentifizierung beim SES-Prozeß 
//	eingeholt werden muß, oder eine Such-Anfrage an das OSA-GW gesendet werden kann.
//
Void TdfAccess::processSendQueue()
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::processSendQueue(...)");
#endif

	idaTrackData(("process send queue:"));

	// -------------------------------------------------------------------------
	// Wir brauchen nur zu senden, wenn die Registrierung steht
	// Dadurch werden überflüssige Sendeversuche vermieden
//	if (getStatus() != registered)
//	{
//		// Wenn nicht, dann wird der Timer gestartet, damit diese Methode
//		// regelmäßig wieder aufgerufen wird
//		startSendRetryTimer();
//		return;
//	}


	// -------------------------------------------------------------------------
	// Der älteste Request kommt zuerst dran
	// Wenn das Ergebnis "false" ist, dann gibt es nichts zu tun
	RequestContainer requestContainer;
	if (!requestPool.getOldestWaitingRequest(requestContainer)) return;

	// -------------------------------------------------------------------------
	// Wir versuchen einen freien Channel zu bekommen
	UShort channel;
	channel = channelMgr.reserveChannel();
	if (channel <= 0)
	{
		idaTrackExcept(("no free channel found"));
		#ifdef MONITORING
			cout << dbIdString << "\t\t\t\t*** no free channel" << endl;
		#endif
		return;
	}


	// -------------------------------------------------------------------------
	// Channel eintragen
	requestContainer.setChannel(channel);
	requestContainer.getTdfArgument()->setSourceId(channel);
	idaTrackData(("sending on channel: %d", channel));


	// -------------------------------------------------------------------------
	// Feuer frei !!
	#ifdef MONITORING
		cout << dbIdString << "\t\t\t\tchannel = " << channel << endl;
	#endif

	ReturnStatus rs = isNotOk;
	TdfDataFormat format = requestContainer.getTdfArgument()->getDataFormat();

	if ( format == tdsModifyDataFormat )
	{
	  rs = modifyRequest(*(requestContainer.getTdfArgument()));
	}
	else
	{
	  rs = searchRequest(*(requestContainer.getTdfArgument()));
	}
         
	if (rs == isOk)
	{
		
		{
			PcpTime time;
			idaTrackData(("*** Request send to OSA-Gateway      at %d:%d:%d,%d",
					   time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
		}
		
		// Nachricht konnte gesendet werden
		requestContainer.setStatus(RequestContainer::sendAndWaiting);
		requestPool.removeRequest(requestContainer.getRequestId());
		requestPool.addRequest(requestContainer);

		// Und weil es so schoen war, geht vieleicht noch einer
		// ACHTUNG: Rekursion
		processSendQueue();
	}
	else
	{
		OsaComError error = getLastError();
		#ifdef MONITORING
			cout << "search/modifyRequest() failed: " << error.getErrorCode() << "\n\t"
				 << error.getErrorSource() << "\n\t"
				 << error.getErrorText() << endl;
		#endif

		idaTrackData(("search/modifyRequest() failed:  error.getErrorCode() = %d", error.getErrorCode() ));
		idaTrackData(("search/modifyRequest() failed:  error.getErrorSource() = %s", error.getErrorSource() ));
		idaTrackData(("search/modifyRequest() failed:  error.getErrorText() = %s", error.getErrorText() ));

		
		// leider hat's nicht geklappt !
		ALARM(myObjectId, iDAMinRepClass, 1, " TdfAccess --> IDA_DAP");
		idaTrackExcept(("search/modifyRequest() failed"));

		#ifdef MONITORING
			cout << dbIdString << "searchRequest failed !" << endl;
		#endif

		// Den Channel müssen wir wieder freigeben
		requestContainer.setChannel(0);
		requestContainer.getTdfArgument()->setSourceId(0);
		channelMgr.releaseChannel(channel);

		// Es kann nur die Registrierung verloren geganngen sein
		// also :
		startRegistration();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wandelt einen HEX-formatierten String in einen Byte-String um
//
//	>>> die Funktion ist zur Zeit noch nicht ganz dicht !!!
//
Data& TdfAccess::hexToData(const char * hex)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::hexToData(...)");
#endif

	// Buffer zurücksetzen
	tempData.reset();
	static char buffer[1024];

	// Schutz vor Null-Pointer Übergabe
	if (hex == 0) return tempData;

	int i = 0, l = 0;
	for (; i < strlen(hex) ; i += 2)
	{
	   l++;
	   char c1 = hex[i];
	   char c2 = hex[i+1];

	   c1 = (c1 >= 'A') ? (c1 - 'A' + 10) : (c1 - '0');
	   c2 = (c2 >= 'A') ? (c2 - 'A' + 10) : (c2 - '0');
	   buffer[i >> 1] = (c1 << 4) + c2;
	}
	tempData.assign(buffer, l);

	return tempData;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
UChar TdfAccess::ucharOfString(const char* str)
{
	return (UChar)atoi(str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
ULong TdfAccess::ulongOfString(const char* str)
{
	return (ULong)atol(str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
UShort TdfAccess::ushortOfString(const char* str)
{
	return (UShort)atol(str);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Methode ermittelt den Tag-Namen des obersten Elementes (Wurzel-Element) des mit "dom"
//	übergebenen DOM-Dokumentes
//
ReturnStatus TdfAccess::getRootElementName(DOMDocument*	dom,
										   String&			rootElementName)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::getRootElementName(...)");
#endif	
	// Wurzel-Element holen
	DOMElement* root = dom->getDocumentElement();
	if (root==null) 
	{
		return isNotOk;
	}
	

	// Namen des Knoten holen
	const XMLCh* nodeName = root->getNodeName();
    char* serializedNodeName = XMLString::transcode(nodeName);	

	Cpd tagString(serializedNodeName);

	// Wert zuweisen
	rootElementName = String(tagString);

	return isOk;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die folgende Funktion liefert zwei Werte aus dem DOM und zwar:
//		"ClientIpAddress" und "TdfLinkContext"
//
ReturnStatus TdfAccess::getParameterFromDom(DOMDocument*	dom,
											String&			clientIpAddressString,
											String&			linkContextString)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::getParameterFromDom(...)");
#endif	
	
	// Wir holen das Wurzel-Element
	DOMElement* root = dom->getDocumentElement();
	if (root==null) return isNotOk;

	DOMNodeList* childsFromRoot = root->getChildNodes();
	
	for (int i = 0; i < childsFromRoot->getLength(); i++)
	{
		// Knoten i holen
		DOMNode* child = childsFromRoot->item(i);

		// Koten-Typ prüfen
		if (DOMNode::ELEMENT_NODE == child->getNodeType())
		{
			// Namen des Knoten holen
			const XMLCh* childNodeName = child->getNodeName();
			char* transChildNodeName = XMLString::transcode(childNodeName);
			Cpd tagString(transChildNodeName);
			// <ClientIpAddress> gefunden ?
			if (0 == strcmp(tagString, "ClientIpAddress"))
			{
				DOMNode* childTextNode = child->getFirstChild();
				if (!childTextNode==null && childTextNode->getNodeType() == DOMNode::TEXT_NODE)
				{
					const XMLCh* childTextValue = childTextNode->getNodeValue();
					char* transChildTextValue = XMLString::transcode(childTextValue);
					clientIpAddressString = String(Cpd(transChildTextValue));
					idaTrackData(("clientIpAddress: %s", clientIpAddressString.cString()));
				}
			}
			
			// <TdfLinkContext> gefunden ?
			if (0 == strcmp(tagString, "TdfLinkContext"))
			{
				DOMNode* childTextNode = child->getFirstChild();
				if (!childTextNode==null && childTextNode->getNodeType() == DOMNode::TEXT_NODE)
				{
					const XMLCh* childTextValue = childTextNode->getNodeValue();
					char* transChildTextValue = XMLString::transcode(childTextValue);
					linkContextString = String(Cpd(transChildTextValue));
					idaTrackData(("TdfLinkContext: %s", linkContextString.cString()));
				}
			}
		}
	}


	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die folgende Funktion liefert zwei Werte aus dem DOM und zwar:
//		"ClientIpAddress" und "TdfLinkContext"
//
ReturnStatus TdfAccess::getTimeoutFromDom(DOMDocument* dom, ULong& timeout)
{
#ifdef CLASSLIB_03_00
	TRACE_FUNCTION("TdfAccess::getTimeoutFromDom(...)");
#endif	
	
	// Wir holen das Wurzel-Element
	DOMElement* root = dom->getDocumentElement();
	if (root==null) return isNotOk;

	DOMNodeList* childsFromRoot = root->getChildNodes();
	
	for (int i = 0; i < childsFromRoot->getLength(); i++)
	{
		// Knoten i holen
		DOMNode* child = childsFromRoot->item(i);

		// Koten-Typ prüfen
		if (DOMNode::ELEMENT_NODE == child->getNodeType())
		{
			// Namen des Knoten holen
			const XMLCh* childNodeName = child->getNodeName();
			char* transChildNodeName = XMLString::transcode(childNodeName);
			Cpd tagString(transChildNodeName);
			
			// <Timeout> gefunden ?
			if (0 == strcmp(tagString, "Timeout"))
			{
				DOMNode* childTextNode = child->getFirstChild();
				if (!childTextNode==null && childTextNode->getNodeType() == DOMNode::TEXT_NODE)
				{
					const XMLCh* childTextValue = childTextNode->getNodeValue();
					char* transChildTextValue = XMLString::transcode(childTextValue);
					String timeoutString = String(Cpd(transChildTextValue));

					// String nach ULong wandeln
					long tmpVal = 0;
					tmpVal = atol(timeoutString.cString());
					if (tmpVal > 0) timeout = tmpVal;
				}
			}
		}
	}


	return isOk;
}

ReturnStatus TdfAccess::terminateXMLPlatform()
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::terminateXMLPlatform(...)");
#endif

        try
        {
                XMLPlatformUtils::Terminate();
        }
        catch (const XMLException& toCatch)
        {
                idaTrackFatal(("XMLPlatformUtils::Terminate() failed !"));
                #ifdef MONITORING
                        cout << dbIdString << "XMLPlatformUtils::Terminate() failed !" << end
l;
                #endif

                return isNotOk;
        }

    idaTrackData(("XMLPlatformUtils::Terminate() ok"));
        return isOk;
}


ReturnStatus TdfAccess::createModifySpecification(ModifyRequest& modifyRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::createModifySpecification(...)");
#endif 

        ReturnStatus result = isOk;

        DOMNode* child;
        DOMNode* textNode;
        XMLCh* value;
        ULong enumVal;

        #ifdef XML_REQ_MON
                cout << dbIdString << "\tModifySpec" << endl;
        #endif

        DOMNodeList* nodeList = node->getChildNodes();

        if (nodeList->getLength() <= 0)
	{ 
		errorText = "XML:Request/ModifySpecification: missing parameter";
		return result;
	}

        for (int i = 0; i < nodeList->getLength(); i++)
        {
                child = nodeList->item(i);
                const XMLCh* childElement = child->getNodeName();
                char* transChildElement = XMLString::transcode(childElement);
                Cpd tagString(transChildElement);

                String stringValue("");

                // -------------------------------------------------------------------------
                // ModifySpecification/Operation:
                if (0 == strcmp(tagString, "Operation"))
                {
                        if (getValueOfDomNode(stringValue, child) == isOk)
                        {
                                if (StringToEnum::enumOfModifyOperation(enumVal, stringValue.cString()) == isOk)
                                {
                                        #ifdef XML_REQ_MON
                                                cout << dbIdString << "\t\tOperation = " << stringValue << endl;
                                        #endif
                                        modifyRequest.setOperation((ModifyRequest::Operation)enumVal);
                                        continue;
                                }
                        }
                        idaTrackExcept(("XML:Request/ModifySpecification/Operation: bad or missing value"));
                        errorText = "XML:Request/ModifySpecification/Operation: bad or missing value";
                        return isNotOk;
                }

                // -------------------------------------------------------------------------
                // ModifySpecification/LinkContext:
                if (0 == strcmp(tagString, "LinkContext"))
                {
                        if (getValueOfDomNode(stringValue, child) == isOk)
                        {
                                #ifdef XML_REQ_MON
                                        cout << dbIdString << "\t\tlinkContext = " << stringValue << endl;
                                #endif
                                modifyRequest.setLinkContext(hexToData(stringValue.cString()));
                        }

                        continue;
                }
	}

        return result;
}


ReturnStatus TdfAccess::createModifyAttributes(ModifyRequest& modifyRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::createModifyAttributes(...)");
#endif

        ReturnStatus result = isOk;
	SetOfDbAttribute        set;

        #ifdef XML_REQ_MON
                cout << dbIdString << "\tModifyAttributes" << endl;
        #endif

        DOMNode* child;
        ULong enumVal;

        idaTrackTrace(("TdfAccess::createModifyAttributes after DOMNode*"));

        // den ersten Unterknoten holen ("ModifyAttr")
        child = node->getFirstChild();
        if (child==null)
        {
                errorText = "missing modify attributes";
                return isNotOk;
        }
        idaTrackTrace(("TdfAccess::createModifyAttributes after child = node->getFirstChild()"));

        // -------------------------------------------------------------------------
        // Iteration über alle ModifyAttr-Knoten
        while (!child==null)
        {
          idaTrackTrace(("TdfAccess::createModifyAttributes after !child==null"));
                // ---------------------------------------------------------------------
                // Für alle Faelle pruefen wir mal, ob wir nicht etwas falsches bekommen haben
                const XMLCh* name = child->getNodeName();
                idaTrackTrace(("TdfAccess::createModifyAttributes after child->getNodeName()"));
                char* transName = XMLString::transcode(name);
                idaTrackTrace(("TdfAccess::createModifyAttributes after XMLString::transcode(name)"));
                if (0 != strcmp(Cpd(transName), "ModifyAttr"))
                {
                  idaTrackTrace(("TdfAccess::createModifyAttributes after strcmp(Cpd(transName)"));
                        // wenn doch, dann mit dem naechsten Knoten weiter machen
                        child = child->getNextSibling();
                        continue;
                }
                else{
                  idaTrackTrace(("TdfAccess::createModifyAttributes after else"));
#ifndef _LINUX
#ifndef _HP
        // leads to core on HP
        delete [] transName;
#endif
#endif

                }

                idaTrackTrace(("TdfAccess::createModifyAttributes before DOMNodeList* nodeList = child->getChildNodes()"));
                // ---------------------------------------------------------------------
                // Wenn der Knoten keine Kinder hat, machen wir auch mit dem nächsten weiter
                DOMNodeList* nodeList = child->getChildNodes();
                if (0 == nodeList->getLength())
                {
                  child = child->getNextSibling();
                  continue;
                }

                #ifdef XML_REQ_MON
                cout << dbIdString << "\t\tModifyAttr" << endl;
                #endif

                idaTrackTrace(("TdfAccess::createModifyAttributes before  for (int i = 0; i < nodeList->getLength(); i++)"));
                // ---------------------------------------------------------------------
                // Wir haben eine SearchAttr Knoten gefunden, der auch Kinder hat.
                // Es folgt eine Iteration über alle Kinder
                DbAttribute attr;
                attr.reset();
                ULong variation = 0;                    // für die Variation
                Bool legalAttribut = false;             // Flag zur Schleifensteuerung

                for (int i = 0; i < nodeList->getLength(); i++)

                {
                        DOMNode* childChild = nodeList->item(i);
                        const XMLCh* childName = childChild->getNodeName();
                        char* transChildName = XMLString::transcode(childName);
                        Cpd tagString(transChildName);
                        String stringValue("");

                        // -----------------------------------------------------------------
                        // Attribut:
                        if (0 == strcmp(tagString, "Attribute"))
                        {
                                if (getValueOfDomNode(stringValue, childChild) == isOk)
                                {
                                        if (StringToEnum::enumOfDbAttribute(enumVal, stringValue.cString()) == isOk)
                                        {
                                                #ifdef XML_REQ_MON
                                                        cout << dbIdString << "\t\t\tAttribute = " << stringValue << endl;
                                                #endif
                                                attr.setAttributeId((DbAttribute::AttributeId)enumVal);
                                                 idaTrackData(("XML:Request/ModifyAttributes/Attribute: "));
                                                // Der Attribut-Name war gueltig
                                                legalAttribut = true;
                                                continue;
                                        }
					else
					{
						errorText = String("unknown attribute ") + stringValue;
						return isNotOk;  // unknown attribute, error according to Yu
					}
                                }
                                idaTrackExcept(("XML:Request/ModifyAttributes/Attribute: bad or missing value: %s", stringValue.cString()));
                                //break;
                        }
                        // -----------------------------------------------------------------
                        // Value:
                        if (0 == strcmp(tagString, "Value"))
                        {
                                if (getValueOfDomNode(stringValue, childChild) == isOk)
                                {
                                        #ifdef XML_REQ_MON
                                                cout << dbIdString << "\t\t\tValue = " << stringValue.cString() << endl;
                                        #endif
                                        attr.setData(stringValue.cString());
                                        idaTrackData(("XML:Request/ModifyAttributes/Attribute/Value : %s", stringValue.cString()));
                                        continue;
                                }
                                idaTrackExcept(("XML:Request/ModifyAttributes/Attribute/Value: bad or missing value"));

                        }
                }
                // ---------------------------------------------------------------------
                // Wenn ein gültiges Attribut gefunden wurde ...
                if (legalAttribut)
                {
                        set.insert(attr);
                }


                child = child->getNextSibling();
        }

        modifyRequest.setSetOfDbAttribute(set);

        return result;
}


ReturnStatus TdfAccess::createModifyFlags(ModifyRequest& modifyRequest, DOMNode* node, String& errorText)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::createModifyFlags(...)");
#endif 

        ReturnStatus result = isOk;

        DOMNode* child;
        DOMNode* textNode;
        XMLCh* value;
        ULong enumVal;
	ULong flags = 0;
	ULong flagsMask = 0;

        #ifdef XML_REQ_MON
                cout << dbIdString << "\tModifyFlags" << endl;
        #endif

        DOMNodeList* nodeList = node->getChildNodes();

        if (nodeList->getLength() <= 0)
	{
        	errorText = "XML:Request/ModifyFlags: missing parameter";
		return isNotOk;
	}

	clTrackTrace(("createModifyFlags searching for <Flags>"));

        for (int i = 0; i < nodeList->getLength(); i++)
        {
                child = nodeList->item(i);
                const XMLCh* childElement = child->getNodeName();
                char* transChildElement = XMLString::transcode(childElement);
                Cpd tagString(transChildElement);

                String stringValue("");

                // -------------------------------------------------------------------------
                // ModifyRequest/Flags:
                if (0 == strcmp(tagString, "Flags"))
		{
		  clTrackTrace(("createModifyFlags <Flags> found"));

		  DOMNodeList* nodeList = child->getChildNodes();

        	  if (nodeList->getLength() <= 0) return result;

		  for ( int f = 0; f < nodeList->getLength(); ++f )
		  {
		    DOMNode * flag = nodeList->item(f);
		    const XMLCh * childElement = flag->getNodeName();
                    char* transChildElement = XMLString::transcode(childElement);

		    if ( 0 == strcmp(transChildElement, "Flag") )
		    {
		    	clTrackTrace(("createModifyFlags <Flag> found"));
			String name;
		  	ULong value = 0;
			Bool nameSet = false;
			Bool valueSet = false;

		  	DOMNodeList* nodeList = flag->getChildNodes();

		  	for (int j = 0; j < nodeList->getLength(); j++)
		  	{
		  	  DOMNode * sub = nodeList->item(j);
		  	  const XMLCh* childElement = sub->getNodeName();
                  	  char* transChildElement = XMLString::transcode(childElement);
			  String strValue;
		    
		  	  if ( 0 == strcmp(transChildElement, "Name") )
		  	  {
		    		clTrackTrace(("createModifyFlags <Name> found"));
				if (getValueOfDomNode(name, sub) == isOk)
				{
				  nameSet = true;
				  idaTrackTrace(("createModifyFlags: got name %s from Node", name.cString() ));
				}
		  	  }	
		    	  else if  ( 0 == strcmp(transChildElement, "Value") )
		  	  {
		    		clTrackTrace(("createModifyFlags <Value> found"));
				if (getValueOfDomNode(strValue, sub) == isOk)
				{
				  if ( DecString(strValue).getValue(value) == isOk )
				  {
				    valueSet = true;
				    idaTrackTrace(("createModifyFlags: got value %ld from Node", (ULong)value ));
				  }
				}
			  }
			} // for j

			if ( nameSet && valueSet )
			{
			  IndentRecord::Flags mask = (IndentRecord::Flags)0;

			  if ( stringToEnum(name.cString(), mask) != isOk )
			  {
			    idaTrackExcept(("createModifyFlags: %s is unknown", name.cString() ));
			    errorText = String(" unknown flag: ") + name;
			    result = isNotOk;
		          }
		          else // set the flag value and mask:
			  {
			    if ( value )
			    {
				flags |= (ULong) mask;
			    }

			    flagsMask |= (ULong) mask;	
			
			    idaTrackTrace(("createModifyFlags: setting flag %s to %ld", name.cString(), (ULong)value));    
			  }
			}
			else // missing name or value
		        {
			  idaTrackExcept(("createModifyFlags: name and/or value in Flag missing"));
			  errorText = "name and/or value in Flag missing";
			  result = isNotOk;
			}
		    } // if Flag
		  } // for f 
		} // if Flags
	} // for i

	modifyRequest.setFlags(flags);
	modifyRequest.setFlagsMask(flagsMask);

	return result;
}


Void TdfAccess::convModifyResponseToXml(ModifyResponse & modifyResponse, String & xml, String & typeString)
{
#ifdef CLASSLIB_03_00
        TRACE_FUNCTION("TdfAccess::convModifyResponseToXml(...)");
#endif

        VisibleString visibleString;
	Bool isError = false;

        // -----------------------------------------------------------------------------
        // ModifyResult
        xml.cat("<ModifyResult>");

	if (modifyResponse.isErrorSourceSet())
        {
                enumToString(modifyResponse.getErrorSource(), visibleString);
                createXmlNode(xml, tagErrorSource, visibleString.getDataString());
		isError = true;
        }

        if (modifyResponse.isErrorCodeSet())
        {
                //errorFlag = true;
                enumToString(modifyResponse.getErrorCode(), visibleString);
                createXmlNode(xml, tagErrorCode, visibleString.getDataString());
		isError = true;
        }

        xml.cat("</ModifyResult>");

	if ( isError )
	{
	  typeString.assign("error");
	}
	else
	{
	  typeString.assign("modify");
	}
}

// *** EOF ***

