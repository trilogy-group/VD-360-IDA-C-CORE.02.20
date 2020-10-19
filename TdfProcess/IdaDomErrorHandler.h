#ifndef IdaDomErrorHandler_h
#define IdaDomErrorHandler_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaDomErrorHandler.h 1.1
//
//   File:      IdaDomErrorHandler.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:55
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------


static const char * SCCS_Id_IdaDomErrorHandler_h = "@(#) IdaDomErrorHandler.h 1.1";



////////////////////////////////////////////////////////////////////////////////////////////////////
/**
*/

#include <xercesc/sax/ErrorHandler.hpp>
XERCES_CPP_NAMESPACE_USE


class DomErrorHandler : public ErrorHandler
{
	public:
		// -----------------------------------------------------------------------
		//  Constructors and Destructor
		// -----------------------------------------------------------------------
		DomErrorHandler(String& errStr) : errorString(errStr)
		{
		}
	
		~DomErrorHandler()
		{
		} 
	
	
		// -----------------------------------------------------------------------
		//  Implementation of the error handler interface
		// -----------------------------------------------------------------------
		void warning(const SAXParseException& toCatch);
		void error(const SAXParseException& toCatch);
		void fatalError(const SAXParseException& toCatch);
		void resetErrors();

	private:
		void buildErrorText(const SAXParseException& toCatch);

		String& errorString;
};



#endif	// IdaDomErrorHandler_h


// *** EOF ***


