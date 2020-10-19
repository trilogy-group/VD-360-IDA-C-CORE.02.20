#ifndef IdaTypes_h
#define IdaTypes_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaTypes.h 1.1
//
//   File:      IdaTypes.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:55
//
//   DESCRIPTION:
//     Yellow Page wide used constants and types.
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTypes_h = "@(#) IdaTypes.h 1.1";


class Types
{
	private:
		
		// Class is uninstantiable
		Types () {};
	

	public:
		
		// Message types for object communication
		enum MessageType
		{
			XML_RESPONSE_MSG	= 7101,
			XML_REQUEST_MSG		= 7102,
			SES_LOGINREQ_MSG	= 7103,
			SES_LOGINRSP_MSG	= 7104,
			SES_LOGOUTREQ_MSG	= 7105,
			SES_LOGOUTRSP_MSG	= 7106 ,
			TDF_TRYINGTOREGISTER_MSG = 7107
		};

		
		// Miscellaneous defines
		enum MaximumLength
		{
			MAX_LEN_QUERY_STRING	= 16384,
			MAX_LEN_RESPONSE		= 1000000,
			MAX_CLASSLIB_MSG_SIZE	= 4096,
			MAX_NO_OF_TDFACCESS		= 20
		};


		// Header Struktur
		typedef struct _header
		{
			UShort  messageId;		// ID der Nachricht
			UShort  blockNumber;	// Nummer des aktuellen Blocks
			UShort  endFlag;		// Zeigt mit 1 das Ende an, sonst 0
			ULong   messageSize;	// Groesse der zu uebertragenen Nachricht
		} TransferHeader;



};

#endif	// IdaTypes_h

// *** EOF ***


