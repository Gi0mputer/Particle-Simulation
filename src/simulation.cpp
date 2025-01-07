#include "simulation.h"
#include <cstdlib>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <algorithm> // Per std::clamp

Simulation::Simulation(int count, int width, int height) 
    : width(width), height(height) 
{
     // Crea le particelle con posizioni e velocità iniziali casuali
    for (int i = 0; i < count; ++i) {
        float x = static_cast<float>(rand() % width);
        float y = static_cast<float>(rand() % height);
        float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI; // Angolo casuale tra 0 e 2π
        float speed = static_cast<float>(rand()) / RAND_MAX * 50  + 50; // Velocità casuale tra 0 e 100
        particles.emplace_back(x, y, angle, speed);
    }
}

void Simulation::update(float deltaTime) 
{
    // Aggiorna la posizione delle particelle in base all'angolo e alla velocità
    for (auto& p : particles) {
        p.updatePosition(deltaTime);

        // Rimbalzo ai bordi della simulazione
        if (p.x <= 0 || p.x >= width) {
            p.angle = M_PI - p.angle; // Inverti la direzione orizzontale
            p.x = std::clamp(p.x, 0.0f, static_cast<float>(width)); // Mantieni la particella all'interno dei bordi
        }
        if (p.y <= 0 || p.y >= height) {
            p.angle = -p.angle; // Inverti la direzione verticale
            p.y = std::clamp(p.y, 0.0f, static_cast<float>(height)); // Mantieni la particella all'interno dei bordi
        }
    }
}
