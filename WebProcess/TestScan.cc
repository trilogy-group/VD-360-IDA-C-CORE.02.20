//CB>-------------------------------------------------------------------
// 
//   File, Component, Release:
//                  TestScan.cc 1.1
// 
//   File:      TestScan.cc
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:57
// 
//   DESCRIPTION:
//     Test program for YpHtmlScanner.
//     
//   #############################################################
//   #     Licensed material - property of pc-plus COMPUTING     #
//   #              Copyright (c) pc-plus COMPUTING              #
//   #############################################################
//     
//   
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_YpHtmlScanner_cc = "@(#) TestScan.cc 1.1";



#include <fstream.h>
#include <stdlib.h>
#include <YpHtmlScanner.h>

char outBuffer[16384];




/** =============================================================== **/
/** ==                 H a u p t p r o g r a m m                 == **/
/** =============================================================== **/

int main ( )
{
    char tplFileName[256];
    char tokenValue[1024];
    YpHtmlScanner htmlScanner;
    int i;

    std::cout << std::endl;
    std::cout << "---------  Testprogramm TestScan  ----------" << std::endl;
    std::cout << std::endl;
    std::cout << "Bitte Template-Dateiname eingeben > "; 
    cin >> tplFileName;
    std::cout << tplFileName << std::endl << std::endl;

    try
    {
        htmlScanner.initialize (tplFileName);
        htmlScanner.reset ();
    }
    catch (const YpHttpException& exptn)
    {
        std::cout << std::endl << std::endl;
        std::cout << "+++ Datei " << tplFileName << " nicht gefunden.";
        std::cout << std::endl << std::endl;
        return(0);
    }

    YpHtmlScanner::TokenId curToken;
    YpHtmlScanner::MacroType macroType;
    const char* valPtr;
    int valLen, outBufPtr=0;
    do
    {
        curToken = htmlScanner.getNextToken ();
        htmlScanner.getValue (valPtr, valLen);
        strncpy (outBuffer+outBufPtr,valPtr,valLen);
        outBufPtr += valLen;
        switch (curToken)
        {
            case YpHtmlScanner::WHITE_SPACE :
                *tokenValue = '\0';
                for (i=0; i<valLen; i++)
                {
                    if (*(valPtr+i) == ' ')
                        strcat (tokenValue,"<SP>");
                    else
                        strcat (tokenValue,"<TAB>");
                }
                break;
            case YpHtmlScanner::NEW_LINE :
                strcpy (tokenValue, "<CR>");
                break;
            default :
                strncpy (tokenValue, valPtr, valLen);
                tokenValue[valLen] = '\0';
        }
        std::cout << YpHtmlScanner::getTokenName (curToken);
        std::cout << " | " << tokenValue;
        if (curToken == YpHtmlScanner::YP_MACRO)
        {
            macroType = htmlScanner.getMacroType ();
            std::cout << " | " << YpHtmlScanner::getMacroName (macroType);
        }
        std::cout << std::endl;
    }
    while (curToken != YpHtmlScanner::END_OF_INPUT);
    outBuffer[outBufPtr] = '\0';
    ofstream outFile ("TestScan.output");
    outFile << outBuffer;
    return( 0 );
}

