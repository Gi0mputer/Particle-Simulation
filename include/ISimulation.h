#pragma once

#include <vector>
#include "Particle.h" 

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
