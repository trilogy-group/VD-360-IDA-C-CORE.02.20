//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  WebProcess/testsocket.cc 1.0 12-APR-2008 18:52:12 DMSYS
//
//   File:      WebProcess/testsocket.cc
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:12
//
//   DESCRIPTION:
//     Programm to test socket interface if IDA_CGI_Process
//
//   #############################################################
//   #     Licensed material - property of pc-plus COMPUTING     #
//   #              Copyright (c) pc-plus COMPUTING              #
//   #############################################################
//
//
//<CE-------------------------------------------------------------------

static const char * SCCS_Id_testsocket_cc = "@(#) WebProcess/testsocket.cc 1.0 12-APR-2008 18:52:12 DMSYS";


#include "streamsocketdata.h"
#include "clientsockettcp.h"
#include "serversockettcp.h"
#include "buffer.h"

String getHostName()
{       
    char tmpHostName[64];
    if (gethostname ( tmpHostName, 32 ) != 0)
    {
        return "";
    }
    String tmpHostString(tmpHostName);
    return tmpHostString;
}


int main(int argc, char **argv, char **envp)
{
    ClientSocketTcp  client;
    StreamSocketData clientData;
    ServerSocketTcp  server;
    char buffer [100000];
    char * queryString;
    int portnr;
    long len;

    // Analyze arguments
    if (argc != 3)
    {
        cout << "Usage: testsocket querystring portnr" << endl;
        cout << "e.g.:  testsocket \"NAM=pc-plus&QRY=all&HRA=mymachine\" 8888" << endl;
        exit (1);
    }

    queryString = argv[1];
    portnr = atoi ( argv[2] );

    // Determine hostname
    String hostname = getHostName();
    if (hostname == "")
    {
        cout << "ERROR: getHostName failed" << endl;
        exit(1);                
    }

    // Open connection to server
    if (client.connect( hostname, portnr ) == isOk)
    {
        clientData = client;
//        cout << "Client connecting to hostname " << hostname << " and port # " << portnr << endl;
    }
    else
    {
        cout << "ERROR: Could not connect to port # " << portnr << endl;
        exit (1);
    }

    if (clientData.writeStream(queryString, strlen(queryString), len) == isOk)
    {
//        cout << "Client sent: " << queryString << endl;          
    }
    else
    {
        cout << "ERROR: Could not send data" << endl;
        exit (1);
    }

    len=0;
    char * bufferPtr = buffer;
    while (clientData.readStream(bufferPtr, 100000, len) == isOk)
    {
        bufferPtr+=len;
//        cout << "Len: " << len << endl;          
//        cout << "Client received: " << buffer << endl;           
    }
/*
    else
    {
        cout << "ERROR: Could not receive response" << endl;
        exit (1);
    }
*/

    cout << buffer;

    exit(0);
}
