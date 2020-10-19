//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//						  templates/idaprocs.par.tpl 1.0 12-APR-2008 18:52:11 DMSYS
//
//   File:      templates/idaprocs.par.tpl
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:11
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

// "@(#) templates/idaprocs.par.tpl 1.0 12-APR-2008 18:52:11 DMSYS"

Process SET
BEGIN
	name        = "${IDA_ROOT}/modules/IdaTdfProcess"
	args        = "${IDA_ROOT}/config/ida.par -tf ${TRACE_DIR}/IdaTdfProcess"
	groupName   = "ida"
	controlMode = "supervisor"
	processId   = #890
	startNo     = #1
	autoStart   = "false"
END
// -----------------------------------------------------------------------------------------
BEGIN
	name        = "${IDA_ROOT}/modules/IdaWebProcess"
	args        = "0 ${IDA_ROOT}/config/ida.par -tf ${TRACE_DIR}/IdaWebProcess1"
	processId   = #100
	groupName   = "ida"
   startNo     = #2
	controlMode = "supervisor"
        restartable = "false"
        autoStart   = "false"
END
// -----------------------------------------------------------------------------------------
BEGIN
	name        = "${IDA_ROOT}/modules/IdaWebProcess"
	args        = "1 ${IDA_ROOT}/config/ida.par -tf ${TRACE_DIR}/IdaWebProcess2"
	processId   = #101   
	groupName   = "ida"
	controlMode = "supervisor"
	restartable = "false"
	autoStart   = "false"
	startNo     = #3
END
// -----------------------------------------------------------------------------------------
BEGIN
	name        = "${IDA_ROOT}/modules/IdaWebProcess"
	args        = "2 ${IDA_ROOT}/config/ida.par -tf ${TRACE_DIR}/IdaWebProcess3"
	processId   = #102
	groupName   = "ida"
	controlMode = "supervisor"
	restartable = "false"
	autoStart   = "false"
	startNo     = #4
END
ENDSET
