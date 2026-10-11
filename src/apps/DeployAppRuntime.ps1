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

# Deploy Qt modules/plugins and the compiler runtime first.
& $WinDeployQt --release --no-translations --compiler-runtime --dir $appDir $appExePath
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed for '$appExePath' with exit code $LASTEXITCODE"
}

# windeployqt can deploy every Qt SQL driver plugin, even though Blastmaster
# Databases uses SQLite only. Other drivers (notably qsqlibase.dll) pull in
# unrelated clients such as fbclient.dll. Keep only qsqlite for Databases;
# remove SQL drivers entirely from apps that do not use Qt SQL.
$sqlDriverDir = Join-Path $appDir "sqldrivers"
$isDatabaseApp = [System.IO.Path]::GetFileName($appExePath).Equals(
    "blastmaster_database.exe",
    [System.StringComparison]::OrdinalIgnoreCase
)
if (Test-Path -LiteralPath $sqlDriverDir -PathType Container) {
    Get-ChildItem -LiteralPath $sqlDriverDir -File -Filter "*.dll" |
        Where-Object {
            -not ($isDatabaseApp -and $_.Name.Equals("qsqlite.dll", [System.StringComparison]::OrdinalIgnoreCase))
        } |
        ForEach-Object {
            Write-Host "Removing unused Qt SQL driver plugin: $($_.Name)"
            Remove-Item -LiteralPath $_.FullName -Force
        }
    if (-not $isDatabaseApp -and -not (Get-ChildItem -LiteralPath $sqlDriverDir -Force | Select-Object -First 1)) {
        Remove-Item -LiteralPath $sqlDriverDir -Force
    }
}
if ($isDatabaseApp -and -not (Test-Path -LiteralPath (Join-Path $sqlDriverDir "qsqlite.dll") -PathType Leaf)) {
    throw "The SQLite SQL driver qsqlite.dll was not deployed for the Databases app."
}

# Resolve the matching MinGW objdump. Static DLL lists are brittle: Qt/MSYS2
# updates rename dependencies, and different apps import different DLL sets.
$objdumpCommand = Get-Command objdump.exe -ErrorAction SilentlyContinue
if (-not $objdumpCommand) {
    $objdumpCommand = Get-Command objdump -ErrorAction SilentlyContinue
}
if (-not $objdumpCommand) {
    throw "objdump was not found on PATH. Run this from the MSYS2 UCRT64 environment with mingw-w64-ucrt-x86_64-binutils installed."
}
$objdump = $objdumpCommand.Source

# Windows-provided DLLs do not belong in the application folder. API-set
# imports are virtual names resolved by Windows, not files to package.
$windowsDlls = @(
    "advapi32.dll","bcrypt.dll","bcryptprimitives.dll","cfgmgr32.dll",
    "combase.dll","comctl32.dll","comdlg32.dll","crypt32.dll","cryptbase.dll",
    "dwmapi.dll","gdi32.dll","imm32.dll","iphlpapi.dll","kernel32.dll",
    "kernelbase.dll","msvcrt.dll","ntdll.dll","ole32.dll","oleaut32.dll",
    "powrprof.dll","profapi.dll","rpcrt4.dll","sechost.dll","setupapi.dll",
    "shell32.dll","shlwapi.dll","user32.dll","uxtheme.dll","version.dll",
    "win32u.dll","winmm.dll","ws2_32.dll","wtsapi32.dll"
)

function Get-ImportedDlls([string]$BinaryPath) {
    $output = & $objdump -p $BinaryPath 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "objdump could not inspect '$BinaryPath': $($output -join [Environment]::NewLine)"
    }
    foreach ($line in $output) {
        if ("$line" -match '^\s*DLL Name:\s*(\S+)\s*$') {
            $Matches[1].ToLowerInvariant()
        }
    }
}

function Find-Dll([string]$DllName) {
    $candidates = @(
        (Join-Path $appDir $DllName),
        (Join-Path $qtBinDir $DllName)
    )
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return (Get-Item -LiteralPath $candidate).FullName
        }
    }

    # Search the toolchain's PATH entries, but never silently accept a
    # dependency from an arbitrary working directory.
    foreach ($pathEntry in ($env:PATH -split ';')) {
        if ([string]::IsNullOrWhiteSpace($pathEntry)) { continue }
        $candidate = Join-Path $pathEntry.Trim('"') $DllName
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            $resolved = (Get-Item -LiteralPath $candidate).FullName
            # Never copy Windows-owned binaries from System32/SysWOW64 into the app.
            $windowsRoot = [System.IO.Path]::GetFullPath($env:WINDIR).TrimEnd('\\') + '\\'
            if ($resolved.StartsWith($windowsRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
                continue
            }
            return $resolved
        }
    }
    return $null
}

$queue = [System.Collections.Generic.Queue[string]]::new()
$scanned = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$queue.Enqueue($appExePath)

# Inspect all app binaries, including Qt plugins deployed in subdirectories.
Get-ChildItem -LiteralPath $appDir -Recurse -File |
    Where-Object { $_.Extension -in @('.dll', '.exe') } |
    ForEach-Object { $queue.Enqueue($_.FullName) }

while ($queue.Count -gt 0) {
    $binary = $queue.Dequeue()
    if (-not $scanned.Add([System.IO.Path]::GetFullPath($binary))) { continue }

    foreach ($dllName in (Get-ImportedDlls $binary | Sort-Object -Unique)) {
        if ($dllName -like 'api-ms-win-*.dll' -or
            $dllName -like 'ext-ms-win-*.dll' -or
            $windowsDlls -contains $dllName) {
            continue
        }

        # Treat any DLL actually present in the Windows system directories as
        # an OS dependency, even when it is not in the curated common-DLL list
        # (for example D3Dcompiler_47.dll).
        $systemDllFound = $false
        foreach ($systemDir in @(
            (Join-Path $env:WINDIR 'System32'),
            (Join-Path $env:WINDIR 'SysWOW64')
        )) {
            if (Test-Path -LiteralPath (Join-Path $systemDir $dllName) -PathType Leaf) {
                $systemDllFound = $true
                break
            }
        }
        if ($systemDllFound) { continue }

        $destination = Join-Path $appDir $dllName
        if (Test-Path -LiteralPath $destination -PathType Leaf) {
            $queue.Enqueue((Get-Item -LiteralPath $destination).FullName)
            continue
        }

        $source = Find-Dll $dllName
        if (-not $source) {
            throw "Unresolved non-Windows dependency '$dllName' imported by '$binary'. Install the matching MSYS2 UCRT64 package or add its bin directory to PATH, then rerun deployment."
        }

        if ([System.IO.Path]::GetFullPath($source) -ne [System.IO.Path]::GetFullPath($destination)) {
            Copy-Item -LiteralPath $source -Destination $destination -Force
        }
        Write-Host "Packaged dependency: $dllName (required by $([System.IO.Path]::GetFileName($binary)))"
        $queue.Enqueue($destination)
    }
}

Write-Host "Runtime dependency audit passed for $appExePath"
