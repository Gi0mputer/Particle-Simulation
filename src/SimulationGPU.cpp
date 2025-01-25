
// SimulationGPU.cpp
#include "SimulationGPU.h"
#include <stdexcept>

SimulationGPU::SimulationGPU(int particleCount, int width, int height)
    : m_particleCount(particleCount)
    , m_width(width)
    , m_height(height)
    , m_computeShaderID(0)
    , m_particleBuffer(0)
{
}

void SimulationGPU::initialize()
{
    // Caricare/compilare compute shader,
    // creare SSBO, inizializzare dati in GPU
    // ...
    // Per ora, se non implementato, puoi lasciare vuoto o un throw:
    // throw std::runtime_error("SimulationGPU::initialize() not implemented yet");
}

void SimulationGPU::update(float dt)
{
    // Lanciare compute shader con glDispatchCompute
    // passare dt come uniform
    // ...
    // throw std::runtime_error("SimulationGPU::update() not implemented yet");
}

const std::vector<Particle>& SimulationGPU::getParticles() const
{
    // Se non vuoi ritornare i dati su CPU,
    // potresti restituire un vettore vuoto
    static std::vector<Particle> dummy;
    return dummy;
}

int SimulationGPU::getParticleCount() const
{
    return m_particleCount;
}
