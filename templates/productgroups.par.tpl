//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//						  templates/productgroups.par.tpl 1.0 12-APR-2008 18:52:10 DMSYS
//
//   File:      templates/productgroups.par.tpl
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
//
//   DESCRIPTION:
//   Template for a parameter file for the supervisor's processgroups
//   specification
//
//<CE-------------------------------------------------------------------


Configuration
BEGIN
   netProcess="true"
   reporterProcess="true"
   routerServer="false"
   adminServer="true"
   reporterArgs    = "-rp ${CL_WORKDIR_PATH}/ida_reporter.par"
END

Group SET
BEGIN
    groupName        = "ida"
    parameterFile    = "${IDA_ROOT}/config/idaprocs.par"
    startNo          = #1
    autoStartSuccess = "dontcare"
    shutdown         = "explicit"
END
ENDSET

