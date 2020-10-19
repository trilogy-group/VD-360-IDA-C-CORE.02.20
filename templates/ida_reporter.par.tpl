//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//						  templates/ida_reporter.par.tpl 1.0 12-APR-2008 18:52:10 DMSYS
//
//   File:      templates/ida_reporter.par.tpl
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:10
//
//   DESCRIPTION:
//
//   #############################################################
//   #     Licensed material - property of Volt Delta GmbH       #
//   #              Copyright (c) Volt Delta GmbH                #
//   #############################################################
//
//
//<CE-------------------------------------------------------------------

// "@(#) templates/ida_reporter.par.tpl 1.0 12-APR-2008 18:52:10 DMSYS"

Level SET
BEGIN
    reportLevel = "event"
    reportClass = "IDA"
    deviceId= #111
END
BEGIN
    reportLevel = "problem"
    reportClass = "IDA"
    deviceId= #112
END
BEGIN
    reportLevel = "alarm"
    reportClass = "IDA"
    deviceId= #113
END
BEGIN
    reportLevel = "all"
    reportClass = "IDA"
    deviceId= #114
END
ENDSET


Device SET
BEGIN
    deviceType = "file"
    deviceId     = #111
    reportDestination      = "${CL_REPORT_PATH}/ida.Event.rep"
    unbufferedSave="true"
END
BEGIN
    deviceType = "file"
    deviceId     = #112
    reportDestination      = "${CL_REPORT_PATH}/ida.Problem.rep"
    unbufferedSave="true"
END
BEGIN
    deviceType = "file"
    deviceId     = #113
    reportDestination      = "${CL_REPORT_PATH}/ida.Alarm.rep"
    unbufferedSave="true"
END
BEGIN
    deviceType = "file"
    deviceId     = #114
    reportDestination      = "${CL_REPORT_PATH}/ida.ALL.rep"
    unbufferedSave="true"
END
ENDSET
   
Applreports SET
BEGIN
    reportClass = "IDA"
    reportClassId = #8301
    reportTextDefFile  = "${IDA_ROOT}/dat/ida.rtd"
END
ENDSET
