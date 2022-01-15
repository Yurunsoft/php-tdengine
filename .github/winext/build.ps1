#php extension builder

param (
    [string]$ToolsPath = "C:\tools\phpdev",
    [string]$ExtPath = ".",
    [string]$ExtName,
    [parameter(ValueFromRemainingArguments = $true)]
    [string[]]$ExtraArgs
)

$scriptPath = Split-Path -parent $MyInvocation.MyCommand.Definition
. "$scriptPath\utils.ps1" -ToolName "build" -MaxTry $MaxTry

info "Start building php extension"
$origwd = (Get-Location).Path
Set-Location $ExtPath

info "Phpize it"
phpize.bat
if (0 -Ne $lastexitcode){
    err "Failed phpize it, are we at ext dir?"
    Set-Location $origwd
    exit 1
}
$buildargs = ,".\configure.bat"
if("".Equals($ExtName)){
    warn "No ext name given, will not prepend --enable-extname arg"
}else{
    $buildargs += ,"--enable-$ExtName"
}
$buildargs += ,"--with-php-build=$ToolsPath\deps"
$buildargs += $ExtraArgs
info "Configure it with arg $buildargs"
Invoke-Expression "$buildargs"
if (0 -Ne $lastexitcode){
    err "Failed configure."
    Set-Location $origwd
    exit 1
}

if ("${env:FIX_PICKLE}" -Eq "1"){
    info "Modify config.pickle.h to avoid C4005"
    $picklefn = "${env:DEVPACK_PATH}\include\main\config.pickle.h"
    $orig = Get-Content -Raw $picklefn
    $modified = $orig

    $definitions = @(
        "PHP_BUILD_SYSTEM",
        "PHP_BUILD_PROVIDER",
        "PHP_LINKER_MAJOR",
        "PHP_LINKER_MINOR",
        "__SSE__",
        "__SSE2__",
        "__SSE3__",
        "__SSSE3__",
        "__SSE4_1__",
        "__SSE4_2__",
        "PHP_SIMD_SCALE"
    )
    foreach ($definition in $definitions){
        $re = "^(\s*#\s*define\s*" + $definition + ".+)$"
        $modified = $modified -Replace ($re, '// $1')
    }
    # avoid utf8 bom
    [System.IO.File]::WriteAllLines($picklefn, $modified)
}

info "Start nmake"
nmake
if (0 -Ne $lastexitcode){
    err "Failed nmake."
    Set-Location $origwd
    exit 1
}
Set-Location $origwd

exit 0
