
//CB>---------------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaTdfChannelMgr.cc 1.1
// 
//   File:      IdaTdfChannelMgr.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:12
// 
//   DESCRIPTION:
//     TDF channel administration within IDA TDF client.
//     
//     
//   
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaTdfChannelMgr_cc = "@(#) IdaTdfChannelMgr.cc 1.1";




#include <IdaDecls.h>
#include <idatraceman.h>
#include <IdaTdfChannelMgr.h>
#include <pcptime.h>

#ifdef ALLOW_STDOUT
	#define MONITORING
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Defaultkonstruktor
//
TdfChannelMgr::TdfChannelMgr()
	: noOfFreeChannels(0),
	  totalNoOfChannels(0)
{
	lastReservedChannel = -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Destruktor
//
TdfChannelMgr::~TdfChannelMgr()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Setzt die Anzahl an verf�gbaren Channels und legt entsprechend Elemente 
//	in der Liste an
//
ReturnStatus TdfChannelMgr::setNoOfChannels(UShort nChannels)
{
#ifdef CLASSLIB_03_00
    TRACE_FUNCTION("TdfChannelMgr::setNoOfChannels(...)");
#endif	
	
	// Wenn schon Channels angelegt wurden und sich die Anzahl
	// nicht ge�ndert hat, dann machen wir nichts.
	if (nChannels == totalNoOfChannels) return isOk;

	// Anzahl Channels speichern
	noOfFreeChannels  = nChannels;
	totalNoOfChannels = nChannels;

	// Alle evtl. vorhandenen Elemente der Liste l�schen
	erase(begin(), end());
	
	// Channels in gew�nschter Anzahl neu anlegen
	// die Z�hlung der Channels beginnt bei "1" !
	// Ja, die n�chste Zeile ist tricki ! (leider nicht von mir lua)
	//	 Implizite Konvertierung von i in ein TdfChannel-Objekt
	for (UShort i = 1; i <= nChannels; i++) push_back(i);	

	return isOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
UShort TdfChannelMgr::reserveChannel()
{
#ifdef CLASSLIB_03_00
    TRACE_FUNCTION("TdfChannelMgr::reserveChannel(...)");
#endif	
	
	// Iteration �ber alle Channels
	for (int i = 0; i < totalNoOfChannels; i++)
	{
		++lastReservedChannel;
		lastReservedChannel %= totalNoOfChannels;
		// channel holen
		TdfChannel& channel = (*this)[lastReservedChannel];
		// Wenn er verf�gbar ist ...
		if (channel.getStatus() == TdfChannel::available)
		{
			channel.reserve();
			--noOfFreeChannels;
			return channel.getNumber();
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
ReturnStatus TdfChannelMgr::releaseChannel(UShort channelNo)
{
#ifdef CLASSLIB_03_00
    TRACE_FUNCTION("TdfChannelMgr::releaseChannel(...)");
#endif	
	
	if ((channelNo > 0) && (channelNo <= size()))
	{
		(*this)[channelNo-1].release();
		++noOfFreeChannels;
		return isOk;
	}
	return isNotOk;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//ReturnStatus TdfChannelMgr::reuseChannel(UShort channelNr)
//{
//	(void) channelList[channelNr].useAgain();
//	return isOk;
//}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void TdfChannelMgr::dump(std::ostream & out)
{
	iterator cursor = begin();

	while (cursor != end())
	{
		const TdfChannel& curChannel = *cursor;
		curChannel.dump(out);
		cursor++;
	}
}



Void TdfChannelMgr::showStatus()
{
	char channels[1024];
	memset(channels,0,1024);

	char ch;

	int counter = 0;

	iterator cursor = begin();
	while (cursor != end())
	{
		++counter;
		const TdfChannel& curChannel = *cursor;
		switch (curChannel.getStatus())
		{
			case TdfChannel::available :
				ch = '.';
				break;
			case TdfChannel::reserved :
				ch = 'X';
				break;
			case TdfChannel::disabled :
				ch = 'D';
				break;
		}
		channels[curChannel.getNumber()-1] = ch;
		cursor++;
	}
	std::cout << "\t\t\t\t[" << channels << "]" << counter << std::endl;
}






// *** EOF ***


