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
Assert-True (Test-Path (Join-Path $common "BlastmasterHelp.exe")) "common payload contains Help launcher"
Assert-True (Test-Path (Join-Path $common "blastmaster_suite_setup.svg")) "common payload contains setup branding"

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
    Assert-True (Test-Path (Join-Path $InstallRoot "BlastmasterHelp.exe")) "Installed Help launcher exists"

    $apps = @(
        @{ Name = "Docs"; Extension = ".dccx"; Exe = "blastmaster_docs.exe"; ProgId = "Blastmaster.Docs" },
        @{ Name = "Workbooks"; Extension = ".wkbx"; Exe = "blastmaster_workbooks.exe"; ProgId = "Blastmaster.Workbooks" },
        @{ Name = "Presentations"; Extension = ".prex"; Exe = "blastmaster_presentations.exe"; ProgId = "Blastmaster.Presentations" }
    )

    foreach ($app in $apps) {
        $extPath = "HKCU:SoftwareClasses$($app.Extension)"
        $progPath = "HKCU:SoftwareClasses$($app.ProgId)"
        $default = (Get-ItemProperty -Path $extPath -Name '(default)' -ErrorAction SilentlyContinue).'(default)'
        Assert-True ($default -eq $app.ProgId) "$($app.Extension) points to $($app.ProgId)"
        Assert-True (Test-Path $progPath) "$($app.ProgId) exists"
        Assert-True (Test-Path (Join-Path $InstallRoot $app.Exe)) "$($app.Name) executable exists"
    }

    $startMenu = Join-Path $env:APPDATA "MicrosoftWindowsStart MenuProgramsBlastmaster Suite"
    Assert-True (Test-Path (Join-Path $startMenu "Blastmaster Suite Help.lnk")) "Start Menu contains Blastmaster Suite Help"

    $dbExe = Join-Path $InstallRoot "blastmaster_database.exe"
    if (Test-Path $dbExe) {
        $extPath = "HKCU:SoftwareClasses.dbbx"
        $default = (Get-ItemProperty -Path $extPath -Name '(default)' -ErrorAction SilentlyContinue).'(default)'
        Assert-True ($default -eq "Blastmaster.Databases") ".dbbx points to Blastmaster.Databases"
        Assert-True (Test-Path "HKCU:SoftwareClassesBlastmaster.Databases") "Blastmaster.Databases exists"
        Assert-True (Test-Path (Join-Path $startMenu "Databases.lnk")) "Professional Start Menu contains Databases"
    } else {
        Assert-True (-not (Test-Path "HKCU:SoftwareClasses.dbbx")) "Standard install does not register .dbbx"
        Assert-True (-not (Test-Path (Join-Path $startMenu "Databases.lnk"))) "Standard Start Menu excludes Databases"
    }
}

Write-Host ""
Write-Host "Setup verification completed successfully."
Write-Host "For upgrade/reinstall verification:"
Write-Host "  1. Place a test file in the existing installation directory."
Write-Host "  2. Run Setup again against the same edition and destination."
Write-Host "  3. Confirm the test file remains and the application files are refreshed."
Write-Host ""
Write-Host "For uninstall verification, confirm:"
Write-Host "  - the install directory is gone"
Write-Host "  - HKCU:SoftwareMicrosoftWindowsCurrentVersionUninstallBlastmaster Suite is gone"
Write-Host "  - the Blastmaster Suite Start Menu folder is gone"
Write-Host "  - Blastmaster Docs/Workbooks/Presentations/Databases ProgIDs are gone"
Write-Host "  - Blastmaster-owned extensions are gone or restored to their previous owners"


Write-Host ""
Write-Host "Low-priority setup checks:"
$setupSource = Join-Path $PSScriptRoot "SetupWizard.cpp"
if (Test-Path $setupSource) {
    $setupText = Get-Content -Raw -LiteralPath $setupSource
    if ($setupText -match 'setAccessibleName' -and $setupText -match 'setAccessibleDescription') {
        Write-Host "PASS: installer controls expose accessibility metadata."
    } else {
        Write-Warning "Accessibility metadata was not found in SetupWizard.cpp."
    }
    if ($setupText -match 'tr("') {
        Write-Host "PASS: installer UI uses Qt translation-aware strings."
    } else {
        Write-Warning "Translation-aware strings were not found in SetupWizard.cpp."
    }
}

$translationDir = Join-Path $PSScriptRoot "translations"
if ((Test-Path $translationDir) -and (Get-ChildItem -LiteralPath $translationDir -Filter "*.ts" -File).Count -gt 0) {
    Write-Host "PASS: Qt Linguist catalogs are present."
} else {
    Write-Warning "No Qt Linguist catalogs were found."
}

if ($env:BLASTMASTER_ENABLE_SIGNING -eq "1") {
    if ($env:BLASTMASTER_SIGNTOOL -or (Get-Command signtool.exe -ErrorAction SilentlyContinue)) {
        Write-Host "PASS: signing was requested and SignTool is available."
    } else {
        Write-Warning "Signing was requested but SignTool was not found."
    }
} else {
    Write-Host "INFO: code signing is disabled for this verification run."
}
