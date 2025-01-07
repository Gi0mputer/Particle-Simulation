#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "Mode.h"

class SimulationManager {
public:
    static void getSimulationSize(SimulationResolution resolution, int& simWidth, int& simHeight);
};

#endif // SIMULATION_MANAGER_H
