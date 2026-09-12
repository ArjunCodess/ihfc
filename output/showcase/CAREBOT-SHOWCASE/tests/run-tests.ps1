$ErrorActionPreference = 'Stop'
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw 'These host simulations need g++ with C++17 support. They are not needed to upload with Arduino IDE.'
}
$testExe = Join-Path ([System.IO.Path]::GetTempPath()) ('carebot-tests-' + [guid]::NewGuid().ToString('N') + '.exe')
& g++ -std=c++17 -Wall -Wextra -Werror -I $PSScriptRoot (Join-Path $PSScriptRoot 'test.cpp') -o $testExe
if ($LASTEXITCODE -ne 0) { throw 'Host test compilation failed.' }
& $testExe
if ($LASTEXITCODE -ne 0) { throw 'Host tests failed.' }
Write-Output "Test executable: $testExe"
