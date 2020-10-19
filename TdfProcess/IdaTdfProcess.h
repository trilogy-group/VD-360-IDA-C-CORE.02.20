#ifndef IdaTdfProcess_h
#define IdaTdfProcess_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaTdfProcess.h 1.1
//
//   File:      IdaTdfProcess.h
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:15
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------




static const char * SCCS_Id_IdaTdfProcess_h = "@(#) IdaTdfProcess.h 1.1";


//--------------------- include files ------------------------------------
#include <pcpprocess.h>
#include <vector.h>
#include <syspar.h>



class TdfAccess;


class TdfProcess : public Process
{
	public:
		/** Konstruktor */
		TdfProcess(int argc, char** argv, char** envp);

		/** Destruktor */
		~TdfProcess();
	
		// -------------------------------------------------------------------------
		/** */
		ReturnStatus initialize(const String& parFileName);
	
		void run();

	private:

		enum TdfAccessStatus
		{
			statusOk    = 0,
			statusNotOk = 1
		};

		s_vector<TdfAccess*> tdfAccessList;
		Long             _procObjectId;

};

#endif  // IdaTdfProcess_h



// *** EOF ***

