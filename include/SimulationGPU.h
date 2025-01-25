#pragma once
#include "ISimulation.h"
#include <vector>
#include "Particle.h"

class SimulationGPU : public ISimulation
{
public:
    SimulationGPU(int particleCount, int width, int height);

    void initialize() override;
    void update(float dt) override;
    const std::vector<Particle>& getParticles() const override;
    int getParticleCount() const override;

    // (In futuro) potresti aggiungere metodi per restituire
    // un buffer GPU, ID di un SSBO, ecc.

private:
    int m_particleCount;
    int m_width, m_height;

    // GPU resource placeholders
    unsigned int m_computeShaderID;
    unsigned int m_particleBuffer; // SSBO o VBO
    // ...
};