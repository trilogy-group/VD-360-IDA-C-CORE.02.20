#ifndef IdaTraceMgr_h
#define IdaTraceMgr_h

//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  Base/IdaTraceMgr.h 1.0 12-APR-2008 18:52:10 DMSYS
// 
//   File:      Base/IdaTraceMgr.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
// 
//   DESCRIPTION:
//     Handles command line trace control settings.
//     
//     
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTraceMgr_h = "@(#) Base/IdaTraceMgr.h 1.0 12-APR-2008 18:52:10 DMSYS";


class String;




class TraceMgr

 {

  public :


    static void getTraceSettings ( int argc, char** argv,
				   String& traceFileName,
				   int& traceLevel );
	// Read trace settings from command line arguments.
	// If -tf is not given, traceFileName contains
	// empty string on return.
	// If -tr is not given, traceLevel contains 0 on return.


  private :

    TraceMgr () {}
	// Class is uninstantiable

 };

#endif	// IdaTraceMgr_h

// *** EOF ***


