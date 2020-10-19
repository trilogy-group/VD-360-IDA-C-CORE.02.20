#ifndef IdaRequestList_h
#define IdaRequestList_h

//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaRequestList.h 1.1
// 
//   File:      IdaRequestList.h
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:07
// 
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaRequestList_h = "@(#) IdaRequestList.h 1.1";




#include <map.h>
#include <IdaRequestContainer.h>


typedef s_multimap < UShort, RequestContainer, s_less<UShort> > tRequestList;

class TdfArgument;

class RequestList : public tRequestList
{

	public :
	
		/** Default Constructor */
		RequestList();
	
	
		/** Destructor */
		~RequestList();
	

		/** Insert request into list */
		Void addRequest(RequestContainer& requestContainer);

	
		/** Insert request into list */
//		Void addRequest(const UShort	requestId,
//						TdfArgument*	tdfArgument,
//						UShort			channel,
//						const ObjectId&	senderOid,
//						RefId			timerId,
//						ULong			timeout);
	
		/** Request anhand der requestId suchen */
		Bool getRequest(const UShort&	requestId,
						TdfArgument*&	tdfArgument,
						UShort&			channel,
						ObjectId&	   	senderOid,
						RefId&			timerId,
						PcpTime&		birth);

		/** Request anhand der requestId suchen */
		Bool getRequest(const UShort&	requestId,
						RequestContainer& requestContainer);

		/** Gibt die Werte des ersten, gefundenen Requests zurück.
			Das Ergebnis ist dann true. Wenn die Liste leer ist,
			ist das Ergebnis false */
		Bool getFirstRequest(UShort&		requestId,
							 TdfArgument*&	tdfArgument,
							 UShort&		channel,
							 ObjectId&	   	senderOid,
							 RefId&			timerId,
							 PcpTime&		birth);

		/** Gibt die Werte des ältesten Requests zurück.
			Das Ergebnis ist dann true. Wenn die Liste leer ist,
			ist das Ergebnis false */
		Bool getOldestRequest(UShort&		requestId,
							  TdfArgument*&	tdfArgument,
							  UShort&		channel,
							  ObjectId&	   	senderOid,
							  RefId&		timerId,
							  PcpTime&			birth);

		/** Gibt die Werte des ältesten Requests zurück.
			Das Ergebnis ist dann true. Wenn die Liste leer ist,
			ist das Ergebnis false */
		Bool getOldestWaitingRequest(RequestContainer& requestContainer);
		
		/** Die Methode versucht einen Request anhand der
		    TimerId zu finden und gibt im positiven Fall
			die zugehörige RequestId zurück */
		Bool findTimerId(const RefId timerId,
						 UShort&	 requestId);

		/** prüft, ob der Request in der Liste ist */
		Bool exists(UShort requestId);
	
		/** Löscht einen Request aus der Liste */
		Bool removeRequest(UShort requestId);
		Bool removeRequest(RequestContainer& requestContainer);
	
	
		ULong getLevel() { return itemCounter; }

		/** Remove all request from internal list */
//		Void removeAllRequests();

//		Void showStatus();

	private:
		ULong itemCounter;

	
};

#endif	// IdaRequestList_h


// *** EOF ***


