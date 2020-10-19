
//CB>---------------------------------------------------------------------------
//
//   File, Component, Release:
//                  TdfProcess/IdaTdfProcessMain.cc 1.0 12-APR-2008 18:52:10 DMSYS
//
//   File:      TdfProcess/IdaTdfProcessMain.cc
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
//
//   DESCRIPTION:
//
//
//
//<CE---------------------------------------------------------------------------




static const char * SCCS_Id_IdaTdfProcessMain_cc = "@(#) TdfProcess/IdaTdfProcessMain.cc 1.0 12-APR-2008 18:52:10 DMSYS";


//--------------------- include files ------------------------------------
#include <pcpstring.h>
#include <IdaDecls.h>
#include <iostream.h>
#include <IdaTdfProcess.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream.h>
// #include <dl.h>


#ifdef ALLOW_STDOUT
	#define MONITORING
#endif



int main(int argc, char** argv, char** envp)
{
	cerr << "TdfProcess started ..." << endl;
	
	// Check parameter
   String par1( argv[1] );
	if ( (argc < 1 ) || ( (argc < 3) && (par1 == "115") ) )
	{
		cerr << "error: missing arguments" << endl;
		cerr << "Usage: " << argv[0] << " <ParFile> "
			 << "[ -tr <TraceLevel> -tf <TraceFile> ]" << endl;
		exit(1);
	}

   String parFileName;
   
	// Extract Parameter- and Declaration-Filename
   if ( par1 == "115" )
   {
     parFileName = String(argv[3]);
   }
   else
   {
     parFileName = String(argv[1]);
   }
   

	// Instantiate process object
	TdfProcess tdfProcess(argc, argv, envp);


	// Wenn die Initialisierung ok ist ...
	if (tdfProcess.initialize(parFileName) == isOk)
	{
		// ... dann können wir die Instanz starten
		tdfProcess.run();
	}

/*
#ifdef _HPUX_
	if (Data)
	{
		dlclose(Data);
	}
#endif
*/


	return 0;
}


// *** EOF ***
