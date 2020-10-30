workspace=${1:-`pwd`}
out=${2:-$workspace/dist}

# Set paths to CICD framework and build tools
export _360CICDFramework=${_360CICDFramework:-${workspace}/commonCICD}
export _360CICDTools=${_360CICDTools:-${workspace}/.cicd/tools}
export _360CICDTools_BI=${_360CICDTools_BI:-${_360CICDTools}/buildinfo/genreleaseinfo}

export OSAAPI_POOL="${workspace}/dependencies/osaapi/dist"
export MIS_POOL="${workspace}/dependencies/misdal/dist"
export CLASSLIB_POOL="${workspace}/dependencies/osaapi/dependencies/classlib/classlib"
export STL_POOL="${workspace}/dependencies/osaapi/dependencies/classlib/dependencies/stl"
export AMI_POOL="${workspace}/dependencies/spoc-ami/dist"
export XERCES_POOL="${XERCES_POOL:-/pool/xerces/03.02.001.14}"
export ICU_POOL="${ICU_POOL:-/pool/icu/05.08.002.14}"

export OSAAPIHOME="${OSAAPI_POOL}"
export STLPATH="${STL_POOL}"
export CLHOME="${CLASSLIB_POOL}"
export XERCESHOME="${XERCES_POOL}"
export ICUHOME="${ICU_POOL}"

export CLASSLIB_03_00=1
export USE_CLASSLIB_03_00=yes
export USE_CLASSLIB_03_10=yes
export USEDYNGENXML=yes
export USEDYNGENDLL=yes

export ARCH_32bit="${ARCH_32bit:-1}"

(
    echo "Building osaapi .."
    cd "${workspace}/dependencies/osaapi" &&\
        .cicd/build_dist.sh "${workspace}/dependencies/osaapi"
) &&\
(
    cd "${workspace}" &&\
    ./buildall &&\
    (
        mkdir -p "$out/modules" "$out/util" "$out/custom" "$out/lib" "$out/dat" "$out/templates" "$out/bin" "$out/config" "$out/support"
        cp "modules/"Ida* "$out/modules/"
        cp "bin/"* "$out/bin/"
        cp "dat/"* "$out/dat/"
        cp "templates/"* "$out/templates/"
        cp "$XERCES_POOL/lib/"*.so* "$out/lib/"
    )
)