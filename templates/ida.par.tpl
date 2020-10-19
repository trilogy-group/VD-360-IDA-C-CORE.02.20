//CB>---------------------------------------------------------------------
//
//  File:      ida.par.tpl
//  Revision:  1.1
//  Date:      08-AUG-2008 15:46:56
//
//  DESCRIPTION:
//    ida.par parameter file
//
//--------------------------------------------------------------------------
//<CE

// -------------------------------------------------------------------------------
// Please follow these instructions:
// - There must be one TdsClientGroup for every DAP
// - Define one subgroup for every TdfAccess-Object.
// - Maximum number of TdfAccess-Objects is 20 !!!
// - The TdfAccess-Objects will be automaticly enumerated starting at "1"
TdfProcessGroup 
SET
BEGIN
  objectid = #900797                    // OID (Range: 900001 - 910000)
  dbid = #1				                      // Database identification number
  dbserver_objectid = #@DB_OID@         // Database object id
  dbserverctl_objectid = #@DB_OID@      // Database object id
  service_name = "@SERVICE_NAME@"
  application_name = "360 Web"
  nrof_channels = #20
  osa_ticket = "${IDA_ROOT}/config/@IDA_OSA_TICKET@" // default is to replace IDA_OSA_TICKET by osaTicket.cfg
END
ENDSET
// -------------------------------------------------------------------------------
TimerAndMaxValueGroup
SET
BEGIN
  register_timer         = #60000       // Registrationtimer in [ms]
  search_timeout         = #10000       // Timer for queries in [ms]
  max_registration       = #10000       // Maximum number of registration attempts
END
ENDSET
// -------------------------------------------------------------------------------
WebProcessGroup
SET
BEGIN
  base_objectid    = #900700            // Base object address to which WebProcesses add an offset 
                                        // (Range: 900001 - 910000)  					
  base_socket_port = #@IDA_BASE_PORT@   // First Portnumber to be used
  max_web_procs    = #64                // maximum number of IdaWebProcesses
END
ENDSET



