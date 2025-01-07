#include "simulationmanager.h"
#include <GLFW/glfw3.h>

void SimulationManager::getSimulationSize(SimulationResolution resolution, int& simWidth, int& simHeight) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    switch (resolution) {
        case SimulationResolution::NATIVE:
            simWidth = mode->width;
            simHeight = mode->height;
            break;
        case SimulationResolution::HD:
            simWidth = 1920;
            simHeight = 1080;
            break;
        case SimulationResolution::SD:
            simWidth = 1280;
            simHeight = 720;
            break;
        default:
            simWidth = 1920;
            simHeight = 1080;
            break;
    }
}
