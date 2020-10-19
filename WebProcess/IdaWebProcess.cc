//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  WebProcess/IdaWebProcess.cc 1.0 12-APR-2008 18:52:14 DMSYS
//
//   File:      WebProcess/IdaWebProcess.cc
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:14
//
//   DESCRIPTION:
//     IDA.plus Web interface
//
//
//
//<CE-------------------------------------------------------------------
static const char * SCCS_Id_IdaWebProcess_cc = "@(#) WebProcess/IdaWebProcess.cc 1.0 12-APR-2008 18:52:14 DMSYS";


#include <IdaDecls.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <decstring.h>
#include <unistd.h>
#include <apptimer.h>
#include <idatraceman.h>
#include <pcpdefs.h>
# ifdef CLASSLIB_03_00
  #include <pcpdispatcher.h>
  #include <pcpeventdispatcher.h>
# else
  #include <dispatcher.h>
  #include <eventdispatcher.h>
# endif
#include <IdaFunctionTrace.h>
#include <pcpstring.h>
#include <pcptime.h>
#include <toolbox.h>
#include <pcpostrstream.h>
#include <syspoll.h>


#include <IdaTypes.h>
#include <IdaDatabaseList.h>
#include <IdaWebProcess.h>



static char idaGroupName[]			= "ida";
static String requestTerminationTag1("</Request>");
static String requestTerminationTag2("</Login>");
static String requestTerminationTag3("</Logout>");
static String requestTerminationTag4("</ChangePwd>");
static String requestTerminationTag5("</ModifyRequest>");
static String requestStartTag1("<Request>");
static String requestStartTag2("<Login>");
static String requestStartTag3("<Logout>");
static String requestStartTag4("<ChangePwd>");
static String requestStartTag5("<ModifyRequest>");


#include <IdaXmlTags.h>



// Wenn die Ausgaben nach "cout" unterbleiben sollen, dann diese Zeile auskommentieren !
#ifdef ALLOW_STDOUT
	#define MONITORING
#endif


#define PROBLEM(a,b,c,d) reporter()->reportProblem((a),(b),(c),(d))
#define ALARM(a,b,c,d) reporter()->reportAlarm((a),(b),(c),(d))
#define EVENT(a,b,c,d) reporter()->reportEvent((a),(b),(c),(d))




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Konstruktor
//
WebProcess::WebProcess(
	Int						argc,
	Char**					argv,
	Char**					envp,
	RefId					objectId,
	UShort					webProcessNo,
	DatabaseList&			dbList,
	int						socketNr,
	UShort					sTimeout)
:
	Process			(argc, argv, envp),// , processOID, idaGroupName ),
	ownObjectId     (objectId),
	objectIdOffset	(webProcessNo),
	socketPortNr    (socketNr),
	searchTimeout	(sTimeout),
	databaseList	(dbList),
	clientIpAddress ((char *) 0),
	timerId         (0),
	requestCounter	(0),
	connectionFlag	(false),
	requestId		(0),
	pReporterClient (0)
{
	idaTrackTrace(("Constructor WebProcess -IN-"));
	
   // content moved to init method since it can fail!
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Destruktor
//
WebProcess::~WebProcess()
{
	idaTrackTrace(("Destructor WebProcess -IN-"));

	EVENT(ownObjectId, iDAMinRepClass, 202," IdaWebProcess");
#ifdef CLASSLIB_03_00
	// Check out socket
	eventDispatcher->checkOut(serverSocket.getListenFd());
	// Test: Checkout Signal
	comManNoRouter->signalCheckOut(ownObjectId, SIGPIPE);

	// Check out classlib ID's
	comManNoRouter->dispatcherCheckOut(ownObjectId);
	comManNoRouter->checkOut(ownObjectId);

	shutdownComplete(ownObjectId);
# else

	 delete eventDispatcher;
# endif

	idaTrackTrace(("Destructor WebProcess -OUT-"));
}

ReturnStatus WebProcess:: init ()
{
	idaTrackTrace(("WebProcess: init -IN-"));
	
	// -------------------------------------------------------------------------
	// Einchecken des Prozesses beim Kommunikations-Manager
	// damit wir Messages vom Dispatcher bekommen ( via messageBox() )
# ifdef CLASSLIB_03_00
	comManNoRouter = Process::getComManNoRouter();
	comManNoRouter->checkIn(ownObjectId);
	idaTrackTrace(("Constructor WebProcess ownObjectId = %d", ownObjectId));
	comManNoRouter->dispatcherCheckIn(ownObjectId, this);
	// -------------------------------------------------------------------------
	// Einchecken des Prozesses beim Kommunikations-Manager
	// damit wir Events bekommen ( via eventBox() )
	// Test: Handle SIGPIPE
	comManNoRouter->signalCheckIn(ownObjectId, SIGPIPE);
# else
 	comMan = Process::getComMan();
        comMan->checkIn(ownObjectId);
        comMan->dispatcherCheckIn(ownObjectId, this);

        // -------------------------------------------------------------------------
        // Einchecken des Prozesses beim Kommunikations-Manager
        // damit wir Events bekommen ( via eventBox() )
        // Test: Handle SIGPIPE
        comMan->signalCheckIn(ownObjectId, SIGPIPE);
# endif

	// Init Socket
	idaTrackData(("Socket port #: %d", socketPortNr));

	if (serverSocket.initForConnect(socketPortNr) == isNotOk)
	{
		idaTrackFatal(("socket.initForConnect failed!"));
		return isNotOk;
	}


# ifdef CLASSLIB_03_00
	dispatcher = comManNoRouter->getDispatcher();
	eventDispatcher =  comManNoRouter->getEventDispatcher();
	// ab diesem Zeitpunkt kann die Methode eventBox() aufgerufen werden
	// Fd bedeutet FileDescriptor
	// "pollIn" : beobachte Pollin-Ereignisse

	eventDispatcher->checkIn(serverSocket.getListenFd(), pcpReadMask, this);
# else
        eventDispatcher = new EventDispatcher(comMan);
        // Registriert den ListenSocket beim Event-Dispatcher
        // ab diesem Zeitpunkt kann die Methode eventBox() aufgerufen werden
        // Fd bedeutet FileDescriptor
        // "pollIn" : beobachte Pollin-Ereignisse
        eventDispatcher->checkInFd(serverSocket.getListenFd(), pollIn, this);
#endif

	initComplete();


	// -------------------------------------------------------------------------
	EVENT(ownObjectId, iDAMinRepClass, 201, " IdaWebProcess");
	idaTrackTrace(("Constructor WebProcess -OUT-"));

	#ifdef MONITORING
		cout << "myObjectId        = " << ownObjectId << endl;
		cout << "socketPortNr      = " << socketPortNr << endl;
		cout << "build 0003" << endl;
		cout << "------------------------------------------------------" << endl;
	#endif

		
	reqCounter			= 0;
	requestDauer		= 0;
	requestDauerMin		= 1000000;
	requestDauerMax		= 0;
	responseCounter		= 0;
	responseDauer		= 0;
	responseDauerMin	= 1000000;
	responseDauerMax	= 0;
	gesamtCounter		= 0;
	gesamtDauer			= 0;
	gesamtDauerMin		= 1000000;
	gesamtDauerMax		= 0;
	retryCounter		= 0;

   return isOk;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Starten der Messagebehandlung durch die CLASSLIB
//
Void WebProcess::run()
{
    TRACE_FUNCTION("WebProcess::run(...)");
	
	idaTrackTrace(("WebProcess::run"));
# ifdef CLASSLIB_03_00
	dispatcher->dispatchForever();
# else
	eventDispatcher->dispatchForever();
# endif
	EVENT(ownObjectId, iDAMinRepClass, 203," IdaWebProcess");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn eine Anforderung vom WEB Server (Servlet) kommt, wird die Applikation
//	(bzw. dieser Prozeß) durch Aufruf der Methode eventBox davon benachrichtigt
//
//	Die folgende Zeile kann aktiviert werden, um für Testzwecke den Loopback
//	auf WebProcess-Ebene zu erreichen. Dabei wird der empfangene Request
//	einfach sofort zum Client zurück gesendet
//
//	Sonderfälle, die zu behandeln sind:
//
//	1. Der Client schließt vorzeitig den Socket,
//	   und öffnet ihn dann sofort wieder für einen
//	   neuen Request
//	2. Ein Zweiter Client versucht einen parallele
//	   Anfrage bei einem WebProzess, der bereits seinen
//	   StreamSocket in Betrieb (geöffnet) hat
//
#ifdef CLASSLIB_03_00
ReturnStatus WebProcess::handleEvent( PcpReturnEvent & event)
{
    TRACE_FUNCTION("WebProcess::handleEvent(...)");
	idaTrackTrace(("Event received"));

	long fileDescriptor = event.getSignaledHandle();
   Int  myError = event.getConnectError ();
#else
ReturnStatus WebProcess::eventBox(PollEvent event)
{
    TRACE_FUNCTION("WebProcess::eventBox(...)");
	idaTrackTrace(("Event received"));

	long fileDescriptor = event.getFdsQid();
   Int  myError = event.getError();
#endif


	// Sollte auch nicht auftreten:
	if (myError != 0)
	{
		idaTrackExcept(("Event error"));
		#ifdef MONITORING
			cout << "\n*** EventError #" << myError << " occured" << endl;
		#endif
		return isNotOk;
	}
	idaTrackData(("***** no pollEvent error detected"));


	// -------------------------------------------------------------------------
	// Am File-Descriptor können wir erkennen, für welchen Socket
	// das Event galt ...
	if (fileDescriptor == serverSocket.getListenFd())
	{
		#ifdef MONITORING
			timeStamp1 = Time();
		#endif
		idaTrackData(("***** event for server socket"));
		handleServerSocketEvent();

		{
		  PcpTime time;

		  idaTrackData(("*** Server Socket Event at        %d:%d:%d,%d",
					   time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
		}
		
	}
	else if (fileDescriptor == streamSocket.getSockFd())
	{
		idaTrackData(("***** event for stream socket"));
		handleStreamSocketEvent();
	}
	else
	{
		idaTrackExcept(("***** unknown event received"));
		#ifdef MONITORING
			cout << "\nunknown event received" << endl;
		#endif
	}

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn ein Event für den Server-Socket eintrifft, dann versucht ein Client seinerseits eine
//	Socket-Verbindung zu öffnen
//
Void WebProcess::handleServerSocketEvent()
{
    TRACE_FUNCTION("WebProcess::handleServerSocketEvent(...)");
	
	
	// ---------------------------------------------------------------------
	// Nur eine Verbindung ist zu einem Zeitpunkt zulässig
	if (connectionFlag)
	{
		idaTrackExcept(("Additional connection refused"));
		#ifdef MONITORING
			cout << "*** Zusätzliche Verbindung abgelehnt !" << endl;
		#endif
		refuseAdditionalConnection();
		return;
	}
	idaTrackTrace(("***** connectionFlag was false"));


	// ---------------------------------------------------------------------
	// Socketverbindung herstellen 
	if (serverSocket.connect(true) == isOk)
	{
		// es hat geklappt, wir können uns den StreamSocket holen
		#ifdef MONITORING
			cout << "[S>>|" << flush;
		#endif
		
		// Wir spalten einen StreamSocket ab ...
		streamSocket = serverSocket;
		idaTrackExcept(("***** streamSocket created"));


		// ... und checken ihn ein, damit wir entsprechende Event erhalten
#ifdef CLASSLIB_03_00
		if (eventDispatcher->checkIn(streamSocket.getSockFd(), pcpReadMask, this) == isNotOk)
# else
		if (eventDispatcher->checkInFd(streamSocket.getSockFd(), pollIn, this) == isNotOk)
# endif
		{
			idaTrackFatal(("checkInFd(streamSocket...) failed!"));
			#ifdef MONITORING
				cout << "*** checkInFd(streamSocket...) failed" << endl;
			#endif
			connectionFlag = false;
		}
		else
		{
			// Variable zum Speichern des gesamten Requestes leeren
			requestString = "";
			// Der Request wurde noch nicht abgeschlossen
			requestCompleted = false;
			// Die Verbindung besteht
			connectionFlag = true;
			idaTrackExcept(("***** streamSocket checked in, connectionFlag = true"));
		}

	}
	else
	{
		// Fehler:
		// Leider können wir kein Fehlerdokument zurück senden
		idaTrackExcept(("***** socket.connect() failed!"));
		#ifdef MONITORING
			cout << "*** socket.connect() failed" << endl;
		#endif
		connectionFlag = false;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn die Verbindung zum Client steht löst das senden der Daten (vom Client zum WebProcess) 
//	ein Event für den Stream-Socket aus der hier behandelt wird
//
Void WebProcess::handleStreamSocketEvent()
{
    TRACE_FUNCTION("WebProcess::handleStreamSocketEvent(...)");
	
	
	// ---------------------------------------------------------------------
	// Request-String (XML) vom Socket lesen
	// Hier wird geprüft, dass die Ergebnislänge kleiner als die Bufferlänge ist
	ssize_t	receivedBytes;
//svc	streamSocket.setBlocking(false);
	streamSocket.setBlocking(true);
	if (streamSocket.readStream(buffer, Types::MAX_LEN_QUERY_STRING - 1, receivedBytes) == isOk) 
	{
		idaTrackTrace(("***** reading from stream ok !"));

		// Bei EOF ist die Länge 0
		// (Der Client hat den Socket geschlossen)
		if (receivedBytes == 0)
		{
			idaTrackExcept(("***** 0 bytes received, = transmition end"));
			#ifdef MONITORING
				cout << "receivedLength == 0" << endl;
			#endif
# ifdef CLASSLIB_03_00
			eventDispatcher->checkOut(streamSocket.getSockFd());
# else
			 eventDispatcher->checkOutFd(streamSocket.getSockFd());
# endif
			streamSocket.closeSocket();

			stopTimer(timerId);
			connectionFlag = false;
			#ifdef MONITORING
				cout << "connection closed by Client" << endl;
			#endif
			idaTrackExcept(("***** streamSocket closed, checked out, timer stopped, connectionFlag = false"));
		}
		else
		{
			// Null-Byte Begrenzung
			buffer[receivedBytes] = 0;
			if (!requestCompleted)
			{
				// Der nächste Teil der Nachricht wird angehängt
				requestString.cat(String(buffer));
				// Wenn der Request komplett ist ...
				// (Kriterium hierfür ist die Existenz des schließenden XML-Tags)
				UInt index;
				if (   requestString.isSubStrInStr(requestTerminationTag1, index)
					|| requestString.isSubStrInStr(requestTerminationTag2, index)
					|| requestString.isSubStrInStr(requestTerminationTag3, index)
					|| requestString.isSubStrInStr(requestTerminationTag4, index)
					|| requestString.isSubStrInStr(requestTerminationTag5, index)
					)
				{
					idaTrackData(("Request received:\n%s", requestString.cString()));
					EVENT(ownObjectId, iDAMinRepClass, 204, "");
					// Der Request ist vollständig
					requestCompleted = true;
					// Request weiter schicken
					sendRequestToTdfClient(requestString);
				}
				else if (   requestString.isSubStrInStr(requestStartTag1, index)
					|| requestString.isSubStrInStr(requestStartTag2, index)
					|| requestString.isSubStrInStr(requestStartTag3, index)
					|| requestString.isSubStrInStr(requestStartTag4, index)
					|| requestString.isSubStrInStr(requestStartTag5, index)
					)
				{
					// OK
				}
				else // no Request Start tag found, illegal request -> close
				{
					idaTrackExcept(("no valid request start tag found -> close"));
# ifdef CLASSLIB_03_00
					eventDispatcher->checkOut(streamSocket.getSockFd());
# else
			 		eventDispatcher->checkOutFd(streamSocket.getSockFd());
# endif
					streamSocket.closeSocket();

					stopTimer(timerId);
					connectionFlag = false;
					idaTrackExcept(("***** socket checked out, closed !"));
				}
			}
		}
	}
	// -------------------------------------------------------------------------
	// streamSocket.readStream(...) == isNotOk
	else
	{
		idaTrackExcept(("socket.readStream() failed! Client closed connection ?"));
		idaTrackTrace(("socket.readStream() receivedBytes = %d", receivedBytes)); 
		PROBLEM(ownObjectId, iDAMinRepClass, 103, "");
		#ifdef MONITORING
			cout << "ConnectionError(" << streamSocket.getError() << ")|" << flush;
		#endif
# ifdef CLASSLIB_03_00
		eventDispatcher->checkOut(streamSocket.getSockFd());
# else
		eventDispatcher->checkOutFd(streamSocket.getSockFd());
# endif
		streamSocket.closeSocket();
		idaTrackExcept(("***** socket checked out, closed !"));
		stopTimer(timerId);
		connectionFlag = false;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Sollte ein Client versuchen eine Verbindung zu einem WebProcess aufzunehmen, bei dem bereits
//	eine Verbindung besteht, dann wird mit dieser Methode der Verbindungswunsch abgelehnt
//
Void WebProcess::refuseAdditionalConnection()
{
    TRACE_FUNCTION("WebProcess::refuseAdditionalConnection(...)");
	
	
	idaTrackExcept(("I'm busy, refuse additional connection !"));
	PROBLEM(ownObjectId, iDAMinRepClass, 119, "");
	#ifdef MONITORING
		cout << "refuse additional connection" << endl;
	#endif

	
	StreamSocketData tmpStreamSocket;

	// ---------------------------------------------------------------------
	// Socketverbindung herstellen 
	if (serverSocket.connect(true) == isOk)
	{
		idaTrackData(("***** serverSocket connect ok, ..."));
		// Wir spalten einen StreamSocket ab ...
		tmpStreamSocket = serverSocket;

		idaTrackData(("***** temp. streamSocket created"));
		// Fehlermeldung vorbereiten
		String xmlString;
		formatXMLErrorResponse(xmlString,
							   String("busy"),
							   String("the server does not handle multiple connections"));
		
		// Nachricht senden
		ssize_t bytesSend;
		idaTrackData(("***** sending error document"));

		if (tmpStreamSocket.writeStreamFixLen(xmlString.cString(), xmlString.cStringLen(),
										 bytesSend) == isNotOk)
		{
			idaTrackExcept(("***** writing error document via socket failed"));
		}

		tmpStreamSocket.closeSocket();
		idaTrackExcept(("***** temp. socket closed !"));
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Hier wird der Request an den TdfClient weitergesandt. Die Entscheidung, welcher TdfClient die
//	Nachricht erhält, wird ebenfalls hier getroffen
//
ReturnStatus WebProcess::sendRequestToTdfClient(const String& queryStr)
{
    TRACE_FUNCTION("WebProcess::sendRequestToTdfClient(...)");
	
	// initiale Anfrage -> retryCounter auf 0
	retryCounter = 0;

	// -------------------------------------------------------------------------
	// Wir benötigen die ID der Datenbank, die angesprochen werden soll
	RefId dbId = -1;
	// ... holen diese aus dem Request
	getValueOfXMLElement(queryStr.cString(), "DBID", dbId);
	// Anhand der DBID können wir die zugehörige ObjectID des entsprechenden
	// TdfAccess-Objektes ermitteln
	ObjectId destOid(databaseList.findOIDofDB(dbId));
	// Wenn die ObjectId -1 ist, ...
	if (destOid.getIdentifier() == -1)
	{
		// ... dann ist ein Fehler aufgetreten
		idaTrackTrace(("findOIDofDB() failed"));
		
		// Ohne DBID könne wir nicht fortfahren
		// also Fehlerdokument senden
		String xmlString;
		formatXMLErrorResponse(xmlString, String("parameterError"), String("Wrong or missing 'DBID' value"));
		sendViaSocketAndClose(xmlString);

		return isOk;
	}

	
	// -------------------------------------------------------------------------
	// Timeout Wert aus dem Request holen
	long timeOut = -1;
	if (getValueOfXMLElement(queryStr.cString(), "Timeout", timeOut) == isNotOk)
	{
		idaTrackTrace(("findOIDofDB() failed --> default value"));
		
		// Wir nehemen den Default-Wert
		timeOut = searchTimeout;
	}
	if (timeOut <= 100) timeOut = 100;	// = 100 Milli-Sekunden


	// -------------------------------------------------------------------------
	// Vorbereiten der Message an den TdfClient
	Message msg;
	msg.setMsgRef(getNextRequestId());
	
	msg.setDestinationOid(destOid.getIdentifier());
	msg.setSourceOid(ownObjectId);
	msg.setData(queryStr.cString(), queryStr.cStringLen());
	msg.setMsgType(Types::XML_REQUEST_MSG);
	
# ifdef CLASSLIB_03_00
	if ( comManNoRouter->sendDup(msg) == isNotOk)
# else
	if (comMan->sendDup(msg) == isNotOk)
# endif
	{
		idaTrackTrace(("sendDup failed"));
		// Fehlerdokument senden
		String xmlString;
		formatXMLErrorResponse(xmlString, String("commError"), String("sendDup failed"));
		sendViaSocketAndClose(xmlString);
		return isNotOk;
	}

	#ifdef MONITORING
		cout << "R(" << requestId << ")>>T|" << flush;
	#endif

	{
	  PcpTime time;

	  idaTrackData(("*** Request send to TdfProcess at %d:%d:%d,%d",
						 time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
	}
	
		
	// -------------------------------------------------------------------------
	// Timer aufziehen
	startTimer(timerId, timeOut, PcpTimerInterface::once);
	idaTrackTrace(("***** start timer !"));

	#ifdef MONITORING
		{
			++reqCounter;
			RelTime delta = Time() - timeStamp1;
			ULong dauer = delta.inMilliSeconds();
			requestDauer += dauer;
			if (dauer < requestDauerMin) requestDauerMin = dauer;
			if (dauer > requestDauerMax) requestDauerMax = dauer;
			if (reqCounter >= 100)
			{
				cout << "\tRequest  min/avg/max [ms] : " 
					 << requestDauerMin << "/"
					 << requestDauer / reqCounter << "/"
					 << requestDauerMax
					 << endl;
				requestDauerMin = 1000000;
				requestDauerMax = 0;
				requestDauer = 0;
				reqCounter = 0;
			}
		}
	#endif
	
	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//      Hier wird der Request an den TdfClient weitergesandt. Die Entscheidung, welcher TdfClient die
//      Nachricht erhält, wird ebenfalls hier getroffen
//
ReturnStatus WebProcess::sendRequestToBackupTdfClient(const String& queryStr)
{
    TRACE_FUNCTION("WebProcess::sendRequestToBackupTdfClient(...)");


        // -------------------------------------------------------------------------
        // Wir benötigen die ID der Datenbank, die angesprochen werden soll
        RefId dbId = -1;
        // ... holen diese aus dem Request
        getValueOfXMLElement(queryStr.cString(), "DBID", dbId);
        // Anhand der DBID können wir die zugehörige BackupObjectID des entsprechenden
        // TdfAccess-Objektes ermitteln
        ObjectId destOid(databaseList.findBackupOIDofDB(dbId));
        // Wenn die ObjectId -1 ist, ...
        if (destOid.getIdentifier() == -1)
        {
                // ... dann ist ein Fehler aufgetreten
                idaTrackTrace(("findBackupOIDofDB() failed"));

                // Ohne DBID könne wir nicht fortfahren
                // also Fehlerdokument senden
                String xmlString;
                formatXMLErrorResponse(xmlString, String("parameterError"), String("Wrong or missing 'DB ID' value"));
                sendViaSocketAndClose(xmlString);

                return isOk;
        }


        // -------------------------------------------------------------------------
        // Timeout Wert aus dem Request holen
        long timeOut = -1;
        if (getValueOfXMLElement(queryStr.cString(), "Timeout", timeOut) == isNotOk)
        {
                idaTrackTrace(("findOIDofDB() failed --> default value"));

                // Wir nehemen den Default-Wert
                timeOut = searchTimeout;
        }
        if (timeOut <= 100) timeOut = 100;      // = 100 Milli-Sekunden


        // -------------------------------------------------------------------------
        // Vorbereiten der Message an den TdfClient
        Message msg;
        msg.setMsgRef(getNextRequestId());

        msg.setDestinationOid(destOid.getIdentifier());
        msg.setSourceOid(ownObjectId);
        msg.setData(queryStr.cString(), queryStr.cStringLen());
        msg.setMsgType(Types::XML_REQUEST_MSG);

# ifdef CLASSLIB_03_00
        if ( comManNoRouter->sendDup(msg) == isNotOk)
# else
        if (comMan->sendDup(msg) == isNotOk)
# endif
        {
                idaTrackTrace(("sendDup failed"));
                // Fehlerdokument senden
                String xmlString;
                formatXMLErrorResponse(xmlString, String("commError"), String("sendDup failed"));
                sendViaSocketAndClose(xmlString);
                return isNotOk;
        }

        #ifdef MONITORING
                cout << "R(" << requestId << ")>>T|" << flush;
        #endif

        {
          PcpTime time;

          idaTrackData(("*** Request send to TdfProcess at %d:%d:%d,%d",
             time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
        }


        // -------------------------------------------------------------------------
        // Timer aufziehen
        startTimer(timerId, timeOut, PcpTimerInterface::once);
        idaTrackTrace(("***** start timer !"));

        #ifdef MONITORING
                {
                        ++reqCounter;
                        RelTime delta = Time() - timeStamp1;
                        ULong dauer = delta.inMilliSeconds();
                        requestDauer += dauer;
                        if (dauer < requestDauerMin) requestDauerMin = dauer;
                        if (dauer > requestDauerMax) requestDauerMax = dauer;
                        if (reqCounter >= 100)
                        {
                                cout << "\tRequest  min/avg/max [ms] : "
                                         << requestDauerMin << "/"
                                         << requestDauer / reqCounter << "/"
                                         << requestDauerMax
                                         << endl;
                                requestDauerMin = 1000000;
                                requestDauerMax = 0;
                                requestDauer = 0;
                                reqCounter = 0;
                        }
                }
        #endif

        return isOk;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Standard Empfangsroutine nach dem CLASSLIB Framwork
//
# ifdef CLASSLIB_03_00
  ReturnStatus WebProcess::messageBox(Message& msg)
# else
  ReturnStatus WebProcess::messageBox(Message*& msg)
# endif
{
    TRACE_FUNCTION("WebProcess::messageBox(...)");
	

	Types::TransferHeader header;

	Char* data = 0;
	ULong dataLen = 0;

#ifdef CLASSLIB_03_00
	switch (msg.getMsgType())
# else
	switch (msg->getMsgType())
#endif
	{
		// ---------------------------------------------------------------------
		// Beenden der Prozeßaktivität und runterfahren
		case comSysMsg:
#ifdef CLASSLIB_03_00
			if (msg.getOperationId() == initShutdown)
# else
			if (msg->getOperationId() == initShutdown)
#endif
			{
				idaTrackTrace(("received shutdown from supervisor"));
				stopTimer(timerId);
				// Check out socket
# ifdef CLASSLIB_03_00
				eventDispatcher->checkOut(serverSocket.getListenFd());
				comManNoRouter->signalCheckOut(ownObjectId, SIGPIPE);
				eventDispatcher->stopDispatchForever();
				comManNoRouter->dispatcherCheckOut(ownObjectId);
				comManNoRouter->checkOut(ownObjectId);
# else
				eventDispatcher->checkOutFd(serverSocket.getListenFd());
				comMan->signalCheckOut(ownObjectId, SIGPIPE);
				eventDispatcher->stopDispatchForever();
				comMan->dispatcherCheckOut(ownObjectId);
				comMan->checkOut(ownObjectId);
# endif
				shutdownComplete(ownObjectId);
			}
			break;

		// ---------------------------------------------------------------------
		// Behandlung der neuen Nachrichten für IDA 2.0
		// Eine gesamte Nachricht kommt in mehreren Teilen, eine Message pro
		// Teil, an. Die einzelnen Teile sind Null-terminierte Teilstrings und
		// müssen hier wieder zusammengesetzt werden. Insgesamt ergibt sich dann
		// ein vollständiges XML-Dokument
		case Types::XML_RESPONSE_MSG :
			// Alle Messages mit falscher requestId werden verworfen
# ifdef CLASSLIB_03_00
			if (msg.getMsgRef() == requestId)
# else
			if (msg->getMsgRef() == requestId)
# endif
			{
				char* headerAndString;
				ULong len;
# ifdef CLASSLIB_03_00
				msg.getDataPtr(headerAndString, len);
# else
				msg->getDataPtr(headerAndString, len);
# endif

				memcpy(&header, headerAndString, sizeof(header));
				headerAndString += sizeof(header);

				// -------------------------------------------------------------
                // Fall 1: Erstes Paket (bzw. erstes und letztes)
				if (header.blockNumber == 0)
				{
					#ifdef MONITORING
						timeStamp2 = Time();
					#endif
					#ifdef MONITORING
						cout << "T>>R(" << msg.getMsgRef() << ")." << flush;
					#endif
					msgString = "";
					msgString.cat(headerAndString);
					
					// Wenn das Ende-Flag gesetzt ist, dann gab es nur ein Paket
					// und das vollständige Dokument kann weiter geschickt werden
					if (header.endFlag)
					{
						idaTrackData(("receiving XML document from TdfAccess complete:\n%s", msgString.cString()));
						EVENT(ownObjectId, iDAMinRepClass, 207, "");
						// ... und wieder den Timer löschen
						stopTimer(timerId);

						// senden ...
						sendViaSocketAndClose(msgString);
						
						// requestId weiterschalten
						++requestId;
					}
					blockNumberCounter = 1;	// Zähler auf nächsten Block setzen
				}
				// -------------------------------------------------------------
				// Fall 2: letzes Paket
				else if (header.endFlag)
				{
					// Wenn die Blocknummer nicht stimmt (Reihenfolgefehler oder
					// verloren gegangener Block) dann melden wir eine Fehler
					if (header.blockNumber != blockNumberCounter)
					{
						String xmlString;
						formatXMLErrorResponse(xmlString,
											   String("messageOrderError"),
											   String("Message with wrong blockNumber received"));
						sendViaSocketAndClose(xmlString);
						++requestId;
						break;
					}
					++blockNumberCounter;

					#ifdef MONITORING
						cout << "." << flush;
					#endif
					// Wir stoppen den Timer erst, wenn der letzte Teil des
					// XML-Dokumentes angekommen ist
					stopTimer(timerId);
					
					// letzten Teil noch anhängen
					msgString.cat(headerAndString);

					idaTrackData(("receiving XML document from TdfAccess complete"));
					EVENT(ownObjectId, iDAMinRepClass, 207, "");
					
					// ... und via Socket verschicken
					sendViaSocketAndClose(msgString);

					// requestId weiterschalten, was bedeutet, dass
					// keine weiteren Pakete (Blöcke) angenommen werden
					++requestId;
				}
				// -------------------------------------------------------------
				// Fall 3: weder erstes noch letztes Paket (aus Bereich 2..n-1)
				else
				{
					// Wenn die Blocknummer nicht stimmt (Reihenfolgefehler oder
					// verloren gegangener Block) dann melden wir eine Fehler
					if (header.blockNumber != blockNumberCounter)
					{
						String xmlString;
						formatXMLErrorResponse(xmlString,
											   String("messageOrderError"),
											   String("Message with wrong blockNumber received"));
						sendViaSocketAndClose(xmlString);
						++requestId;
						break;
					}
					++blockNumberCounter;
					#ifdef MONITORING
						cout << "." << flush;
					#endif
					msgString.cat(headerAndString);
				}
			}
			break;

        case Types::TDF_TRYINGTOREGISTER_MSG :
             idaTrackData(("TDF_TRYINGTOREGISTER_MSG message received!"));
# ifdef CLASSLIB_03_00
                        if (msg.getMsgRef() == requestId)
# else
                        if (msg->getMsgRef() == requestId)
# endif
             {
              stopTimer(timerId);
              if ( retryCounter < 1 ){
                 sendRequestToBackupTdfClient(requestString);
                 ++retryCounter;
              }else{
                 String xmlString;
                 formatXMLErrorResponse(xmlString,
                 String("tryingToRegister"),
                 String("System is trying to register at DB"));
                 sendViaSocketAndClose(xmlString);
                 ++requestId;
              }

             }
             break;

		// ---------------------------------------------------------------------
		case signalMsg:
			idaTrackExcept(("Signal message (SIGPIPE) received!"));

			#ifdef MONITORING
				cout << "signalMsg received" << endl;
			#endif
			break;

		// ---------------------------------------------------------------------
		default:
			// Ignore unexpected message type
			#ifdef MONITORING
				cout << "other Msg received" << endl;
			#endif
			
			break;

	}

# ifdef CLASSLIB_03_00
	return isOk;
# else
        // Speicher der Message freigeben !
        if (msg)
        {
                delete msg;
        }
        msg = 0;

        return isOk;
# endif 
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn wir ein Timer-Event erhalten bedeutet das immer, dass ein Fehler 
//	aufgetreten ist. Eine XML_Dokument vom TdfAccess muß immer vollständig
//	innerhalb der geforderten Zeit eingetroffen sein, sonst wird es verworfen
//
Void WebProcess::timerBox(const RefId id)
{
    TRACE_FUNCTION("WebProcess::timerBox(...)");
	
	
	// -------------------------------------------------------------------------
	// Timeout für den Request
	if (id == timerId)
	{
		idaTrackExcept(("Query timed out"));
		EVENT(ownObjectId, iDAMinRepClass, 102, "");
		
		// Request Nummer inkrementieren ---
		// das bedeutet, dass alle weiteren Messages des Dokumentes
		// verworfen werden
		++requestId;
		
		// Messageteile, die evtl. schon eingegangen sind verwerfen
		msgString = "";

		#ifdef MONITORING
			cout << "Timeout";
		#endif
		
		// Fehler Dokument erzeugen
		formatXMLErrorResponse(msgString, String("timeout"), String("timeout message received"));

		// Fehler Dokument senden
		sendViaSocketAndClose(msgString);

	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wie der Name der Methode schon sagt: Erst werden die Daten über den noch offenen Socket 
//	gesende, dann wird der Socket geschlossen. Die "Session" ist damit beendet.
//
ReturnStatus WebProcess::sendViaSocketAndClose(const String& sendString)
{
    TRACE_FUNCTION("WebProcess::sendViaSocketAndClose(...)");
	
	
	if (connectionFlag == false) 
	{
		#ifdef MONITORING
			cout << "]" << endl;
		#endif
		return isNotOk;
	}

	idaTrackData(("Sending Response via Socket:\n%s", sendString.cString()));

	// -------------------------------------------------------------------------
	// Senden der Daten via Socket
	ssize_t bytesSend  = 0;
	size_t  sendLength = sendString.cStringLen();
	char*   str        = (char*)sendString.cString();
	#ifdef MONITORING
		cout << "{" << sendLength << "}" << flush;
	#endif
	
	streamSocket.setBlocking(true);

	// PcpHandle pcph = streamSocket.getSockFd();
	

	
	idaTrackData(("***** writing data within loop in blocking mode"));
	while (sendLength > 0)
	{
		idaTrackData(("***** loop start"));
		if (streamSocket.writeStreamFixLen(str, sendLength, bytesSend) == isNotOk)
		{
			idaTrackExcept(("***** writeStreamFixLen failed ! (may be ok)"));
			StreamSocketBase::Errors err = streamSocket.getError();
			#ifdef MONITORING
				cout << "E(" << err << ")" << flush;
			#endif
			if ( ( err == StreamSocketBase::tryAgain ) || ( err == StreamSocketBase::interruptOccured ) )
			{
				// Senden hat nicht direkt geklappt, kann aber beim nächsten
				// Mal funktionieren
				idaTrackExcept(("***** err: tryAgain or interruptOccured"));

#ifdef CLASSLIB_03_00
            PcpHandle handle;
            waitForEvent ( handle, 20);
#else     
				// Eine kleine Pause
				SysPoll pollObj;
				PollEvent event;
				pollObj.waitForEvent(event, 20);
#endif
				continue;
			}

			idaTrackExcept(("Error in method writeStreamFixLen: %d", err));
   
			// Nicht vergessen, dass der Socket irgendwann geschlossen werden muss.
			break;
		}

		idaTrackData(("sendLength=%d, bytesSend=%d", sendLength, bytesSend));
		idaTrackData(("error=%d", (int)(streamSocket.getError())));

		sendLength -= bytesSend;
		str        += bytesSend;
		#ifdef MONITORING
			cout << "{" << sendLength << "}" << flush;
		#endif
	}


	{
	  PcpTime time;

	  idaTrackData(("*** Response writen to Socket at  %d:%d:%d,%d\n", time.getHour(), time.getMinute(), time.getSec(), time.getMilliSec()));
	}

	// -------------------------------------------------------------------------
	// Es sollen keine Events den StreamSocket betreffend gesendet werden
# ifdef CLASSLIB_03_00
	eventDispatcher->checkOut(streamSocket.getSockFd());
# else
	 eventDispatcher->checkOutFd(streamSocket.getSockFd());
#endif



	// -------------------------------------------------------------------------
	// Socket schließen, die etwas kompliziertere Variante für "graceful shutdown":
	struct linger myLingerStruct;
	// preparing linger structure for setsockopt
	// activate lingering
	myLingerStruct.l_onoff = 1;
	// set linger time to 5 seconds
	myLingerStruct.l_linger = 5;

	if (setsockopt(streamSocket.getSockFd(), SOL_SOCKET, SO_LINGER,
		(void*)&myLingerStruct, sizeof(myLingerStruct)) !=0)
	{
		idaTrackExcept(("Could not set SO_LINGER option, error %d (%s)", errno, strerror(errno)));
	}
	streamSocket.setBlocking(false);

	idaTrackData(("Zeitmessung"));
	{
	  PcpTime t0;

	  // Eine kleine Pause
#ifdef CLASSLIB_03_00
            PcpHandle handle;
            waitForEvent ( handle, 100);
#else     
	  SysPoll pollObj;
	  PollEvent event;
	  pollObj.waitForEvent(event, 100);
#endif
     
	  RelTime delta = PcpTime() - t0;
	  idaTrackData(("***** sleeping = %d", delta.inMilliSeconds()));
	}


//	shutdown(streamSocket.getSockFd(), 0);

	if (streamSocket.closeSocket() == isNotOk)
	{
		// Fehler beim Schließen des Sockets
		idaTrackExcept(("WebProcess::sendViaSocketAndClose failed (closeSocket)"));
		#ifdef MONITORING
			cout << "Server could not close Socket" << endl;
		#endif
		return isNotOk;
	}
	idaTrackTrace(("***** socket closed !"));
	connectionFlag = false;


	#ifdef MONITORING
		cout << "|>>S] (";
	#endif
	#ifdef MONITORING
		{
			++gesamtCounter;
			RelTime delta = Time() - timeStamp1;
			ULong dauer = delta.inMilliSeconds();
			cout << dauer << "ms)" << endl;
			gesamtDauer += dauer;
			if (dauer < gesamtDauerMin) gesamtDauerMin = dauer;
			if (dauer > gesamtDauerMax) gesamtDauerMax = dauer;
			if (gesamtCounter >= 100)
			{
				cout << "\tGesamt   min/avg/max [ms] : " 
					 << gesamtDauerMin << "/"
					 << gesamtDauer / gesamtCounter << "/"
					 << gesamtDauerMax
					 << endl;
				gesamtDauerMin = 1000000;
				gesamtDauerMax = 0;
				gesamtDauer = 0;
				gesamtCounter = 0;
			}
		}
	#endif
	#ifdef MONITORING
		{
			++responseCounter;
			RelTime delta = Time() - timeStamp2;
			ULong dauer = delta.inMilliSeconds();
			responseDauer += dauer;
			if (dauer < responseDauerMin) responseDauerMin = dauer;
			if (dauer > responseDauerMax) responseDauerMax = dauer;
			if (responseCounter >= 100)
			{
				cout << "\tResponse min/avg/max [ms] : " 
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


	return isOk;
}


/////////////////////////////////////////////////////////////////////////
//
ReturnStatus WebProcess::createXmlNode(String& xmlString, const String& tag, const char* val)
{
//	TRACE_FUNCTION("WebProcess::createXmlNode(...)");


	xmlString.cat(tagStart);
	xmlString.cat(tag);
	xmlString.cat(tagEnd);

	// an dieser Stelle muessen wir einige Zeichen maskieren
	Int saveCounter = 255;
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
		if (!--saveCounter)	break;
	}

	xmlString.cat(tagcStart);
	xmlString.cat(tag);
	xmlString.cat(tagEndNL);

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Methode erzeugt aus einem als Text übergebenen Fehler ein gültiges 
//	XML Dokument
//
ReturnStatus WebProcess::formatXMLErrorResponse(String& xmlString,
											    const String& errorCode,
												const String& errorText)
{
    TRACE_FUNCTION("WebProcess::formatXMLErrorResponse(...)");
	
	
	xmlString.cat(tagHEADER);

	xmlString.cat(tagoOsaResponse);
	xmlString.cat(" type=\"error\">\n");
	
	xmlString.cat(tagoSearchResult);

	createXmlNode(xmlString, tagErrorSource, "webProcess");
	createXmlNode(xmlString, tagErrorCode, errorCode.cString());
	createXmlNode(xmlString, tagErrorText, errorText.cString());

	xmlString.cat(tagcSearchResult);
	xmlString.cat(tagcOsaResponse);

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Methode holt den Wert eines XML-Elementes aus einer XML-Struktur (String). Voraussetzung
//	ist, dass der Tag-Name eindeutig ist, da hier kein richtiger Parser verwendet wird.
//
ReturnStatus WebProcess::getValueOfXMLElement(const char* queryStr,
											  const char* tagName,
											  String& value)
{
    TRACE_FUNCTION("WebProcess::getValueOfXMLElement(...)");
	
	
	// Wir suchen den Eintrag mit Form "<tagName>nnn</tagName>
	String startTag;
	startTag.cat("<");
	startTag.cat(tagName);
	startTag.cat(">");

	String endTag;
	endTag.cat("</");
	endTag.cat(tagName);
	endTag.cat(">");

	UInt startPos   = 0;
	UInt endPos     = 0;

	String tmpStr(queryStr);

	// Positionen der beiden Tags
	if (!tmpStr.isSubStrInStr(startTag, startPos))     return isNotOk;
	if (!tmpStr.isSubStrInStr(endTag, endPos))         return isNotOk;

	// Auf die Startposition addieren wir noch die Länge des StartTags
	startPos += startTag.cStringLen();

	// Ergebnisstring holen
	String valueStr;
	if (tmpStr.subString(startPos, endPos - startPos, valueStr) == isNotOk)
		return isNotOk;

	// Ergebnis zuweisen
	value = valueStr;

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
ReturnStatus WebProcess::getValueOfXMLElement(const char* queryStr,
											  const char* tagName,
											  long& value)
{
//	TRACE_FUNCTION("WebProcess::getValueOfXMLElement(...)");
	
	
	// Ergebnisstring holen
	String valueStr;
	if (getValueOfXMLElement(queryStr, tagName, valueStr) == isNotOk)
		return isNotOk;

	// String nach long wandeln
	DecString decString(valueStr);
	long val = 0;
	
	if (decString.getValue(val) == isNotOk)
		return isNotOk;

	// Ergebnis zuweisen
	value = val;

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Bei jedem Aufruf dieser Methode wird eine neue Nummer für den Request
//	ermittelt. die Nummern bewegen sich in einem Bereich von 1024 Nummern.
//	Zusätzlich wird ein WebProcess abhängiger Offset hinzuaddiert, damit
//	sich die Nummern im TdfProcess immernoch eindeutig sind.
//	Wenn es sich also um den WebProcess 1 handelt liegen die Nummern
//	im Bereich 0 ... 1023, für den WebProcess im Bereich 1023 ... 2047, etc.
//
Short WebProcess::getNextRequestId()
{
//	TRACE_FUNCTION("WebProcess::getValueOfXMLElement(...)");
	
	
	// Anzahl Nummern, die ein WebProcess verwenden darf
	const UShort slotSize = 1024;

	// Id inkrementieren
	++requestCounter;

	// modulo slotSize
	requestCounter %= slotSize;

	requestId = (requestCounter + (slotSize * objectIdOffset));

	return requestId;
}



// *** EOF ***


