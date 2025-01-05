#ifndef PARTICLE_H
#define PARTICLE_H

struct Particle {
    float x, y;  // Posizione
    float vx, vy; // Velocità

    Particle(float x, float y, float vx, float vy)
        : x(x), y(y), vx(vx), vy(vy) {}
};

#endif
