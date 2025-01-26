#pragma once

#include <vector>
#include <cmath>
// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GpuParticle {
    float x, y;  // Posizione
    float angle, speed; // Angolo e velocità
};

struct Particle { 
    float x, y;
    float angle;
    float speed;

    Particle() : x(0), y(0), angle(0), speed(0) {}

    Particle(float x, float y, float angle, float speed)
        : x(x), y(y), angle(angle), speed(speed) {}

    // Funzione per aggiornare la posizione in base all'angolo e alla velocità
    void updatePosition(float deltaTime) {
        x += speed * std::cos(angle) * deltaTime;
        y += speed * std::sin(angle) * deltaTime;
    }
};

class ISimulation
{
public:
    virtual ~ISimulation() = default;

    // Da chiamare una volta all'inizio, se devi allocare risorse, buffer, ecc.
    virtual void initialize() = 0;

    // Aggiorna la simulazione di un passo dt
    virtual void update(float dt) = 0;

    // Se vuoi rendere accessibile il vettore di particelle (CPU). 
    // Se la simulazione è su GPU, potrebbe restituire un vettore vuoto
    virtual const std::vector<Particle>& getParticles() const = 0;

    // (Facoltativo) Se serve sapere il numero di particelle
    virtual int getParticleCount() const = 0;
};
