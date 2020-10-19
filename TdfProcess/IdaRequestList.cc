
//CB>---------------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaRequestList.cc 1.1
// 
//   File:      IdaRequestList.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:05
// 
//   DESCRIPTION:
//     
//     
//   
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaRequestList_cc = "@(#) IdaRequestList.cc 1.1";




#include <map.h>
#include <pcpdefs.h>
#include <idatraceman.h>
#include <pcptime.h>
#include <IdaRequestList.h>


#ifdef ALLOW_STDOUT
	#define MONITORING
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
RequestList::RequestList()
{
	itemCounter = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
RequestList::~RequestList()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Einen Request in die List einfügen
//
Void RequestList::addRequest(RequestContainer& requestContainer)
{
	idaTrackTrace(("RequestList::addRequest"));


//	if (requestContainer)
//	{
		UShort requestId = requestContainer.getRequestId();
		// Objekt in die Liste eintragen
#ifdef OS_AIX_4
		s_pair<UShort, RequestContainer> listItem(requestId, requestContainer);
#else
		s_pair<const UShort, RequestContainer> listItem(requestId, requestContainer);
#endif
		insert(listItem);
		++itemCounter;
//	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Werte eines Requests aus der Liste holen.
//	Der Request wird über die "requestId" spezifiziert.
//
Bool RequestList::getRequest(const UShort&	requestId,		// 
							 TdfArgument*&	tdfArgument,	// Request im Tdf-Format
							 UShort&		channel,		// 
							 ObjectId&		senderOid,		// ObjektID des Senders
							 RefId&			timerId,
							 PcpTime&			birth)
{
	idaTrackTrace(("RequestList::getRequest"));


	// Iterieren bis die passende Referenz gefunden wurde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).first == requestId)
		{
			tdfArgument = (*cursor).second.getTdfArgument();
			channel     = (*cursor).second.getChannel();
			senderOid   = (*cursor).second.getSenderOid();
			timerId		= (*cursor).second.getTimerId();
			birth		= (*cursor).second.getBirth();
			return true;
		}
		cursor++;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Werte eines Requests aus der Liste holen.
//	Der Request wird über die "requestId" spezifiziert.
//
Bool RequestList::getRequest(const UShort&	requestId,
							 RequestContainer& requestContainer)
{
	idaTrackTrace(("RequestList::getRequest"));


	// Iterieren bis die passende Referenz gefunden wurde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).first == requestId)
		{
			requestContainer = (*cursor).second;
			return true;
		}
		cursor++;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Den ersten Request in die List holen
//
Bool RequestList::getFirstRequest(UShort&		requestId,		// 
								  TdfArgument*&	tdfArgument,	// Request im Tdf-Format
								  UShort&		channel,		// 
								  ObjectId&		senderOid,		// ObjektID des Senders
								  RefId&		timerId, 
								  PcpTime&			birth)
{
	idaTrackTrace(("RequestList::getFirstRequest"));


	// Wir nehmen das erste Listenelement, dass wir finden
	RequestList::iterator cursor = begin();
	if (cursor != end())
	{
		requestId	= (*cursor).second.getRequestId();
		tdfArgument = (*cursor).second.getTdfArgument();
		channel     = (*cursor).second.getChannel();
		senderOid   = (*cursor).second.getSenderOid();
		timerId		= (*cursor).second.getTimerId();
		birth		= (*cursor).second.getBirth();
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Liefert den ältesten Request
//
Bool RequestList::getOldestRequest(UShort&			requestId,		// 
								   TdfArgument*&	tdfArgument,	// Request im Tdf-Format
								   UShort&			channel,		// 
								   ObjectId&		senderOid,		// ObjektID des Senders
								   RefId&			timerId,
								   PcpTime&			birth)
{
	idaTrackTrace(("RequestList::getOldestRequest"));

	ULong secs  = PcpTime().getSecs();
	Bool  found = false;

	// Wir nehmen das erste Listenelement, dass wir finde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).second.getBirth().getSecs() <= secs)
		{
			secs		= (*cursor).second.getBirth().getSecs();
			requestId	= (*cursor).second.getRequestId();
			found		= true;
		}
		cursor++;
	}

	if (found)
	{
		return getRequest(requestId, tdfArgument, channel, senderOid, timerId, birth);
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Liefert den ältesten Request, der noch nicht gesendet wurde 
//
Bool RequestList::getOldestWaitingRequest(RequestContainer& requestContainer)
{
	idaTrackTrace(("RequestList::getOldestRequest"));

	ULong	secs		= PcpTime().getSecs();
	Bool	found		= false;
	UShort	requestId	= 0;

	// Wir nehmen das erste Listenelement, dass wir finde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ( ((*cursor).second.getBirth().getSecs() <= secs) &&
			 ((*cursor).second.getStatus() == RequestContainer::readyToSend ||
			  (*cursor).second.getStatus() == RequestContainer::authenticationRequired) )
		{
			secs		= (*cursor).second.getBirth().getSecs();
			requestId	= (*cursor).second.getRequestId();
			found		= true;
		}
		cursor++;
	}

	if (found)
	{
		return getRequest(requestId, requestContainer);
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Die Methode versucht einen Request anhand der TimerId zu finden und gibt
//	im positiven Fall die zugehörige RequestId zurück
//
Bool RequestList::findTimerId(const RefId timerId, UShort& requestId)
{
	idaTrackTrace(("RequestList::findTimerId"));


	// Iterieren bis die passende Referenz gefunden wurde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).second.getTimerId() == timerId)
		{
			requestId = (*cursor).second.getRequestId();
			return true;
		}
		cursor++;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Wenn ein Request mit der "searchRef" existiert liefert die Methode true,
//	sonst false
//
Bool RequestList::exists(UShort requestId)
{
	idaTrackTrace(("RequestList::exists"));


	// Iterieren bis die passende Referenz gefunden wurde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).first == requestId)
		{
			return true;
		}
		cursor++;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Ein Request-Element aus der Liste entfernen
//	Das Element wird durch die Referenz-Nummer identifiziert
//	
//	Ergebnis: Referenz auf einen Request-Container
//
Bool RequestList::removeRequest(UShort requestId)
{
	idaTrackTrace(("RequestList::removeRequest"));


	// Iterieren bis die passende Referenz gefunden wurde
	RequestList::iterator cursor = begin();
	while (cursor != end())
	{
		if ((*cursor).first == requestId)
		{
			// Gefunden !
			// Eintrag aus der Liste löschen
			erase(cursor);
			--itemCounter;
			
			return true;
		}
		cursor++;
	}

	return false;
}
// -----------------------------------------------------------------------------
Bool RequestList::removeRequest(RequestContainer& requestContainer)
{
	return removeRequest(requestContainer.getRequestId());
}





// *** EOF ***

