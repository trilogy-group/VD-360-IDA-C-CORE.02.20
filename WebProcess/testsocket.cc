//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  testsocket.cc 1.1
//
//   File:      testsocket.cc
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:49
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

static const char * SCCS_Id_testsocket_cc = "@(#) testsocket.cc 1.1";


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
        std::cout << "Usage: testsocket querystring portnr" << std::endl;
        std::cout << "e.g.:  testsocket \"NAM=pc-plus&QRY=all&HRA=mymachine\" 8888" << std::endl;
        exit (1);
    }

    queryString = argv[1];
    portnr = atoi ( argv[2] );

    // Determine hostname
    String hostname = getHostName();
    if (hostname == "")
    {
        std::cout << "ERROR: getHostName failed" << std::endl;
        exit(1);                
    }

    // Open connection to server
    if (client.connect( hostname, portnr ) == isOk)
    {
        clientData = client;
//        std::cout << "Client connecting to hostname " << hostname << " and port # " << portnr << std::endl;
    }
    else
    {
        std::cout << "ERROR: Could not connect to port # " << portnr << std::endl;
        exit (1);
    }

    if (clientData.writeStream(queryString, strlen(queryString), len) == isOk)
    {
//        std::cout << "Client sent: " << queryString << std::endl;          
    }
    else
    {
        std::cout << "ERROR: Could not send data" << std::endl;
        exit (1);
    }

    len=0;
    char * bufferPtr = buffer;
    while (clientData.readStream(bufferPtr, 100000, len) == isOk)
    {
        bufferPtr+=len;
//        std::cout << "Len: " << len << std::endl;          
//        std::cout << "Client received: " << buffer << std::endl;           
    }
/*
    else
    {
        std::cout << "ERROR: Could not receive response" << std::endl;
        exit (1);
    }
*/

    std::cout << buffer;

    exit(0);
}
