#ifndef IdaTdfProcess_h
#define IdaTdfProcess_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  TdfProcess/IdaTdfProcess.h 1.0 12-APR-2008 18:52:10 DMSYS
//
//   File:      TdfProcess/IdaTdfProcess.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------




static const char * SCCS_Id_IdaTdfProcess_h = "@(#) TdfProcess/IdaTdfProcess.h 1.0 12-APR-2008 18:52:10 DMSYS";


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

