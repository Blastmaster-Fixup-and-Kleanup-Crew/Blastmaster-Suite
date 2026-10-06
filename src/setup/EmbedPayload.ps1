param(
    [Parameter(Mandatory = $true)]
    [string]$SetupExe,

    [Parameter(Mandatory = $true)]
    [string]$PayloadRoot
)

$ErrorActionPreference = "Stop"

$magic = [System.Text.Encoding]::ASCII.GetBytes("BMSPAY01")

$tempExe = "$SetupExe.payload.tmp"

try {
    Copy-Item -LiteralPath $SetupExe -Destination $tempExe -Force

    $output = [System.IO.File]::Open(
        $tempExe,
        [System.IO.FileMode]::Append,
        [System.IO.FileAccess]::Write,
        [System.IO.FileShare]::Read)

    $writer = New-Object System.IO.BinaryWriter($output)

    $payloadOffset = $output.Position

    $files = Get-ChildItem -LiteralPath $PayloadRoot -File -Recurse

    foreach ($file in $files) {
        $relative = $file.FullName.Substring(
            $PayloadRoot.TrimEnd('\').Length + 1)

        $pathBytes = [System.Text.Encoding]::UTF8.GetBytes(
            $relative.Replace('\', '/'))

        $data = [System.IO.File]::ReadAllBytes(
            $file.FullName)

        $writer.Write([UInt32]$pathBytes.Length)
        $writer.Write([UInt64]$data.Length)
        $writer.Write([UInt64]$data.Length)
        $writer.Write($pathBytes)
        $writer.Write($data)
    }

    # Footer is written after the entries. Its first 8 bytes are
    # the format marker and the next 8 bytes are the payload offset.
    $writer.Write($magic)
    $writer.Write([UInt64]$payloadOffset)

    $writer.Flush()
    $writer.Dispose()
    $output.Dispose()

    Move-Item -LiteralPath $tempExe -Destination $SetupExe -Force
}
catch {
    if (Test-Path -LiteralPath $tempExe) {
        Remove-Item -LiteralPath $tempExe -Force
    }

    throw
}
finally {
    if (Test-Path -LiteralPath $PayloadRoot) {
        Remove-Item -LiteralPath $PayloadRoot -Recurse -Force
    }
}
