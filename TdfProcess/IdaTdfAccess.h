#ifndef IdaTdfAccess_h
#define IdaTdfAccess_h

//CB>-------------------------------------------------------------------
//
//   File, Compoponent, Release:
//                  TdfProcess/IdaTdfAccess.h 1.0 12-APR-2008 18:52:13 DMSYS
//
//   File:      TdfProcess/IdaTdfAccess.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:13
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------





static const char * SCCS_Id_IdaTdfAccess_h = " (#) TdfProcess/IdaTdfAccess.h 1.0 12-APR-2008 18:52:13 DMSYS";


#include <pcpdefs.h>
#include <pcpstring.h>
#include <osaticket.h>
#include <toolbox.h>
#include <reporterclient.h>
#include <tdfclient.h>
#include <IdaTdfChannelMgr.h>
#include <IdaRequestContainer.h>
#include <IdaRequestList.h>
#include <tdferrorarg.h>
#include <tdfaddress.h>
#include <tdforiginid.h>
#include <tdsrequest.h>
#include <pcptimerinterface.h>
#include <modifyrequest.h>
#include <modifyresponse.h>

XERCES_CPP_NAMESPACE_USE


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class TdfAccess : public TdfClient, public PcpTimerInterface
{
		// -------------------------------------------------------------------------
		// types
		// -------------------------------------------------------------------------
	private:
	
		// -------------------------------------------------------------------------
		// Status
		// -------------------------------------------------------------------------
		enum TdfAccessStatus
		{
			start					= 0,	// Anfangszustand
			initializationFailed	= 1,	// Initialisierung schlug fehl
			initialized				= 2,	// Initialisierung erfolgreich
			tryingToRegister		= 3,	// war noch nie registriert aber Request abgeschickt
			unAvailable				= 4,	// nicht verfügbar
			registered				= 5		// registriert fertig Anfragen zu bearbeiten
		};
	
		Void					changeStatus(enum TdfAccessStatus newStatus);
		TdfAccessStatus			getStatus() const { return status; }
	
	
		// -------------------------------------------------------------------------
		// Konstruktor / Destruktor
		// -------------------------------------------------------------------------
	public:
	
		TdfAccess(const ObjectId&           myId,			// die eigene ObjcetId
				  Long              dbId,			// Database ID
				  const ObjectId&   serverService,	// für TdfAccess
				  const String&     serviceName,	// 
				  const String&     applName,		// 
				  UShort            nChannels,		// Anzahl anzufordernder Channels
				  const String&     osaTicket,		// 
				  ULong             regTimer,		// Timeout für Registrierung
				  ULong             searchTimeout,	// Timeout für Suchanfragen
				  long             maxReg,		// Max. Anzahl Registrierungsversuche
				  Bool              regTest,		// Regressions-Test Ja/Nein ?
				  SesConfig			sesConfigPars);	// Parameter für SES Anbindung	!!! evtl. lieber Referenz nehmen
	
		~TdfAccess();
	
	
		ReturnStatus    initLocalTdfAddress();
	
		/** Resolve client "browser" IP address */
		ULong           resolveClientIp(String clientIp);
	
	
	
	private:
		TdfAccessStatus status;
	
		String                  osaTicketFileName;
		ReturnStatus            prepareOsaTicket();
		ReturnStatus            initXMLPlatform();
		ReturnStatus		terminateXMLPlatform();
		Bool                    regressionTest;
		XercesDOMParser * 	parser;	
	
		// -------------------------------------------------------------------------
		// Registrierung bei der Datenbank (NDIS)
		// -------------------------------------------------------------------------
	private:
		/** length of time in milli seconds after which registration with NDIS DAP is repeated */
		UShort              registerTimer;
		Bool				registerTimerFlag;
		/** max. number of registration attempts */
		Short              maxRegistrationAttempts;
		/** Timer ReferenzNummer */
		RefId               registerTimerId;
		UShort              registrationRetryCounter;
	
	
		ReturnStatus        startRegistration();
	
		ReturnStatus        sendRegisterRequest();
		ReturnStatus        sendDeRegisterRequest();
	
		/** Handle registerRes from TDF Server */
		Void                registerConfirmation(const TdfRegisterRes & arg);
	
		/** Handle deRegisterReport from TDF Server */
		Void                deRegisterReportIndication(const TdfDeRegisterReport & arg);
	
		/**	Startet einen Timer einmalig für die Registrierung */
		ReturnStatus        startRegisterTimer();
	
		/** Canceled einen, für die Registrierung laufenden, Timer */
		ReturnStatus        cancelRegisterTimer();
	
	
	
	
	
	
	public:
	
		// public methodes -----------------------------------------------------
	
	
		// Handle statusReport from TDF Server
		Void            statusReportIndication(const TdfStatusReport & arg);
	
		// Handle searchRes from TDF Server
		Void            searchConfirmation(const TdfResponse & arg);
	
		// Message handler
		Void            applicationMessageBox(Message & message);
		Void			handleRequest(Message& message);
	
		Void            timerBox(const RefId);
	
		// Receive shutdown message
		Void            shutdown(ObjectId & myId);
	
	
	private:
	
		// Determine next free channel number
		UShort          getNextChannelNr();
	
		// write application data like values of
		// class member variables into a string
		String          dumpReadable();
	
		// write request data into a string
		String          dumpRequests();
	
		// private methodes ----------------------------------------------------
	
		Void            assembleTdfOriginId(TdfOriginId* orId, const char* clientAddress);
	
		ReturnStatus    parseXmlRequestToDom(String& xmlString,
                                             XercesDOMParser* parser, 
											 DOMDocument*& doc,
											 String& errorText);
		ReturnStatus    getParameterFromDom(DOMDocument* dom,
											String& clientIpAddressString,
											String& linkContextString);
		ReturnStatus    getTimeoutFromDom(DOMDocument* dom,
											ULong& timeout);
		ReturnStatus	getAuthenticationParameter(DOMDocument*	dom,
												   String&			usr,
												   String&			pwd,
												   String&			npw,
												   String&			errorText);
		ReturnStatus	getRootElementName(DOMDocument*	dom,
										   String&			rootElementName);
	
	//	ReturnStatus    createTdfArgument();
	//		ReturnStatus	extractChannelFromXmlString(UShort& channel, String& xmlString);
	
	
		// Konvertierung TDS > XML direkt
		ReturnStatus    convTdsResponseToXml(TdsResponse& tdsResponse,
											 String& xmlString,
											 String& typeString);
	
		ReturnStatus    convTdfErrorArgToXml(TdfErrorArg& tdfErrorArg,
											 String& xmlString,
											 String& typeString);
	
		ReturnStatus    createXmlNode(String& xmlString,
									  const String& tag,
									  const char* val);
		ReturnStatus    createXmlNode(String& xmlString,
									  const String& tag,
									  const Int val);
		ReturnStatus    createXmlNode(String& xmlString,
									  const String& tag,
									  const ULong val);
		ReturnStatus    convIndentRecordToXml(
											 String& xmlString,
											 IndentRecord& indentRecord);
	
		ReturnStatus    sendBigString(RefId serverOid,
									  Short requestId,
									  RefId myOid,
									  const Short msgType,
									  const String& string);
	
		ReturnStatus    formatXMLErrorResponse(String& xmlString, const String& errorCode, const String& errorText);
	
		Void            dataToHexString(String byteSequence, String& result);
	
		UShort          getNextSearchId()
		{
			return ++searchId;
		}
		UShort          getActualSearchId() const
		{
			return searchId;
		}
		UShort          getNextReferenceId()
		{
			return ++referenceId;
		}
		UShort          getActualReferenceId() const
		{
			return referenceId;
		}
		ULong           getNextOpId()
		{
			return curOperationId++;
		}
	
	
	
		ReturnStatus    startSendRetryTimer();
		ReturnStatus	cancelSendRetryTimer();
	
	
	
		/** Zur Vereinfachung der Reporter-Ausgaben. Der Pointer auf den Reporter wird
			nur beim ersten Aufruf tatsächlich geholt */
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
	
	
	
		Data&           hexToData(const char * hex);
		UChar           ucharOfString(const char* str);
		ULong           ulongOfString(const char* str);
		UShort          ushortOfString(const char* str);
	
	
	
	
		// ---------------------------------------------------------------------
		// methodes to create a TdsRequest-object from a DOM-document
		ReturnStatus    createTdsRequestFromDom(TdsRequest& tdsRequest, DOMDocument* dom, String& errorText);
		ReturnStatus    createSearchSpecification(TdsRequest& tdsRequest, DOMNode* node, String& errorText);
		ReturnStatus    createSearchVariation(TdsRequest& tdsRequest, DOMNode* node, String& errorText);
		ReturnStatus    createRequestedResponse(TdsRequest& tdsRequest, DOMNode* node, String& errorText);
		ReturnStatus    createSearchAttributes(TdsRequest& tdsRequest, DOMNode* node, String& errorText);
		ReturnStatus	getValueOfDomNode(String& textValue, DOMNode* node);
	
		
		Void			processSendQueue();
	
		ReturnStatus	createTdfArgument(TdfArgument*	tdfArgument,
										  TdsRequest&	tdsRequest,
										  DOMDocument* doc,
										  Short         requestId,
										  String&       errorText);
	
		ReturnStatus	createTdfArgument(TdfArgument*	tdfArgument,
										  ModifyRequest&	modifyRequest,
										  DOMDocument* doc,
										  Short         requestId,
										  String&       errorText);
	
	
		Void            handleErrorArg(TdfErrorArg tdfErrorArg);
	
		Void			refuseRequest(RequestContainer& requestContainer,
									  String errorCode,
									  String errorText);
		Void			sendAuthenticationAcknowledge(RequestContainer& requestContainer);

		virtual Void modifyConfirmation( const TdfResponse & arg );
		
		ReturnStatus createModifyRequestFromDom(ModifyRequest& modifyRequest,
                                                 DOMDocument* dom,
                                                 String& errorText);	

	 	ReturnStatus createModifySpecification(ModifyRequest& modifyRequest, DOMNode* node, String& errorText);

		ReturnStatus createModifyAttributes(ModifyRequest& modifyRequest, DOMNode* node, String& errorText);

		ReturnStatus createModifyFlags(ModifyRequest& modifyRequest, DOMNode* node, String& errorText);

		Void convModifyResponseToXml(ModifyResponse & modifyResponse, String & xml, String & typeString);
	
	private:
	
		// private member ------------------------------------------------------
	
	
		String                  countryCode;		// country oder database identifier für Reporter
		String                  applicationName;	// Name für die Anmeldung bei NDIS
		UShort                  numberOfChannels;	// Anzahl Channels, die von NDIS angefordert werden
		OsaTicket               osaTicket;			// OSA Ticket für den DB Zugriff
		TdfAddress              localTdfAddress;	// 
		String                  serviceName;		// Parameter aus "ida.par"
		ULong                   searchTimeOut;		// timeout in Millisecunden für DB-Anfragen
		ObjectId                myObjectId;			//	
		Long                    databaseId;			// 
		UShort                  applicationId;
		UShort                  referenceId;
		UShort                  searchId;
		ULong                   curOperationId;
		Short                   searchTimerMsgNr;
		TdfChannelMgr           channelMgr;
		RequestList             workList;
		RequestList             waitingList;
		RequestList				requestPool;		// Liste mit den 
		ReporterClient*         pReporterClient;
		Data                    tempData;
		//DOMDocument            requestDOM;
	
		RefId					retryTimerId;		// TimerID des retry timers
		Bool					retryTimerFlag;		// Timer aktiv oder nicht
	
		SesConfig				sesConfig;			// Parameter für SES
        DOMImplementation*      impl;
  		// ------------------------------------------------------------
		// nur für Tests:
		String                  dummyResponse;
		char                    dbIdString[20];
		PcpTime                    timeStamp1;
		PcpTime                    timeStamp2;
	
		ULong					requestCounter;
		ULong					requestDauer;
		ULong					requestDauerMin;
		ULong					requestDauerMax;
		ULong					responseCounter;
		ULong					responseDauer;
		ULong					responseDauerMin;
		ULong					responseDauerMax;
		ULong					dbCounter;
		ULong					dbDauer;
		ULong					dbDauerMin;
		ULong					dbDauerMax;
	
};




#endif	// IdaTdfAccess_h


// *** EOF ***

