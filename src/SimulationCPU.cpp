
// --------------------------------------------------------------------------
// SimulationCPU.cpp
#include "SimulationCPU.h"
#include <cstdlib> // per rand() se vuoi generare posizioni random
#include <cmath>
#include <random>
#include <algorithm>
#include "Particle.h"

// Generatore di numeri casuali (puoi tenerlo globale/statistico, o all'interno della classe)
static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> angleJitterDist(-0.1f, 0.1f);

SimulationCPU::SimulationCPU(int particleCount, int width, int height)
    : m_width(width)
    , m_height(height)
{
    // Pre-alloca spazio per le particelle
    m_particles.resize(particleCount);
}

void SimulationCPU::initialize()
{
    // Inizializza le particelle (posizione casuale, velocità casuale, ecc.)
    for (auto& p : m_particles) 
    {
        float x = static_cast<float>(rand() % m_width);
        float y = static_cast<float>(rand() % m_height);
        float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI; // Angolo casuale tra 0 e 2π
        float speed = static_cast<float>(rand()) / RAND_MAX * 10  + 10; // Velocità casuale tra 0 e 100
        m_particles.emplace_back(x, y, angle, speed);
    }
}



void SimulationCPU::update(float dt)
{
    for (auto& p : m_particles) {
        // Aggiunge un piccolo offset casuale all'angolo,
        // così le particelle “curvano” piano piano.
        float angleOffset = angleJitterDist(rng);  // valore in [-0.1, 0.1]
        p.angle += angleOffset;

        // Aggiorna la posizione in base a (angle, speed)
        p.updatePosition(dt);

        // Rimbalzo ai bordi
        if (p.x < 0.0f || p.x > m_width) {
            p.angle = M_PI - p.angle; 
            p.x = std::clamp(p.x, 0.0f, static_cast<float>(m_width));
        }
        if (p.y < 0.0f || p.y > m_height) {
            p.angle = -p.angle; 
            p.y = std::clamp(p.y, 0.0f, static_cast<float>(m_height));
        }
    }
}

const std::vector<Particle>& SimulationCPU::getParticles() const
{
    return m_particles;
}

int SimulationCPU::getParticleCount() const
{
    return static_cast<int>(m_particles.size());
}
