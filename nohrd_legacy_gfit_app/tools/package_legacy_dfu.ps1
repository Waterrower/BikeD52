param(
    [Parameter(Mandatory = $true)]
    [string]$ApplicationHex,

    [Parameter(Mandatory = $true)]
    [string]$KeyFile,

    [Parameter(Mandatory = $true)]
    [string]$OutputZip,

    [string]$HwVersion = "52",
    [string]$SdReq = "0x0094",
    [string]$ApplicationVersion = "40401",
    [string]$Nrfutil = "nrfutil"
)

$ErrorActionPreference = "Stop"

if (!(Test-Path -LiteralPath $ApplicationHex)) {
    throw "Application HEX not found: $ApplicationHex"
}

if (!(Test-Path -LiteralPath $KeyFile)) {
    throw "DFU key not found: $KeyFile"
}

& $Nrfutil pkg generate `
    --application $ApplicationHex `
    --application-version $ApplicationVersion `
    --hw-version $HwVersion `
    --sd-req $SdReq `
    --key-file $KeyFile `
    $OutputZip

Write-Host "Created $OutputZip"
