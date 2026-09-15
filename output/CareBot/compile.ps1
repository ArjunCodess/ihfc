$ErrorActionPreference = 'Stop'
$cli = (Get-Command arduino-cli -ErrorAction SilentlyContinue).Source
if (-not $cli) {
    $bundledCli = Join-Path $env:LOCALAPPDATA 'Programs/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe'
    if (Test-Path -LiteralPath $bundledCli) { $cli = $bundledCli }
}
if (-not $cli) {
    throw 'Install Arduino CLI or Arduino IDE, or compile CareBotESP32.ino in Arduino IDE. See START-HERE.md.'
}
$buildPath = Join-Path ([System.IO.Path]::GetTempPath()) ('carebot-build-' + [guid]::NewGuid().ToString('N'))
& $cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 --warnings all --build-path $buildPath (Join-Path $PSScriptRoot 'CareBotESP32')
if ($LASTEXITCODE -ne 0) { throw 'ESP32 compilation failed. Check that esp32:esp32 3.3.5 is installed.' }
Write-Output "Build succeeded. Output: $buildPath"
