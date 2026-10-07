param(
    [Parameter(Mandatory=$true)]
    [string]$PayloadRoot,

    [string]$SetupExe,

    [string]$InstallRoot
)

$ErrorActionPreference = "Stop"

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) {
        throw "FAIL: $Message"
    }
    Write-Host "PASS: $Message"
}

$common = Join-Path $PayloadRoot "common"
$standard = Join-Path $PayloadRoot "standard"
$professional = Join-Path $PayloadRoot "professional"

Assert-True (Test-Path $common -PathType Container) "common payload exists"
Assert-True (Test-Path $standard -PathType Container) "standard payload exists"

Assert-True (Test-Path (Join-Path $standard "blastmaster_docs.exe")) "Standard contains Docs"
Assert-True (Test-Path (Join-Path $standard "blastmaster_workbooks.exe")) "Standard contains Workbooks"
Assert-True (Test-Path (Join-Path $standard "blastmaster_presentations.exe")) "Standard contains Presentations"
Assert-True (-not (Test-Path (Join-Path $standard "blastmaster_database.exe"))) "Standard excludes Databases"

if (Test-Path $professional -PathType Container) {
    Assert-True (Test-Path (Join-Path $professional "blastmaster_database.exe")) "Professional contains Databases"
}

if ($SetupExe) {
    Assert-True (Test-Path $SetupExe -PathType Leaf) "Setup executable exists"

    $bytes = [System.IO.File]::ReadAllBytes($SetupExe)
    Assert-True ($bytes.Length -ge 16) "Setup executable is large enough for payload footer"

    $magic = [System.Text.Encoding]::ASCII.GetString($bytes, $bytes.Length - 16, 8)
    Assert-True ($magic -eq "BMSPAY01") "Setup executable has the embedded payload marker"

    $offsetBytes = $bytes[($bytes.Length - 8)..($bytes.Length - 1)]
    [UInt64]$offset = 0
    for ($i = 0; $i -lt 8; $i++) {
        $offset = $offset -bor ([UInt64]$offsetBytes[$i] -shl (8 * $i))
    }

    Assert-True ($offset -lt [UInt64]($bytes.Length - 16)) "Embedded payload offset is inside the Setup executable"
}

if ($InstallRoot) {
    Assert-True (Test-Path $InstallRoot -PathType Container) "Installed directory exists"

    $apps = @(
        @{ Name = "Docs"; Extension = ".dccx"; Exe = "blastmaster_docs.exe"; ProgId = "Blastmaster.Docs" },
        @{ Name = "Workbooks"; Extension = ".wkbx"; Exe = "blastmaster_workbooks.exe"; ProgId = "Blastmaster.Workbooks" },
        @{ Name = "Presentations"; Extension = ".prex"; Exe = "blastmaster_presentations.exe"; ProgId = "Blastmaster.Presentations" }
    )

    foreach ($app in $apps) {
        $extPath = "HKCU:\Software\Classes\$($app.Extension)"
        $progPath = "HKCU:\Software\Classes\$($app.ProgId)"
        Assert-True ((Get-ItemProperty -Path $extPath -Name '(default)' -ErrorAction SilentlyContinue).'(default)' -eq $app.ProgId) "$($app.Extension) points to $($app.ProgId)"
        Assert-True (Test-Path $progPath) "$($app.ProgId) exists"
        Assert-True (Test-Path (Join-Path $InstallRoot $app.Exe)) "$($app.Name) executable exists"
    }

    $dbExe = Join-Path $InstallRoot "blastmaster_database.exe"
    if (Test-Path $dbExe) {
        $extPath = "HKCU:\Software\Classes\.dbbx"
        Assert-True ((Get-ItemProperty -Path $extPath -Name '(default)' -ErrorAction SilentlyContinue).'(default)' -eq "Blastmaster.Databases") ".dbbx points to Blastmaster.Databases"
        Assert-True (Test-Path "HKCU:\Software\Classes\Blastmaster.Databases") "Blastmaster.Databases exists"
    } else {
        Assert-True (-not (Test-Path "HKCU:\Software\Classes\.dbbx")) "Standard install does not register .dbbx"
    }
}

Write-Host ""
Write-Host "Setup verification completed successfully."
Write-Host "For uninstall verification, run this script with the install removed and confirm:"
Write-Host "  - the install directory is gone"
Write-Host "  - HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\Blastmaster Suite is gone"
Write-Host "  - the Blastmaster Suite Start Menu folder is gone"
Write-Host "  - Blastmaster Docs/Workbooks/Presentations/Databases ProgIDs are gone"
Write-Host "  - Blastmaster-owned extensions are gone or restored to their previous owners"
