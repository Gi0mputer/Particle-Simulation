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
    Write-Host "Detected GPUs (from Win32_VideoController):"
    $gpus = Get-CimInstance Win32_VideoController | Where-Object { $_.Name }
    foreach ($gpu in $gpus) {
        $vramGB = if ($gpu.AdapterRAM -gt 0) { "{0:N1} GB" -f ($gpu.AdapterRAM / 1GB) } else { "n/d" }
        Write-Host (" - {0} ({1})" -f $gpu.Name, $vramGB)
    }

    # Force high-performance GPU for this exe via per-app preference
    if ($gpus.Count -gt 0) {
        $preferred = $gpus | Sort-Object AdapterRAM -Descending | Select-Object -First 1
        $gpuPrefKey = "HKCU:\Software\Microsoft\DirectX\UserGpuPreferences"
        if (-not (Test-Path $gpuPrefKey)) { New-Item -Path $gpuPrefKey -Force | Out-Null }
        $desiredValue = "GpuPreference=2;"
        $currentValue = $null
        try { $currentValue = Get-ItemPropertyValue -Path $gpuPrefKey -Name $exe -ErrorAction Stop } catch { }
        if ($currentValue -ne $desiredValue) {
            New-ItemProperty -Path $gpuPrefKey -Name $exe -Value $desiredValue -PropertyType String -Force | Out-Null
            Write-Host ("Set GPU preference to HIGH PERFORMANCE for {0} (preferred: {1})" -f $exe, $preferred.Name)
        } else {
            Write-Host "GPU preference already set to HIGH PERFORMANCE for this exe."
        }
    }

    Write-Host "Running $exe..."
    & $exe 2>&1 | ForEach-Object { Write-Host $_ }
} else {
    Write-Error "Executable not found: $exe"
    exit 2
}
