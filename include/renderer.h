#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include "particle.h"

class Renderer {
public:
    Renderer(const std::vector<Particle>& particles);
    ~Renderer();

    void updateBuffer(const std::vector<Particle>& particles);
    void draw();

private:
    GLuint vbo, vao;
    size_t particleCount;
};

#endif
