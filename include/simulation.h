#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "particle.h"

class Simulation {
public:
    // Costruttore: dimensione della simulazione fissa
    Simulation(int count, int fixedWidth, int fixedHeight);

    // Aggiorna lo stato della simulazione in base al tempo passato
    void update(float deltaTime);

    // Ritorna la lista di particelle
    const std::vector<Particle>& getParticles() const { return particles; }

    // Ritorna le dimensioni della simulazione (per uso nel rendering)
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::vector<Particle> particles; // Elenco delle particelle nella simulazione
    int width, height;               // Dimensioni fisse dello spazio della simulazione
};

#endif // SIMULATION_H
