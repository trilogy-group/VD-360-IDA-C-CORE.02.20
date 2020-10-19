#!/usr/bin/ksh

#########################
# path settings
#########################
export OSA_ROOT=$HOME
export CL_ROOT_PATH=${OSA_ROOT}/classlib
export CL_WORKDIR_PATH=${OSA_ROOT}
export CL_LOG_PATH=${OSA_ROOT}/log/traces
export CL_REPORT_PATH=${OSA_ROOT}/log/reports
export IDA_ROOT=${OSA_ROOT}/ida

export PATH=$PATH:${CL_ROOT_PATH}/bin:${IDA_ROOT}/bin


#########################
# parameter file settings
#########################
export CL_AS_HELPBASE_URL="/cladmin/fileserver/doc/"
export CL_SYSTEM_GROUPS_FILE=${CL_ROOT_PATH}/dat/systemgroups.par
export CL_PRODUCT_GROUPS_FILE=${CL_WORKDIR_PATH}/config/productgroups.par
export CL_REPORTER_FILE=${CL_WORKDIR_PATH}/config/reporterprocess.par
export CL_GLOBAL_REPORTER_FILE=${CL_WORKDIR_PATH}/config/globalreporterprocess.par
export CL_NET_FILE="${CL_WORKDIR_PATH}/config/netprocess.par"
export CL_ROUTER_FILE="${CL_WORKDIR_PATH}/config/routertable.par"
export CL_ROUTER_OID=1000001
export CL_SVID_MAP_FILE="${CL_WORKDIR_PATH}/config/svidmapping.par"


#########################
# important for HP-UX
export UNIX95=	


#########################
# Library Path
#########################
# just export all three, no harm is done with too many, but if it's missing
# our process just won't start
# for Linux:
export LD_LIBRARY_PATH=$CL_ROOT_PATH/lib:${LD_LIBRARY_PATH}

# for AIX:
export LIBPATH=$CL_ROOT_PATH/lib:${LIBPATH}

# for HP-UX:
export SHLIB_PATH=$CL_ROOT_PATH/lib:${SHLIB_PATH}

. ${IDA_ROOT}/config/ida.env

#########################
# Tomcat Configuration
#########################
export CATALINA_HOME=/usr/share/tomcat5
export PATH=$PATH:${CATALINA_HOME}/bin
export CATALINA_BASE=${OSA_ROOT}/webserver/tomcat
export CATALINA_PID=${CATALINA_BASE}/logs/tomcat.pid

