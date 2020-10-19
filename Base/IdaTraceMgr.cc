//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaTraceMgr.cc 1.1
//
//   File:      IdaTraceMgr.cc
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:56
// 
//   DESCRIPTION:
//     Handles command line trace control settings.
//     
//     
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_IdaTraceMgr_cc = "@(#) IdaTraceMgr.cc 1.1";


#include <IdaTraceMgr.h>
#include <pcpstring.h>
#include <idatraceman.h>




void TraceMgr::getTraceSettings(int argc, char** argv,
                                String& traceFileName,
                                int& traceLevel)
{
    int tfContained = 0;

    traceLevel = PFATAL;

    for (int i = 0; i < argc; i++)
    {
		// ---------------------------------------------------------------------
		// Trace Level
        if (strcmp(argv[i],"-tr") == 0)
        {
            if (i < (argc-1))
            {
                if (strstr(argv[i+1],"PNONE"))		traceLevel |= PNONE;
                if (strstr(argv[i+1],"PFATAL"))		traceLevel |= PFATAL;
                if (strstr(argv[i+1],"PEXCEPT"))	traceLevel |= PEXCEPT;
                if (strstr(argv[i+1],"PDATA"))		traceLevel |= PDATA;
                if (strstr(argv[i+1],"PTRACE"))		traceLevel |= PTRACE;
//                if (strstr(argv[i+1],"PCLTRACE"))	traceLevel |= PCLTRACE;
                if (strstr(argv[i+1],"PTEST"))		traceLevel |= PTEST;
                if (strstr(argv[i+1],"PSTATUS"))	traceLevel |= PSTATUS;
                if (strstr(argv[i+1],"PALL"))		traceLevel |= PALL;
            }
        }

		// ---------------------------------------------------------------------
		// Trace Filename
        if (strcmp(argv[i],"-tf") == 0)
        {
            *argv[i] = '\0';
            if (i < (argc-1))
            {
                tfContained = 1;
                traceFileName.assign (argv[i+1]);
            }
        }
    }
    if (!tfContained) traceFileName.clear();
}

