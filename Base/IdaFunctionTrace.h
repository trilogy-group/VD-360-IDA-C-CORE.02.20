#ifndef IdaFunctionTrace_h
#define IdaFunctionTrace_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaFunctionTrace.h 1.1
//
//   File:      IdaFunctionTrace.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:55
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------
static const char * SCCS_Id_IdaFunctionTrace_h = "@(#) IdaFunctionTrace.h 1.1";


#include <pcpstring.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	This is a helper-class for tracing the dynamic call of a method.
	It executes a trace command in the constructor and one in the destructor.
	So if you create an instance of this class at the beginning	of a method
	it produces a trace-output to report the entrance at the earliest moment.
	The report of the exit of the method will be generated automaticly.
	You don't have to care about it, when the methode has more than one exit
	points.
*/
class FunctionTrace
{
	public:
		FunctionTrace(const char* str) : name(str)
		{
			idaTrackTrace(("%s -IN-", name.cString()));
		}
	
		~FunctionTrace()
		{
			idaTrackTrace(("%s -OUT-", name.cString()));
		}											  
	
	private:
		String name;
};




#endif	// IdaFunctionTrace_h


// *** EOF ***

