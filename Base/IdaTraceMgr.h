#ifndef IdaTraceMgr_h
#define IdaTraceMgr_h

//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaTraceMgr.h 1.1
// 
//   File:      IdaTraceMgr.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:55
// 
//   DESCRIPTION:
//     Handles command line trace control settings.
//     
//     
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTraceMgr_h = "@(#) IdaTraceMgr.h 1.1";


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


