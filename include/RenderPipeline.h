#pragma once
#include <vector>
#include <string>

// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Header delle classi create
#include "SimulationCPU.h"
#include "ShaderProgram.h"

class RenderPipeline
{
public:
    // Costruttore: specifica le dimensioni desiderate per la pipeline
    RenderPipeline(int width, int height);

    // Distruttore
    ~RenderPipeline();

    // Inizializza le risorse (FBO, texture, VAO, shader, ecc.)
    void initialize();

    // Esegue la pipeline di rendering sulla base dei dati della simulazione
    void render(const ISimulation& simulation, int windowWidth, int windowHeight);

private:
    // Metodi di "Pass"
    void drawParticlesPass(const ISimulation& simulation);
    void trailPass();
    // Pass finale: disegna su schermo (framebuffer 0)
    void finalPass();

    // Metodi interni di supporto
    void createFBOAndTexture(GLuint &fbo, GLuint &texture);
    void setupQuadVAO();
    void renderQuad();

    void beginPingPong(GLuint fbo);
    void endPingPong();
    void swapPingPong();

private:
    // Dati e dimensioni
    int m_width;
    int m_height;
    int m_outputWidth;
    int m_outputHeight;
    bool m_usePing; // Per sapere quale texture è "in uso" e quale è "quella successiva"

    // FBO e Texture: ping e pong
    GLuint m_pingFBO;
    GLuint m_pingTexture;

    GLuint m_pongFBO;
    GLuint m_pongTexture;

    // VAO/VBO per un quad fullscreen
    GLuint m_quadVAO;
    GLuint m_quadVBO;

    // VAO/VBO per le particelle (se vuoi disegnare particelle come GL_POINTS o brush)
    GLuint m_particlesVAO;
    GLuint m_particlesVBO;

    // ShaderProgram
    ShaderProgram m_trailShader;
    ShaderProgram m_finalShader;
    ShaderProgram m_brushShader;
};
