$ErrorActionPreference = "Stop"

if (-not $env:VITASDK) {
    throw "VITASDK is not set. Install VitaSDK first, then set VITASDK."
}

$env:Path = "$env:VITASDK\bin;$env:Path"

& "$env:VITASDK\bin\vdpm.exe" install libvita2d freetype jansson curl openssl libpng libjpeg-turbo zlib
if ($LASTEXITCODE -ne 0) { throw "vdpm dependency installation failed" }

if (Test-Path build) { Remove-Item build -Recurse -Force }

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }

cmake --build build --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

Write-Host ""
Write-Host "SUCCESS: build\VitaFlightRadar.vpk"
