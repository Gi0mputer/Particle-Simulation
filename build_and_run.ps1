# build_and_run.ps1 - Configure, build, copy DLLs and run executable (PowerShell)
$ErrorActionPreference = 'Stop'
$cmake = "C:\tools\cmake-3.30.0-windows-x86_64\bin\cmake.exe"
$repo = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $repo
Write-Host "Using CMake: $cmake"
if (-not (Test-Path $cmake)) {
    Write-Error "cmake.exe not found at $cmake"
    exit 1
}
# Clean previous build
if (Test-Path build) { Remove-Item -Recurse -Force build }
Write-Host "Configuring..."
& $cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$repo\vcpkg_installed\x64-windows"
Write-Host "Building Release..."
& $cmake --build build --config Release
# Copy runtime DLLs
$vcpkgBin = Join-Path $repo "vcpkg_installed\x64-windows\bin"
$exeDir = Join-Path $repo "build\bin\Release"
if (Test-Path $vcpkgBin) {
    Write-Host "Copying DLLs from $vcpkgBin to $exeDir"
    if (-not (Test-Path $exeDir)) { New-Item -ItemType Directory -Path $exeDir | Out-Null }
    Get-ChildItem -Path $vcpkgBin -Filter *.dll -ErrorAction SilentlyContinue | ForEach-Object { Copy-Item $_.FullName -Destination $exeDir -Force }
} else {
    Write-Host "vcpkg bin not found: $vcpkgBin"
}
# Run exe
$exe = Join-Path $exeDir "ParticleSimulation.exe"
if (Test-Path $exe) {
    Write-Host "Running $exe..."
    & $exe 2>&1 | ForEach-Object { Write-Host $_ }
} else {
    Write-Error "Executable not found: $exe"
    exit 2
}
