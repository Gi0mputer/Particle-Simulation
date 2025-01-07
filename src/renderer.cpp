#include "renderer.h"
#include <stdexcept>
#include <iostream>

Renderer::Renderer(const std::vector<Particle>& particles, int width, int height)
    : particleCount(particles.size()), usePing(true), textureWidth(width), textureHeight(height) {
    // Crea il VBO e VAO per le particelle
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);

    // Crea le texture e i framebuffer per il ping-pong
    createPingPongTextures(width, height);
}

Renderer::~Renderer() {
    // Elimina VAO, VBO e le risorse OpenGL
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glDeleteFramebuffers(1, &pingFBO);
    glDeleteFramebuffers(1, &pongFBO);
    glDeleteTextures(1, &pingTexture);
    glDeleteTextures(1, &pongTexture);
}

void Renderer::createPingPongTextures(int width, int height) {
    configureFramebuffer(pingFBO, pingTexture, width, height);
    configureFramebuffer(pongFBO, pongTexture, width, height);
}

void Renderer::configureFramebuffer(GLuint& fbo, GLuint& texture, int width, int height) {
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Errore nella creazione del framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::updateBuffer(const std::vector<Particle>& particles) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Particle), particles.data());
}

void Renderer::drawParticles() {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particleCount);
}

GLuint Renderer::getCurrentTexture() const {
    return usePing ? pingTexture : pongTexture;
}

GLuint Renderer::getNextTexture() const {
    return usePing ? pongTexture : pingTexture;
}

void Renderer::beginPingPong() {
    GLuint currentFBO = usePing ? pingFBO : pongFBO;
    glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
    glViewport(0, 0, textureWidth, textureHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endPingPong() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::swapPingPong() {
    usePing = !usePing;
}
