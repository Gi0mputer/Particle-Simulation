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

    // Post-Processing Settings
    void setColorMode(int mode) { m_colorMode = mode; }
    void setTime(float t) { m_time = t; }
    void setColors(float* c1, float* c2) { 
        m_color1[0]=c1[0]; m_color1[1]=c1[1]; m_color1[2]=c1[2];
        m_color2[0]=c2[0]; m_color2[1]=c2[1]; m_color2[2]=c2[2];
    }
    void setNeonParams(float speed, float range) { m_neonSpeed = speed; m_neonRange = range; }

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
    
    // Post-Process Params
    int m_colorMode = 0;
    float m_time = 0.0f;
    float m_color1[3] = {0, 1, 1};
    float m_color2[3] = {1, 0, 1};
    float m_neonSpeed = 1.0f;
    float m_neonRange = 1.0f;

private:
    void finalPass(const SimulationGPU& simulation);
    void setupQuadVAO();
    void renderQuad();
};
