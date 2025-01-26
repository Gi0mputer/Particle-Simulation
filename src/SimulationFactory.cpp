
#include "SimulationFactory.h"
#include "SimulationCPU.h"
#include "SimulationGPU.h"
#include <stdexcept>

namespace SimulationFactory
{
    ISimulation* createSimulation(SimulationType type, int numParticles, int width, int height)
    {
        switch (type)
        {
        case SimulationType::CPU:
            return new SimulationCPU(numParticles, width, height);
        case SimulationType::GPU:
            return new SimulationGPU(numParticles, width, height);
        default:
            throw std::runtime_error("Unknown SimulationType");
        }
    }
}
