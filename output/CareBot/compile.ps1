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
$libraryPath = Join-Path $env:USERPROFILE 'Documents/Arduino/libraries'
if (-not (Test-Path -LiteralPath (Join-Path $libraryPath 'Adafruit_PWM_Servo_Driver_Library'))) {
    throw 'Install Adafruit PWM Servo Driver Library and Adafruit BusIO in Documents/Arduino/libraries.'
}
& $cli compile --fqbn esp32:esp32:esp32 --libraries $libraryPath --warnings all --build-path $buildPath (Join-Path $PSScriptRoot 'CareBotESP32')
if ($LASTEXITCODE -ne 0) { throw 'ESP32 compilation failed. Check the compiler output above.' }
Write-Output "Build succeeded. Output: $buildPath"
