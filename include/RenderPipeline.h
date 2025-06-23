#pragma once

#include <glad/glad.h>
#include "SimulationGPU.h"
#include "Shader.h"
#include <memory>

class RenderPipeline
{
public:
    RenderPipeline(int width, int height);
    ~RenderPipeline();

    void initialize();
    void render(const SimulationGPU& simulation, int windowWidth, int windowHeight);

private:
    int m_width, m_height;
    int m_outputWidth, m_outputHeight;

    // VAO, VBO ed EBO per il quad fullscreen
    GLuint m_quadVAO, m_quadVBO, m_quadEBO;

    // Shader per il pass finale
    std::unique_ptr<Shader> m_finalShader;

    // Uniform location
    GLint m_particles_uSimSizeLoc;
    GLint m_final_uTextureLoc;

private:
    void finalPass(const SimulationGPU& simulation);
    void createFullscreenQuad();
    void render(GLuint textureID);
};
