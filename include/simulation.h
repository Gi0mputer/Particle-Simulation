#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "Particle.h"

class Simulation {
public:
    Simulation(int count, int width, int height);
    void update(float deltaTime);

    const std::vector<Particle>& getParticles() const { return particles; }

private:
    std::vector<Particle> particles;
    int width, height;
};

#endif
