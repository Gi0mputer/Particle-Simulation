# build_release_helper.ps1
# Builds Release and copies vcpkg DLLs to build output
$ErrorActionPreference = 'Stop'
$repo = Convert-Path "$PSScriptRoot\.."
$cmake = "C:\tools\cmake-3.30.0-windows-x86_64\bin\cmake.exe"

if (-not (Test-Path $cmake)) { Write-Error "cmake.exe not found at $cmake"; exit 1 }

Set-Location $repo

# check if configured
if (-not (Test-Path "build\CMakeCache.txt")) {
    Write-Host "Configuration missing. Configuring CMake..."
    & $cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$repo\vcpkg_installed\x64-windows"
    if ($LASTEXITCODE -ne 0) { Write-Error "CMake configuration failed"; exit 1 }
}

Write-Host "Building Release..."
& $cmake --build build --config Release
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

$vcpkgBin = Join-Path $repo "vcpkg_installed\x64-windows\bin"
$exeDir = Join-Path $repo "build\bin\Release"
if (Test-Path $vcpkgBin) {
    if (-not (Test-Path $exeDir)) { New-Item -ItemType Directory -Path $exeDir | Out-Null }
    Get-ChildItem -Path $vcpkgBin -Filter *.dll -ErrorAction SilentlyContinue | ForEach-Object { Copy-Item $_.FullName -Destination $exeDir -Force }
}
