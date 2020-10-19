#ifndef IDATRACE_H
#define IDATRACE_H

//CB>-------------------------------------------------------------------
//
//   Author:        Sigrun Peters
//
//   File, Component, Release:
//                  idatraceman.h 1.1
//
//   DESCRIPTION
//            This is the old compatible traceapi for the new tracesystem
//
//   File:      idatraceman.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:58
//
//<CE-------------------------------------------------------------------CE>
static const char * idatrace_h="@(#)  idatraceman.h 1.1";

#include <pcpdefs.h>
#ifndef CLASSLIB_03_00

#include <traceman.h>

#define idaTrackTrace(args) \
  writeTraceHelp (TraceMan::getTracePtr ( 1 ), PTRACE, 0, args )

#define idaTrackExcept(args) \
  writeTraceHelp (TraceMan::getTracePtr ( 1 ), PEXCEPT, 0, args )

#define idaTrackFatal(args) \
  writeTraceHelp (TraceMan::getTracePtr ( 1 ), PFATAL, 0, args )

#define idaTrackData( args ) \
  writeTraceHelp (TraceMan::getTracePtr ( 1 ), PDATA, 0, args )


#else

#include <sys/tracesystem.h>

//-----------------------------------------------------------------
//
//  use this macros instead of traceman::writeData
//
//-----------------------------------------------------------------


#define idaTrackDataBuffer( buffer, maxPrintLen, columnsPerLine ) \
  if ( traceSystem_IdaTraceLevel & PDATA )                          \
  {                                                            \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PDATA, __FILE__, __LINE__, traceSystem_method. methodName, traceSystem_category -> categoryName,      \
                                    traceIda, this, traceSystem_bufferToString ( buffer, maxPrintLen, columnsPerLine ), 0, 0 );  \
    myPtr -> releaseWriteLock ( );                                                                  \
  }



#define idaTrackDataCharBuffer( buffer, bufferLen, maxPrintLen, columnsPerLine ) \
  if ( traceIda & PDATA )                                     \
  {                                                                 \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );            \
    myPtr -> aquireWriteLock ( );                                   \
    myPtr -> writeTraceFct ( PDATA, __FILE__, __LINE__, traceSystem_method. methodName, traceSystem_category -> categoryName,      \
                                    traceIda, this, traceSystem_charToString ( buffer, bufferLen, maxPrintLen, columnsPerLine ), 0, 0 );  \
    myPtr -> releaseWriteLock ( );                                                                  \
  }




//-------------------------------------------------------------
//  Tracing Makros
//       Compatible to old tracing mechanism,
//          be warned it doesn't use new features like
//          categories or id's
//-------------------------------------------------------------

#define idaTrackFatal( args ) \
  if ( ::traceSystem_IdaTraceLevel & PFATAL )      \
  {                                           \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PFATAL,__FILE__, __LINE__, traceSystem_method. methodName, "",         \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );   \
    myPtr -> releaseWriteLock ( );                                                                  \
  }
#define idaTrackExcept( args ) \
  if ( ::traceSystem_IdaTraceLevel & PEXCEPT )   \
  {            \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                                   \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PEXCEPT,__FILE__, __LINE__, traceSystem_method. methodName, "",        \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );  \
    myPtr -> releaseWriteLock ( );                                                                  \
  }

#define idaTrackVisible( args ) \
  if ( ::traceSystem_IdaTraceLevel & PVISIBLE )  \
  {                                               \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PVISIBLE,__FILE__, __LINE__, traceSystem_method. methodName, "",       \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );   \
    myPtr -> releaseWriteLock ( );                                                                  \
  }

#define idaTrackTrace( args ) \
  if ( ::traceSystem_IdaTraceLevel & PTRACE )      \
  {                                            \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PTRACE,__FILE__, __LINE__, traceSystem_method. methodName, "",         \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );  \
    myPtr -> releaseWriteLock ( );                                                                  \
  }

#define idaTrackData( args ) \
  if ( ::traceSystem_IdaTraceLevel & PDATA )      \
  {    \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PDATA,__FILE__, __LINE__, traceSystem_method. methodName, "",          \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );  \
    myPtr -> releaseWriteLock ( );                                                                  \
  }


#define idaTrackStatus ( args ) \
  if ( ::traceSystem_IdaTraceLevel & PSTATUS )          \
  {                \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PSTATUS,__FILE__, __LINE__, traceSystem_method. methodName, "",        \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );   \
    myPtr -> releaseWriteLock ( );                                                                  \
  }
#define idaTrackTest ( args ) \
  if ( ::traceSystem_IdaTraceLevel & PTEST )          \
  {        \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( PTEST,__FILE__, __LINE__, traceSystem_method. methodName, "",          \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );    \
    myPtr -> releaseWriteLock ( );                                                                  \
  }


#define idaTrack( traceLevel, args ) \
  if ( traceSystem_IdaTraceLevel & traceLevel ) \
  {        \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
    myPtr -> aquireWriteLock ( );                                                                   \
    myPtr -> writeTraceFct ( traceLevel,__FILE__, __LINE__, traceSystem_method. methodName, "",     \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );    \
    myPtr -> releaseWriteLock ( );                                                                  \
  }

//mode will be set and unset
#define writeTrace ( traceLevel, mode, args ) \
  if ( traceSystem_IdaTraceLevel & traceLevel ) \
  {        \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );            \
    myPtr -> aquireWriteLock ( );                                   \
    UInt oldMode = myPtr -> getTraceMode ( );                       \
    myPtr -> setTraceMode ( mode );                                 \
    myPtr -> writeTraceFct ( traceLevel,__FILE__, __LINE__, traceSystem_method. methodName, "",                           \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 );    \
    myPtr -> setTraceMode ( oldMode );                                                        \
    myPtr -> releaseWriteLock ( );                                                            \
  }

//mode will be set and unset
//idx will be ignored
#define writeTraceX ( idx, traceLevel, mode, args ) \
  if ( traceSystem_IdaTraceLevel & traceLevel ) \
  {        \
  \
    TraceManager * myPtr = TraceManager::getTraceManager ( );             \
    myPtr -> aquireWriteLock ( );                                    \
    UInt oldMode = myPtr -> getTraceMode ( );                        \
    myPtr -> setTraceMode ( mode );                                  \
    myPtr -> writeTraceFct ( traceLevel,__FILE__, __LINE__, traceSystem_method. methodName, "",                        \
                                                          traceIda, 0, traceSystem_convertToString args, 0, 0 ); \
    myPtr -> setTraceMode ( oldMode );                                                        \
    myPtr -> releaseWriteLock ( );                                                            \
  }

// #define idaTrackAll ( args ) \
//   {                       \
//   \
//     TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
//     myPtr -> aquireWriteLock ( );                                                                   \
//     myPtr -> writeTraceFct ( PVISIBLE,__FILE__, __LINE__, traceSystem_method. methodName, "",       \
//                                                           traceIda, 0, traceSystem_convertToString args, 0, 0 );    \
//     myPtr -> releaseWriteLock ( );                                                                  \
//   }
// 
// #define idaTrackRemark ( args ) \
//   if ( ::traceSystem_IdaTraceLevel & PSTATUS )          \
//   {                                                \
//   \
//     TraceManager * myPtr = TraceManager::getTraceManager ( );                                            \
//     myPtr -> aquireWriteLock ( );                                                                   \
//     myPtr -> writeTraceFct ( PSTATUS,__FILE__, __LINE__, traceSystem_method. methodName, "",        \
//                                                           traceIda, 0, traceSystem_convertToString args, 0, 0 );   \
//     myPtr -> releaseWriteLock ( );                                                                  \
//   }
// 
// #define idaTrackSysErr( level, args, errno ) \
// if ( ::traceSystem_IdaTraceLevel & level )       \
// {\
//   \
//     TraceManager * myPtr = TraceManager::getTraceManager ( );                                          \
//     myPtr -> aquireWriteLock ( );                                                                 \
//     myPtr -> writeTraceFct ( level,__FILE__, __LINE__, traceSystem_method. methodName, "",        \
//                                                           traceIda, 0, traceSystem_convertToString args, 0, 0 );   \
//     myPtr -> writeTraceFct ( level,__FILE__, __LINE__, traceSystem_method. methodName, "",        \
//                                                           traceIda, 0, traceSystem_convertToString ("errno =%d: %s", errno, ( errno<sys_nerr ) ? strerror ( errno ): "" ), 0, 0 );                 \
//     myPtr -> releaseWriteLock ( );                                                                  \
// }

#include <IdaFunctionTrace.h>

#endif

#endif


