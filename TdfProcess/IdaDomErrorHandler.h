#ifndef IdaDomErrorHandler_h
#define IdaDomErrorHandler_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  TdfProcess/IdaDomErrorHandler.h 1.0 12-APR-2008 18:52:10 DMSYS
//
//   File:      TdfProcess/IdaDomErrorHandler.h
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------


static const char * SCCS_Id_IdaDomErrorHandler_h = "@(#) TdfProcess/IdaDomErrorHandler.h 1.0 12-APR-2008 18:52:10 DMSYS";



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


