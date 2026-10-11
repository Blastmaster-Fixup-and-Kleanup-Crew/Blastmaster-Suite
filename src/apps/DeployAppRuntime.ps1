param(
    [Parameter(Mandatory = $true)]
    [string]$AppExe,

    [Parameter(Mandatory = $true)]
    [string]$WinDeployQt
)

$ErrorActionPreference = "Stop"
$appExePath = [System.IO.Path]::GetFullPath($AppExe)
$appDir = [System.IO.Path]::GetDirectoryName($appExePath)
$qtBinDir = [System.IO.Path]::GetDirectoryName([System.IO.Path]::GetFullPath($WinDeployQt))

if (-not (Test-Path -LiteralPath $appExePath -PathType Leaf)) {
    throw "Application executable was not found: $appExePath"
}
if (-not (Test-Path -LiteralPath $WinDeployQt -PathType Leaf)) {
    throw "windeployqt was not found: $WinDeployQt"
}

# Deploy Qt modules and the matching MinGW compiler runtime beside the app.
& $WinDeployQt --release --no-translations --compiler-runtime --dir $appDir $appExePath
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed for '$appExePath' with exit code $LASTEXITCODE"
}

# Extra MSYS2/UCRT64 runtime dependencies that may not be discovered by windeployqt.
foreach ($dllName in @(
    "libwinpthread-1.dll",
    "libstdc++-6.dll",
    "libgcc_s_seh-1.dll",
    "libb2-1.dll",
    "libdouble-conversion.dll",
    "libicuin78.dll",
    "libicuuc78.dll",
    "libpcre2-16-0.dll",
    "zlib1.dll",
    "libzstd.dll",
    "libfreetype-6.dll",
    "libharfbuzz-0.dll",
    "libmd4c.dll",
    "libpng16-16.dll",
    "libicudt78.dll",
    "libbrotlidec.dll",
    "libbz2-1.dll",
    "libglib-2.0-0.dll",
    "libgraphite2.dll",
    "libbritlicommon.dll"
)) {
    $source = Join-Path $qtBinDir $dllName
    $destination = Join-Path $appDir $dllName

    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required runtime DLL '$dllName' was not found beside windeployqt ('$qtBinDir'). Install the matching MSYS2 UCRT64 package or update the runtime path."
    }

    if ([System.IO.Path]::GetFullPath($source) -ne [System.IO.Path]::GetFullPath($destination)) {
        Copy-Item -LiteralPath $source -Destination $destination -Force
    }
    Write-Host "Packaged app runtime DLL: $destination"
}
