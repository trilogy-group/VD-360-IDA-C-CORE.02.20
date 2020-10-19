#ifndef IdaWebProcess_h
#define IdaWebProcess_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  WebProcess/IdaWebProcess.h 1.0 12-APR-2008 18:52:13 DMSYS
//
//   File:      WebProcess/IdaWebProcess.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:13
//
//   DESCRIPTION:
//     IDA.plus Web interface.
//     WebProcess is derived from Process.
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaWebProcess_h = "@(#) WebProcess/IdaWebProcess.h 1.0 12-APR-2008 18:52:13 DMSYS";

#include <pcpdefs.h>
#include <pcpstring.h>
#include <pcpprocess.h>
#include <pcptime.h>
#include <vector.h>
#include <toolbox.h>
#include <reporterclient.h>
#include <comman.h>
# ifndef CLASSLIB_03_00
  #include <comman.h> 
  #include <dispatcher.h>
  #include <eventdispatcher.h>
  #include <eventdispatchable.h>
# else
  #include <sys/commannorouter.h>
  #include <pcpdispatcher.h>
  #include <pcpeventdispatcher.h>
  #include <pcpeventhandler.h>
# endif
#include <dispatchable.h>
#include <message.h>
#include <pcptimerinterface.h>
#include <signal.h>
#include <serversockettcp.h>
#include <streamsocketdata.h>



class ComMan;
class EventDispatcher;

#ifdef CLASSLIB_03_00
class WebProcess : public Process, public Dispatchable, public PcpEventHandler, public PcpTimerInterface
#else
class WebProcess : public Process, public Dispatchable, public EventDispatchable, public PcpTimerInterface
#endif
{

public :
	
		/** Konstruktor */
  WebProcess(
    Int					argc,			// Commandline Parameter Anzahl
    Char**				argv,			// Commandline Parameter
    Char**				envp,			// Envirement Parameter
    RefId				objectId,		// eigene OID
    UShort				webProcessNo,	// Nummer des WebProzesses beginnend bei 0
    DatabaseList&		dbList,			// Liste mit allen verf�gbaren DBID's mit zug. OID
    int					socketNr,		// Port-Nummer des Sockets
    UShort				sTimeout		// Timeout fuer Suchanfragen
    );		
  // Shared Memory Objekt

		/** Destruktor */
  ~WebProcess();

  // init of the web process
  ReturnStatus init ();
  
  /** Startet den Proze� */
  Void run();
	
	
private :
  /** Behandelt die Filedescriptor-Events wenn ein Client versucht via
      Socket zu kommunizieren */
  ReturnStatus		handleEvent(PcpReturnEvent & event);
  Void				refuseAdditionalConnection();
  Void				handleServerSocketEvent();
  Void				handleStreamSocketEvent();

  Void				closeStreamSocket();

		/** Standard Callback-Funktion der CLASSLIB Kommunikation */
# ifdef CLASSLIB_03_00
		ReturnStatus		messageBox(Message&);
# else
		ReturnStatus            messageBox(Message*&);
# endif

		/** Callback-Funktion f�r Timer-Events */
		Void				timerBox(const RefId);

		/** Sendet einen String zum Client und schlie�t 
			anschlie�end den Socket */
		ReturnStatus		sendViaSocketAndClose(const String& sendString);

		ReturnStatus		sendRequestToTdfClient(const String& queryStr);

		ReturnStatus		sendRequestToBackupTdfClient(const String& queryStr);
		/** Erzeugt ein XML Dokument mit Fehlerbeschreibung und legt es in xmlString ab.
			errorText ist der einzubauende Fehlertext */
		ReturnStatus		formatXMLErrorResponse(String& xmlString,
												   const String& errorCode,
												   const String& errorText);

		/** Erzeugt ein volst�ndiges XML-Element mit dem Inhalt val */
		ReturnStatus		createXmlNode(String& xmlString, const String& tag, const char* val);

		/** Extrahiert aus einem XML-Dokument (queryStr) den Wert eines Elementes. Der zugeh�rende
			Tag-Name wird in tagName �bergeben. Der Tagname darf nur einmal im Dokument auftauchen,
			sonst wird das erste Vorkommen ausgewertet. Der Wert wird als long zur�ck gegeben */
		ReturnStatus		getValueOfXMLElement(const char* queryStr, const char* tagName, long& value);

		/** Extrahiert aus einem XML-Dokument (queryStr) den Wert eines Elementes. Der zugeh�rende
			Tag-Name wird in tagName �bergeben. Der Tagname darf nur einmal im Dokument auftauchen,
			sonst wird das erste Vorkommen ausgewertet. Der Wert wird als String zur�ck gegeben */
		ReturnStatus		getValueOfXMLElement(const char* queryStr, const char* tagName, String& value);

		/** Die Methode ermittelt die n�chste Zahl, die als RequestID f�r den n�chsten Request
			benutzt werden kann. Sie ber�cksichtigt dabei die unterschiedlichen ID Bereiche der
			verschiedenen WebProzesse */
		Short				getNextRequestId();
	

		/** Zur Vereinfachung der Reporter-Ausgaben. Der Pointer auf den Reporter wird
			nur beim ersten Aufruf tats�chlich geholt */
		ReporterClient* pReporterClient;
		ReporterClient* reporter()
		{
			if (pReporterClient)
			{
				return pReporterClient;
			}
			else
			{
				return(pReporterClient = Process::getToolBox()->getReporter());
			}
		}


		ReporterClient*		repClient;			// Zugriffs-Objekt f�r den Reporter-Proze�
		SysPoll* 		sysPoll;			// Zugriff-Objekt auf den System Poll	

# ifdef CLASSLIB_03_00
		PcpEventDispatcher*	eventDispatcher;	// dito. f�r den Dispatcher
		 PcpDispatcher*          dispatcher;
		ComManNoRouter*         comManNoRouter;                 // Zugriff-Objekt auf den Kommunikations-Manager
# else
		EventDispatcher*     eventDispatcher;		// dito. f�r den Dispatcher
		 ComMan*                         comMan;                 // Zugriff-Objekt auf den Kommunikations-Manager
#endif
		Long				ownObjectId;		// object ID for classlib communication
		Short				requestId;			// enth�lt die aktuelle Request ID
		Short				requestCounter;		// Hilfsvariable zur Request ID Berechnung
		UShort				objectIdOffset;		// Offset, der zur Basis ID addiert wird, um 
												// entg�ltige eigene ObjectId zu erhalten
		DatabaseList&		databaseList;		// Liste mit allen TdfAccess-ObjectId's und 
												// zugeh�renden Database ID's
		const Char*			clientIpAddress;	// IP Adresse des Clients
		String				servername;			// valid servername for referer address
		UShort				searchTimeout;		// Timeout f�r Suchanfragen
		ServerSocketTcp		serverSocket;		// Server Socket
		StreamSocketData	streamSocket;		// Kommunikations Socket
		int					socketPortNr;		// Portnummer des Server Sockets
		RefId				timerId;			// Variable zum speichern des Return-Wertes von 'startTimer'
		String				msgString;			// Variable f�r den Request-String
		Bool				connectionFlag;		// zeigt an, ob bereits eine Socket-Conection akzeptiert wurde
		UShort				blockNumberCounter;	// Z�hler f�r die Fehlererkennung bei Reihenfolge-Pr�fung
		int				retryCounter; // Zaehler fuer die Anzahl der Versuche ueber ein TdfAccess Objekt eine DB-Verbindung zu erreichen (Backup)
		
		// Zum Daten einlesen brauchen wir pro Socket
		char				buffer[Types::MAX_LEN_QUERY_STRING];
		Bool				requestCompleted;
		Bool				responseSend;
		String				requestString;


		// Nur f�r Testzwecke
		PcpTime                timeStamp1;
		PcpTime                timeStamp2;

		ULong	   			reqCounter;
		ULong				requestDauer;
		ULong				requestDauerMin;
		ULong				requestDauerMax;
		ULong				responseCounter;
		ULong				responseDauer;
		ULong				responseDauerMin;
		ULong				responseDauerMax;
		ULong				gesamtCounter;
		Ulong				gesamtDauer;
		Ulong				gesamtDauerMin;
		Ulong				gesamtDauerMax;
};

#endif	// IdaWebProcess_h


// *** EOF ***


