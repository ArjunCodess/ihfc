$ErrorActionPreference = 'Stop'
if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
    throw 'Install Arduino CLI and add it to PATH, or compile CareBotESP32.ino with Arduino IDE. See START-HERE.md.'
}
$buildPath = Join-Path ([System.IO.Path]::GetTempPath()) ('carebot-build-' + [guid]::NewGuid().ToString('N'))
& arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 --warnings all --build-path $buildPath (Join-Path $PSScriptRoot 'CareBotESP32')
if ($LASTEXITCODE -ne 0) { throw 'ESP32 compilation failed. Check that esp32:esp32 3.3.5 is installed.' }
Write-Output "Build succeeded. Output: $buildPath"
