#include "Simulation.h"
#include <cstdlib>

Simulation::Simulation(int count, int width, int height) : width(width), height(height) {
    for (int i = 0; i < count; ++i) {
        particles.emplace_back(
            static_cast<float>(rand() % width),
            static_cast<float>(rand() % height),
            static_cast<float>((rand() % 100) / 50.0f - 1.0f),
            static_cast<float>((rand() % 100) / 50.0f - 1.0f)
        );
    }
}

void Simulation::update(float deltaTime) {
    for (auto& p : particles) {
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;

        if (p.x <= 0 || p.x >= width) p.vx = -p.vx;
        if (p.y <= 0 || p.y >= height) p.vy = -p.vy;
    }
}
