#include "RenderPipeline.h"
#include <stdexcept>
#include <iostream>

RenderPipeline::RenderPipeline(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_outputWidth(width)
    , m_outputHeight(height)
    , m_quadVAO(0)
    , m_quadVBO(0)
    , m_final_uTextureLoc(-1)
{
}

RenderPipeline::~RenderPipeline()
{
    // Non abbiamo più ping/pong FBO/texture
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

void RenderPipeline::initialize()
{

    // Final (compositing)
    m_finalShader.load("shaders/final.vert", "shaders/final.frag");
    // Pre-cachiamo gli uniform
    m_final_uTextureLoc     = glGetUniformLocation(m_finalShader.getID(), "uTexture");
    // Creiamo solo il quad fullscreen
    setupQuadVAO();
}

void RenderPipeline::render(const SimulationGPU& simulation, int windowWidth, int windowHeight)
{
    m_outputWidth  = windowWidth;
    m_outputHeight = windowHeight;

    finalPass(simulation);
}

void RenderPipeline::finalPass(const SimulationGPU& simulation)
{
    // Disegna sul framebuffer di default (la finestra)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_outputWidth, m_outputHeight);

    m_finalShader.use();

    // Riceviamo dal simulation (compute shader) l'ID della texture su cui ha fatto blur e calcoli di color
    GLuint finalTex = simulation.getFinalTexture();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTex);

    if (m_final_uTextureLoc >= 0)
        glUniform1i(m_final_uTextureLoc, 0);

    renderQuad();
}

void RenderPipeline::setupQuadVAO()
{
    float quadVertices[] = {
        // pos      // texcoords
        -1.f, -1.f,  0.f, 0.f,
         1.f, -1.f,  1.f, 0.f,
         1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f,  0.f, 1.f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // attributo 0: pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

    // attributo 1: texcoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    glBindVertexArray(0);
}

void RenderPipeline::renderQuad()
{
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}
