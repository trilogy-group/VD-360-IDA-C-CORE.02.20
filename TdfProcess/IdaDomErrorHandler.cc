
//CB>---------------------------------------------------------------------------
// 
//   File, Component, Release:
//                  IdaDomErrorHandler.cc 1.1
// 
//   File:      IdaDomErrorHandler.cc
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:56
// 
//   DESCRIPTION:
//     
//     
//   
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaDomErrorHandler_cc = "@(#) IdaDomErrorHandler.cc 1.1";


#include <stdlib.h>
#include <string.h>
#include <pcpdefs.h>
#include <iostream.h>
#include <pcpstring.h>

#ifdef OS_AIX_4
  #define bool Bool
#endif
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <IdaCpd.h>
#include <IdaDomErrorHandler.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void DomErrorHandler::warning(const SAXParseException&)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void DomErrorHandler::error(const SAXParseException& toCatch)
{
	buildErrorText(toCatch);
//	throw SAXParseException(toCatch);  // Copy the 'toCatch' object before throwing - 
                                       //   otherwise we would be throwing a reference to
                                       //   a local object that gets destroyed before
                                       //   the catch.
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void DomErrorHandler::fatalError(const SAXParseException& toCatch)
{
	buildErrorText(toCatch);
//	throw SAXParseException(toCatch);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void DomErrorHandler::resetErrors()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void DomErrorHandler::buildErrorText(const SAXParseException& toCatch)
{
   static char tmpStr[512];
   char* transMessage = XMLString::transcode(toCatch.getMessage());
   
   sprintf(tmpStr, "\nDOM parser error (L=%d,C=%d):\n%s\n", 
					toCatch.getLineNumber(),
					toCatch.getColumnNumber(),
					(char*)(Cpd(transMessage)));
   errorString = tmpStr;
   delete [] transMessage;
}



// *** EOF ***


