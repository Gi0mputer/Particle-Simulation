#pragma once

#include <cmath>

struct Particle {
    float x, y;  // Posizione
    float angle, speed; // Angolo e velocità

    // Costruttore predefinito
    Particle() : x(0), y(0), angle(0), speed(0) {}

    Particle(float x, float y, float angle, float speed)
        : x(x), y(y), angle(angle), speed(speed) {}

    // Funzione per aggiornare la posizione in base all'angolo e alla velocità
    void updatePosition(float deltaTime) {
        x += speed * std::cos(angle) * deltaTime;
        y += speed * std::sin(angle) * deltaTime;
    }
};
