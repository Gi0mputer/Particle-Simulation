#pragma once
#include "ISimulation.h"

enum class SimulationType {
    CPU,
    GPU
};

namespace SimulationFactory
{
    ISimulation* createSimulation(SimulationType type, int numParticles, int width, int height);
}
