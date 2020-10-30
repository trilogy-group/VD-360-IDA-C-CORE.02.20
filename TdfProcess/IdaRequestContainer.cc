
//CB>---------------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaRequestContainer.cc 1.1
// 
//   File:      IdaRequestContainer.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:01:32
// 
//   DESCRIPTION:
//     
//     
//     
//   
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaRequestContainer_cc = "@(#) IdaRequestContainer.cc 1.1";


#include <IdaDecls.h>
#include <idatraceman.h>
#include <objectid.h>
#include <IdaRequestContainer.h>


#ifdef ALLOW_STDOUT
	#define MONITORING
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Konstruktor mit Parameter�bergabe
//
RequestContainer::RequestContainer()
	: status(notReady),
	  requestId(0),
	  tdfArgument(0),
	  channel(0),
	  senderOid(0),
	  timeout(0xFFFFFFFF),
	  timerId(0)
//	  timeStamp(0)
{
	idaTrackData(("RequestContainer::RequestContainer() Constructor called"));
}



RequestContainer::RequestContainer(UShort	reqId,
								   ObjectId	sOid)
	: mode(request),
	  status(notReady),
	  requestId(reqId),
	  tdfArgument(0),
	  channel(0),
	  senderOid(sOid),
	  timeout(0xFFFFFFFF),
	  timerId(0)
{
//	birth.actualizeGmtTime();		
	#ifdef MONITORING
//		cout << birth.formatTime() << std::endl;
	#endif
	idaTrackData(("RequestContainer::RequestContainer() Constructor called"));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Destruktor
//
RequestContainer::~RequestContainer()
{
	idaTrackData(("RequestContainer::~RequestContainer() Destructor called"));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Pr�ft das Alter des Requestes und gibt "true" zur�ck, wenn er veraltet ist,
//	sonst "false"
//
Bool RequestContainer::expired() const
{
	PcpTime actualTime;

	return ((  birth.getMinute() * 60000
			 + birth.getSec() * 1000
			 + birth.getMilliSec() 
			 + timeout )
				< 
			  (actualTime.getMinute() * 60000
			+ actualTime.getSec() * 1000
			+ actualTime.getMilliSec()));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Diese Methode pr�ft einige Parameter und ver�ndert gegebenenfalls den Status
//
Void RequestContainer::checkStatus()
{
	if ( (status	  == notReady)			&&
		 (tdfArgument != 0)					&&
		 (senderOid.getIdentifier() != 0)	&&
		 (timeout	  != 0xFFFFFFFF)
	   )
	{
		// Der Request ist bereit gesendet zu werden
		status = readyToSend;
	}
}




// *** EOF ***


