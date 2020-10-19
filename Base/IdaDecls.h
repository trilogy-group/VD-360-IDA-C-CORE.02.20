#ifndef IdaDecls_h
#define IdaDecls_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaDecls.h 1.1
//
//   File:      IdaDecls.h
//   Revision:  1.1
//   Date:      17-NOV-2010 10:13:40
//
//   DESCRIPTION:
//     Declaration of variables used in IDA main parameter file.
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaDecls_h = "@(#) IdaDecls.h 1.1";


#include <strstream.h>
#include <pcpstring.h>
//#include <pcptrace.h>
#include <reporttypes.h>


// An dieser Stelle kann Ausgabe nach stdout via "cout" komplett
// also für WebProcess und TdfProcess abgeschaltet werden
// #define ALLOW_STDOUT


#define FUNCTION_TRACING

#ifdef FUNCTION_TRACING
#define TRACE_FUNCTION(x) FunctionTrace xXxXxX(x)
#else
#define TRACE_FUNCTION(x) ;
#endif

#define PROBLEM(a,b,c,d) reporter()->reportProblem((a),(b),(c),(d))
#define ALARM(a,b,c,d) reporter()->reportAlarm((a),(b),(c),(d))
#define EVENT(a,b,c,d) reporter()->reportEvent((a),(b),(c),(d))




// Bezeichner genau wie in "ida.par" inkl. Reihenfolge  !
static istrstream idaDecls
(
	"TdfProcessGroup "			// TdsProcess
		"DECLARE "
		"objectid; "
		"dbid; "
		"dbserver_objectid; "
		"dbserverctl_objectid; "
		"service_name; "
		"application_name; "
		"nrof_channels; "
		"osa_ticket; "
		"ENDDECL "
	
	"TimerAndMaxValueGroup "	// Timer, MaxValue, etc.
		"DECLARE "
		"register_timer; "
		"deregister_timer; "
		"search_intervall_timer; "
		"search_timeout; "
		"statusreport_timer; "
		"max_registration; "
		"max_search_request; "
		"ENDDECL "
	
	"WebProcessGroup "			// WebProcess
		"DECLARE "
		"base_objectid; "
		"base_socket_port; "
		"max_web_procs; "
		"ENDDECL "
	
);





struct SesConfig
{
	enum SesConfigMode
	{
		NONE	= 0,
		DEFAULT,
		NORMAL
	};

	long			loSesClientOID;
	SesConfigMode	enumMode;
	String			stDefaultUser;
	String			stDefaultPwd;
	int				iTicketId;

	//ctor
	SesConfig(SesConfigMode mode) : enumMode(mode) {}
};

static const char* szSesConfigModeNONE		= "NONE";
static const char* szSesConfigModeDEFAULT	= "DEFAULT";
static const char* szSesConfigModeNORMAL	= "NORMAL";




//------------------------------------------------------------------------------
// Dieser Bereich ist hier definiert, weil man es in der CLASSLIB bisher
// nicht für nötig erachtet hat IDA zu berücksichtigen !
// Die folgenden Zeilen sollten also irgendwann in 
//const ReportClass      iDAMinRepClass = 8301;
//const ReportClass      iDAMaxRepClass = 8400;



#endif	// IdaDecls_h

// *** EOF ***


