param(
    [Parameter(Mandatory = $true)]
    [string]$SetupExe,

    [Parameter(Mandatory = $true)]
    [string]$WinDeployQt
)

$ErrorActionPreference = "Stop"

$setupExePath = [System.IO.Path]::GetFullPath($SetupExe)
$setupDir = [System.IO.Path]::GetDirectoryName($setupExePath)
$qtBinDir = [System.IO.Path]::GetDirectoryName(
    [System.IO.Path]::GetFullPath($WinDeployQt))

if (-not (Test-Path -LiteralPath $setupExePath -PathType Leaf)) {
    throw "Setup executable was not found: $setupExePath"
}
if (-not (Test-Path -LiteralPath $WinDeployQt -PathType Leaf)) {
    throw "windeployqt was not found: $WinDeployQt"
}

# Deploy Qt DLLs and the compiler runtime required to launch the setup UI.
& $WinDeployQt --release --no-translations --compiler-runtime --dir $setupDir $setupExePath
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

# These MinGW/MSYS2 runtime DLLs were observed to be required by this build.
# windeployqt does not consistently discover every non-Qt transitive DLL.
$requiredRuntimeDlls = @(
    "libwinpthread-1.dll",
    "libb2-1.dll"
)

foreach ($dllName in $requiredRuntimeDlls) {
    $source = Join-Path $qtBinDir $dllName
    $destination = Join-Path $setupDir $dllName

    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required runtime DLL '$dllName' was not found beside windeployqt ('$qtBinDir'). Install the matching MSYS2 UCRT64 package or update the runtime search path."
    }

    if ([System.IO.Path]::GetFullPath($source) -ne [System.IO.Path]::GetFullPath($destination)) {
        Copy-Item -LiteralPath $source -Destination $destination -Force
    }
    Write-Host "Packaged runtime DLL: $destination"
}
