#include "renderer.h"
#include <glad/glad.h>

Renderer::Renderer(const std::vector<Particle>& particles) : particleCount(particles.size()) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::updateBuffer(const std::vector<Particle>& particles) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Particle), particles.data());
}

void Renderer::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particleCount);
}
