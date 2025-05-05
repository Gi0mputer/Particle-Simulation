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
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

void RenderPipeline::initialize()
{
    std::cout << "[RenderPipeline] Inizializzazione..." << std::endl;
    
    std::cout << "[RenderPipeline] Creazione shader..." << std::endl;
    m_finalShader = std::make_unique<Shader>("shaders/final.vert", "shaders/final.frag");
    
    std::cout << "[RenderPipeline] Creazione quad..." << std::endl;
    createFullscreenQuad();
    
    std::cout << "[RenderPipeline] Inizializzazione completata" << std::endl;
}

void RenderPipeline::createFullscreenQuad()
{
    std::cout << "[RenderPipeline] Creazione fullscreen quad..." << std::endl;
    
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glGenBuffers(1, &m_quadEBO);
    
    glBindVertexArray(m_quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    std::cout << "[RenderPipeline] Fullscreen quad creato con successo" << std::endl;
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

    m_finalShader->use();

    // Riceviamo dal simulation (compute shader) l'ID della texture su cui ha fatto blur e calcoli di color
    GLuint finalTex = simulation.getFinalTexture();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTex);

    if (m_final_uTextureLoc >= 0)
        glUniform1i(m_final_uTextureLoc, 0);

    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RenderPipeline::render(GLuint textureID)
{
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 60 == 0) {
        std::cout << "[RenderPipeline] Frame " << frameCount << std::endl;
    }
    
    m_finalShader->use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    m_finalShader->setInt("u_texture", 0);
    
    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
