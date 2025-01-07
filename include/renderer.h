#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include "particle.h"

class Renderer {
public:
    Renderer(const std::vector<Particle>& particles, int width, int height);
    ~Renderer();

    void updateBuffer(const std::vector<Particle>& particles);
    void drawParticles();

    void beginPingPong();
    void endPingPong();

    GLuint getCurrentTexture() const;
    GLuint getNextTexture() const;

    void swapPingPong();

private:
    void createPingPongTextures(int width, int height);
    void configureFramebuffer(GLuint& fbo, GLuint& texture, int width, int height);

    GLuint vbo, vao;
    GLuint pingFBO, pongFBO;
    GLuint pingTexture, pongTexture;
    size_t particleCount;
    bool usePing;

    int textureWidth, textureHeight;
};

#endif // RENDERER_H
