# Particle Simulation

A GPU-accelerated particle simulation using OpenGL and GLFW.

## Prerequisites

- CMake 3.10 or higher
- Visual Studio 2022 Build Tools (or Visual Studio 2022)
- vcpkg (for dependency management)

## Building the Project

### First-time Setup

1. **Install vcpkg** (if not already installed):
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   ```

2. **Configure the project**:
   ```powershell
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```

3. **Build the project**:
   ```powershell
   cmake --build build --config Release
   ```

### Running the Application

After building, the executable will be located at:
```
build/bin/Release/ParticleSimulation.exe
```

Run it from the command line:
```powershell
.\build\bin\Release\ParticleSimulation.exe
```

## Dependencies

The project uses the following libraries (managed via vcpkg):
- **GLFW3** - Window and input handling
- **OpenGL** - Graphics rendering
- **GLAD** - OpenGL function loader (included in source)

## Project Structure

```
Particle-Simulation/
├── src/              # Source files (.cpp)
├── include/          # Header files (.h)
├── shaders/          # GLSL shader files
├── build/            # Build output directory
├── vcpkg.json        # vcpkg dependency manifest
└── CMakeLists.txt    # CMake configuration
```

## Troubleshooting

### CMake can't find GLFW3

Make sure you're using the vcpkg toolchain file:
```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### Clean rebuild

If you encounter build issues, try cleaning the build directory:
```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
```
