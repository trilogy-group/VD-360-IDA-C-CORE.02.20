param(
  [string]$workspace = $(pwd)
)

#########################################################

if (!$workspace -Or -Not (Test-Path "$workspace")) {
    Write-Output "workspace variable not set, exiting."
    exit 1
}

function DoInFolder([Object] $Directory, [scriptblock] $ScriptBlock)
{
    push-location "$Directory"
    Write-Output "Entered directory $Directory .."
    try {
        & $ScriptBlock
        if ($lastexitcode -ne 0) {
            throw ("DoInFolder: Failed with code $lastexitcode")
        }
    }
    finally {
        pop-location
        Write-Output "Exited directory $Directory .."
    }
}

function GetEnvOrDefault([string] $key, [string] $defaultVal = "") {
    $ret = [Environment]::GetEnvironmentVariable($key)
    if ($ret -eq $Null) {
        $ret = $defaultVal
    }
    Write-Output $ret
}

#########################################################

$env:_360CICDFramework = GetEnvOrDefault "_360CICDFramework" "$PSScriptRoot/../commonCICD"
$env:_360CICDTools = GetEnvOrDefault "_360CICDTools" "$PSScriptRoot/tools"
$env:_360CICDTools_BI = GetEnvOrDefault "_360CICDTools_BI" "$env:_360CICDTools/buildinfo/genreleaseinfo.bat"
$env:_360CICDTools_Conan = GetEnvOrDefault "_360CICDTools_Conan" "python ""$env:_360CICDTools/conan/conan.py"""

Import-Module -DisableNameChecking "$env:_360CICDFramework/deployment/lib/tools.psm1" -Scope Global

$out="dist"

$env:VSVER = GetEnvOrDefault "VSVER" "100"
$env:COMPILER_VERSION = GetEnvOrDefault "COMPILER_VERSION" "msvc10.0"

$env:POOL_ROOT = "${workspace}"
$env:OSAAPI_POOL = "${env:POOL_ROOT}\dependencies\osaapi\dist"
$env:MIS_POOL = "${env:POOL_ROOT}\dependencies\misdal\dist"
$env:STL_POOL = "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\dependencies\STL"
$env:REGEXP_POOL = "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\dependencies\regexp"
$env:CLASSLIB_POOL = "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\classlib"
$env:AMI_POOL = "${env:POOL_ROOT}\dependencies\spoc-ami\dist"

$env:POOL_ROOT = GetEnvOrDefault "POOL_ROOT" "${workspace}"
$env:OSAAPI_POOL = GetEnvOrDefault "OSAAPI_POOL" "${env:POOL_ROOT}\dependencies\osaapi\dist"
$env:MIS_POOL = GetEnvOrDefault "MIS_POOL" "${env:POOL_ROOT}\dependencies\misdal\dist"
$env:STL_POOL = GetEnvOrDefault "STL_POOL" "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\dependencies\STL"
$env:REGEXP_POOL = GetEnvOrDefault "REGEXP_POOL" "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\dependencies\regexp"
$env:CLASSLIB_POOL = GetEnvOrDefault "CLASSLIB_POOL" "${env:POOL_ROOT}\dependencies\osaapi\dependencies\classlib\classlib"
$env:AMI_POOL = GetEnvOrDefault "AMI_POOL" "${env:POOL_ROOT}\dependencies\spoc-ami\dist"

$env:XERCES_POOL = GetEnvOrDefault "XERCES_POOL" "C:\pool\xerces\2_11_0"
$env:ICU_POOL = GetEnvOrDefault "ICU_POOL" "C:\pool\icu\05.08\001.1\msvc14.0"

$env:MISDAL_POOL="${MIS_POOL}"
$env:XML_POOL="${XERCES_POOL}"

$env:CLASSLIB_03_00=1
$env:USE_CLASSLIB_03_00="yes"
$env:USE_CLASSLIB_03_10="yes"
$env:USEDYNGENXML="yes"
$env:USEDYNGENDLL="yes"

$env:ARCH_32bit = GetEnvOrDefault "ARCH_32bit" "1"

DoInFolder ("${env:OSAAPI_POOL}\..") {
    & "./.cicd/build_dist.ps1" "$pwd"
}

DoInFolder ("$workspace") {
    EnsureEmptyFolder "$workspace\$out"
    cmd.exe /c "$env:_360CICDTools_BI" "$workspace\BuildInfo.xml" "$workspace\ses\pool\ReleaseInfo.txt"

    cmd.exe /c "$workspace\build.bat"
    if ($lastexitcode -ne 0)
    {
        Write-Error "Build failed"
        exit 1
    }

}
