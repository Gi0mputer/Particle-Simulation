# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\msys64\ucrt64\bin\cmake.exe

# The command to remove a file.
RM = C:\msys64\ucrt64\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build"

# Include any dependencies generated for this target.
include CMakeFiles/ParticleSimulation.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ParticleSimulation.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ParticleSimulation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ParticleSimulation.dir/flags.make

CMakeFiles/ParticleSimulation.dir/codegen:
.PHONY : CMakeFiles/ParticleSimulation.dir/codegen

CMakeFiles/ParticleSimulation.dir/src/glad.c.obj: CMakeFiles/ParticleSimulation.dir/flags.make
CMakeFiles/ParticleSimulation.dir/src/glad.c.obj: CMakeFiles/ParticleSimulation.dir/includes_C.rsp
CMakeFiles/ParticleSimulation.dir/src/glad.c.obj: C:/Users/giova/Progetti/Progetti\ VS\ code/Particle-Simulation/src/glad.c
CMakeFiles/ParticleSimulation.dir/src/glad.c.obj: CMakeFiles/ParticleSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ParticleSimulation.dir/src/glad.c.obj"
	C:\msys64\ucrt64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ParticleSimulation.dir/src/glad.c.obj -MF CMakeFiles\ParticleSimulation.dir\src\glad.c.obj.d -o CMakeFiles\ParticleSimulation.dir\src\glad.c.obj -c "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\glad.c"

CMakeFiles/ParticleSimulation.dir/src/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ParticleSimulation.dir/src/glad.c.i"
	C:\msys64\ucrt64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\glad.c" > CMakeFiles\ParticleSimulation.dir\src\glad.c.i

CMakeFiles/ParticleSimulation.dir/src/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ParticleSimulation.dir/src/glad.c.s"
	C:\msys64\ucrt64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\glad.c" -o CMakeFiles\ParticleSimulation.dir\src\glad.c.s

CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj: CMakeFiles/ParticleSimulation.dir/flags.make
CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj: CMakeFiles/ParticleSimulation.dir/includes_CXX.rsp
CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj: C:/Users/giova/Progetti/Progetti\ VS\ code/Particle-Simulation/src/main.cpp
CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj: CMakeFiles/ParticleSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj -MF CMakeFiles\ParticleSimulation.dir\src\main.cpp.obj.d -o CMakeFiles\ParticleSimulation.dir\src\main.cpp.obj -c "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\main.cpp"

CMakeFiles/ParticleSimulation.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ParticleSimulation.dir/src/main.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\main.cpp" > CMakeFiles\ParticleSimulation.dir\src\main.cpp.i

CMakeFiles/ParticleSimulation.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ParticleSimulation.dir/src/main.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\main.cpp" -o CMakeFiles\ParticleSimulation.dir\src\main.cpp.s

CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj: CMakeFiles/ParticleSimulation.dir/flags.make
CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj: CMakeFiles/ParticleSimulation.dir/includes_CXX.rsp
CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj: C:/Users/giova/Progetti/Progetti\ VS\ code/Particle-Simulation/src/particle.cpp
CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj: CMakeFiles/ParticleSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj -MF CMakeFiles\ParticleSimulation.dir\src\particle.cpp.obj.d -o CMakeFiles\ParticleSimulation.dir\src\particle.cpp.obj -c "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\particle.cpp"

CMakeFiles/ParticleSimulation.dir/src/particle.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ParticleSimulation.dir/src/particle.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\particle.cpp" > CMakeFiles\ParticleSimulation.dir\src\particle.cpp.i

CMakeFiles/ParticleSimulation.dir/src/particle.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ParticleSimulation.dir/src/particle.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\particle.cpp" -o CMakeFiles\ParticleSimulation.dir\src\particle.cpp.s

CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj: CMakeFiles/ParticleSimulation.dir/flags.make
CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj: CMakeFiles/ParticleSimulation.dir/includes_CXX.rsp
CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj: C:/Users/giova/Progetti/Progetti\ VS\ code/Particle-Simulation/src/renderer.cpp
CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj: CMakeFiles/ParticleSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj -MF CMakeFiles\ParticleSimulation.dir\src\renderer.cpp.obj.d -o CMakeFiles\ParticleSimulation.dir\src\renderer.cpp.obj -c "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\renderer.cpp"

CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\renderer.cpp" > CMakeFiles\ParticleSimulation.dir\src\renderer.cpp.i

CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\renderer.cpp" -o CMakeFiles\ParticleSimulation.dir\src\renderer.cpp.s

CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj: CMakeFiles/ParticleSimulation.dir/flags.make
CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj: CMakeFiles/ParticleSimulation.dir/includes_CXX.rsp
CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj: C:/Users/giova/Progetti/Progetti\ VS\ code/Particle-Simulation/src/simulation.cpp
CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj: CMakeFiles/ParticleSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj -MF CMakeFiles\ParticleSimulation.dir\src\simulation.cpp.obj.d -o CMakeFiles\ParticleSimulation.dir\src\simulation.cpp.obj -c "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\simulation.cpp"

CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\simulation.cpp" > CMakeFiles\ParticleSimulation.dir\src\simulation.cpp.i

CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\src\simulation.cpp" -o CMakeFiles\ParticleSimulation.dir\src\simulation.cpp.s

# Object files for target ParticleSimulation
ParticleSimulation_OBJECTS = \
"CMakeFiles/ParticleSimulation.dir/src/glad.c.obj" \
"CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj" \
"CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj" \
"CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj" \
"CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj"

# External object files for target ParticleSimulation
ParticleSimulation_EXTERNAL_OBJECTS =

ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/src/glad.c.obj
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/src/main.cpp.obj
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/src/particle.cpp.obj
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/src/renderer.cpp.obj
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/src/simulation.cpp.obj
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/build.make
ParticleSimulation.exe: C:/msys64/ucrt64/lib/libglfw3.dll.a
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/linkLibs.rsp
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/objects1.rsp
ParticleSimulation.exe: CMakeFiles/ParticleSimulation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable ParticleSimulation.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ParticleSimulation.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ParticleSimulation.dir/build: ParticleSimulation.exe
.PHONY : CMakeFiles/ParticleSimulation.dir/build

CMakeFiles/ParticleSimulation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ParticleSimulation.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ParticleSimulation.dir/clean

CMakeFiles/ParticleSimulation.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation" "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation" "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build" "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build" "C:\Users\giova\Progetti\Progetti VS code\Particle-Simulation\build\CMakeFiles\ParticleSimulation.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/ParticleSimulation.dir/depend
