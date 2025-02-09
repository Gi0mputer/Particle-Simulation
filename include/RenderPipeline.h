#pragma once

#include <glad/glad.h>
#include "SimulationGPU.h"
#include "Shader.h"

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

    // VAO e VBO per il quad fullscreen
    GLuint m_quadVAO, m_quadVBO;

    // Shader per il pass finale
    Shader m_finalShader;

    // Uniform location
    GLint m_particles_uSimSizeLoc;
    GLint m_final_uTextureLoc;

private:
    void finalPass(const SimulationGPU& simulation);
    void setupQuadVAO();
    void renderQuad();
};
