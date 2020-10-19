#ifndef IdaTdfChannelMgr_h
#define IdaTdfChannelMgr_h

//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  TdfProcess/IdaTdfChannelMgr.h 1.0 12-APR-2008 18:52:11 DMSYS
// 
//   File:      TdfProcess/IdaTdfChannelMgr.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:11
// 
//   DESCRIPTION:
//     Administration of TDF channels within IDA TDF client.
//     
//     
//     
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTdfChannelMgr_h = "@(#) TdfProcess/IdaTdfChannelMgr.h 1.0 12-APR-2008 18:52:11 DMSYS";



#include <pcpdefs.h>
#include <vector.h>
#include <IdaTdfChannel.h>


typedef s_vector<TdfChannel> VectorOfChannels;


class TdfChannelMgr : protected VectorOfChannels
{

	public :
		/** Default constructor */
		TdfChannelMgr();
	
	
		/** Destructor */
		~TdfChannelMgr();
	
	
		/** Get number of channels */
		UShort getNoOfChannels() const          { return size(); }
	
	
		/** Set number of available channels */
		ReturnStatus setNoOfChannels(UShort nChannels);
	
	
		/** Get number of free channels */
		UShort getNoOfFreeChannels() const      { return noOfFreeChannels; }
	
	
		/** reserve a free channel
		    If no channel is available, 0 will be returned
			*/
		UShort reserveChannel();
	
	
		/** Release channel with given number. */
		ReturnStatus releaseChannel(UShort channelNo);
	
	
		// Get search reference for UDA searchArg
//		UShort getSearchRefForChannel(UShort channel) const
//		{
//			return channelList[channel].getCurrentSearchRef();
//		}
	
		/** Dump channel info */
		void dump(ostream& out);

		Void showStatus();
	
	private :
	
		UShort			   		noOfFreeChannels;
		UShort					totalNoOfChannels;
		UShort					lastReservedChannel;
};

#endif	// IdaTdfChannelMgr_h


// *** EOF ***


