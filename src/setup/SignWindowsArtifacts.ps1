param(
    [string]$PayloadRoot,
    [string]$SetupExe
)

$ErrorActionPreference = "Stop"

$signtool = $env:BLASTMASTER_SIGNTOOL
if ([string]::IsNullOrWhiteSpace($signtool)) {
    $signtool = (Get-Command signtool.exe -ErrorAction SilentlyContinue).Source
}
if ([string]::IsNullOrWhiteSpace($signtool)) {
    throw "SignTool was not found. Set BLASTMASTER_SIGNTOOL or add signtool.exe to PATH."
}

$certificate = $env:BLASTMASTER_SIGN_CERTIFICATE
$password = $env:BLASTMASTER_SIGN_PASSWORD
$timestamp = $env:BLASTMASTER_SIGN_TIMESTAMP
if ([string]::IsNullOrWhiteSpace($timestamp)) {
    $timestamp = "https://timestamp.digicert.com"
}

function Sign-Artifact([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Signing target does not exist: $Path"
    }

    $args = @("sign", "/fd", "SHA256", "/td", "SHA256", "/tr", $timestamp)
    if (-not [string]::IsNullOrWhiteSpace($certificate)) {
        $args += @("/f", $certificate)
        if (-not [string]::IsNullOrWhiteSpace($password)) {
            $args += @("/p", $password)
        }
    } else {
        $args += @("/a")
    }
    $args += $Path

    & $signtool @args
    if ($LASTEXITCODE -ne 0) {
        throw "SignTool failed for $Path with exit code $LASTEXITCODE."
    }
}

if (-not [string]::IsNullOrWhiteSpace($PayloadRoot)) {
    if (-not (Test-Path -LiteralPath $PayloadRoot -PathType Container)) {
        throw "Payload root does not exist: $PayloadRoot"
    }

    Get-ChildItem -LiteralPath $PayloadRoot -Recurse -File |
        Where-Object { $_.Extension -ieq ".exe" -or $_.Extension -ieq ".dll" } |
        ForEach-Object { Sign-Artifact $_.FullName }
}

if (-not [string]::IsNullOrWhiteSpace($SetupExe)) {
    Sign-Artifact $SetupExe
}
