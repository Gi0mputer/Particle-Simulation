// SimulationCPU.h
#pragma once
#include "ISimulation.h"
#include <vector>
#include <algorithm>  // std::clamp
#include "Particle.h"
#define M_PI 3.14159265358979323846

class SimulationCPU : public ISimulation
{
public:
    SimulationCPU(int particleCount, int width, int height);

    void initialize() override;
    void update(float dt) override;
    const std::vector<Particle>& getParticles() const override;
    int getParticleCount() const override;

private:
    std::vector<Particle> m_particles;
    int m_width, m_height;
};