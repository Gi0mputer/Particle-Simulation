#include "RenderPipeline.h"

#include <iostream>
#include <stdexcept>

// Costruttore
RenderPipeline::RenderPipeline(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_outputWidth(width)     // per default, se non settiamo diversamente
    , m_outputHeight(height) 
    , m_usePing(true)
    , m_pingFBO(0)
    , m_pingTexture(0)
    , m_pongFBO(0)
    , m_pongTexture(0)
    , m_quadVAO(0)
    , m_quadVBO(0)
    , m_particlesVAO(0)
    , m_particlesVBO(0)
    , m_trailShader()
    , m_finalShader()
{
}

// Distruttore: pulizia delle risorse
RenderPipeline::~RenderPipeline()
{
    // Distruggi FBO e Texture
    glDeleteFramebuffers(1, &m_pingFBO);
    glDeleteTextures(1, &m_pingTexture);

    glDeleteFramebuffers(1, &m_pongFBO);
    glDeleteTextures(1, &m_pongTexture);

    // Distruggi VAO/VBO quad
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);

    // Distruggi VAO/VBO particelle
    glDeleteVertexArrays(1, &m_particlesVAO);
    glDeleteBuffers(1, &m_particlesVBO);

    // Gli ShaderProgram si distruggono automaticamente se implementano un destructor
    // o se semplicemente i loro oggetti vanno fuori scope. 
}

// Inizializza la pipeline
void RenderPipeline::initialize()
{
    // 1) Carica e compila gli shader
    //    (path ipotetici: vedi tu se usarli dal disco, incollarli come stringhe, etc.)
    m_trailShader.load("shaders/trail.vert", "shaders/trail.frag");
    m_finalShader.load("shaders/final.vert", "shaders/final.frag");
    m_brushShader.load("shaders/brush.vert", "shaders/brush.frag");

    // 2) Crea i due FBO + texture per ping-pong
    createFBOAndTexture(m_pingFBO, m_pingTexture);
    createFBOAndTexture(m_pongFBO, m_pongTexture);

    // 3) Crea il VAO/VBO per il quad fullscreen
    setupQuadVAO();

    // 4) Crea il VAO/VBO per le particelle (opzionale, se vuoi disegnarle come brush pass)
    glGenBuffers(1, &m_particlesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_particlesVBO);
    // Non sappiamo ancora la dimensione, la settiamo dinamicamente nel metodo `drawParticlesPass()`
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &m_particlesVAO);
    glBindVertexArray(m_particlesVAO);
    glEnableVertexAttribArray(0);
    // Se la struttura di Particle è (x, y), allora:
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glBindVertexArray(0);

    // 5) Altre impostazioni generiche (se servono)
    // es. glEnable(GL_POINT_SPRITE), se stai usando particelle come GL_POINTS.
}

// rendering principale
void RenderPipeline::render(const ISimulation& simulation, int windowWidth, int windowHeight)
{
    m_outputWidth = windowWidth;
    m_outputHeight = windowHeight;

    // 2) PRIMO PASS: Disegna le particelle su uno dei due FBO (ping o pong),
    //    depositandole e/o combinandole con la texture esistente.
    //    Per semplicità, supponiamo che "drawParticlesPass" scriva nella "current" texture
    drawParticlesPass(simulation);

    // 3) SECONDO PASS: Esempio di pass di "trail", in cui mescoli la texture attuale con quella storica
    //    (lettura da "prev" texture, scrittura in "next"). Potresti avere più pass (blur, post-proc, ecc.).
    trailPass();

    // 4) PASS FINALE: Disegna sul framebuffer di default la texture finale (pong o ping a seconda di come hai fatto lo swap).
    finalPass();
}
void RenderPipeline::drawParticlesPass(const ISimulation& simulation)
{
    // (Facoltativo) se la simulazione è CPU-based, carichiamo i dati su m_particlesVBO
    // supponiamo che Particle contenga (x,y) come primi due float
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_particlesVBO);
        const auto& particles = simulation.getParticles();
        glBufferData(GL_ARRAY_BUFFER,
                     particles.size() * sizeof(Particle), 
                     particles.data(),
                     GL_DYNAMIC_DRAW);
    }

    // Scegli FBO (ping o pong)
    GLuint currentFBO = m_usePing ? m_pingFBO : m_pongFBO;
    beginPingPong(currentFBO);

    // Clear (nero)
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Usa brushShader
    m_brushShader.use();

    // Se in brush.vert hai uniform vec2 uSimSize
    GLint loc = glGetUniformLocation(m_brushShader.getID(), "uSimSize");
    if(loc >= 0) {
        glUniform2f(loc, (float)m_width, (float)m_height);
    }

    // Disegna i punti
    glBindVertexArray(m_particlesVAO);
    const auto& particles = simulation.getParticles();
    glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());

    endPingPong();
}

// --------------------------------------------------------------------------
// PASS 2: Scia / trail
// --------------------------------------------------------------------------
void RenderPipeline::trailPass()
{
    GLuint readTex  = m_usePing ? m_pingTexture : m_pongTexture;
    GLuint writeFBO = m_usePing ? m_pongFBO : m_pingFBO;

    beginPingPong(writeFBO);
    //glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    m_trailShader.use();

    // Esempio: uniform float uDecay
    // glUniform1f(glGetUniformLocation(m_trailShader.getID(), "uDecay"), 0.98f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, readTex);
    glUniform1i(glGetUniformLocation(m_trailShader.getID(), "uTexture"), 0);

    // Disegno su un quad fullscreen
    renderQuad();

    //endPingPong();

    // Swap ping-pong
    //swapPingPong();
}

// --------------------------------------------------------------------------
// PASS 3: Disegna la texture finale sullo schermo
// --------------------------------------------------------------------------
void RenderPipeline::finalPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_outputWidth, m_outputHeight);

    // Pulizia schermo
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_finalShader.use();

    GLuint finalTex = m_usePing ? m_pingTexture : m_pongTexture;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTex);
    glUniform1i(glGetUniformLocation(m_finalShader.getID(), "uTexture"), 0);

    renderQuad();
}

// --------------------------------------------------------------------------
// Supporto: creazione di un FBO e texture associata
// --------------------------------------------------------------------------
void RenderPipeline::createFBOAndTexture(GLuint &fbo, GLuint &texture)
{
    // Crea texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Formato RGBA32F per avere abbastanza precisione, va bene anche RGBA8
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Crea FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Attacca la texture come color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Verifica stato FBO
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Error creating framebuffer");
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// --------------------------------------------------------------------------
// Supporto: setup del VAO/VBO per disegnare un quad fullscreen
// --------------------------------------------------------------------------
void RenderPipeline::setupQuadVAO()
{
    float quadVertices[] = {
        // pos      // tex
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

    // Layout
    //  - attribute 0: posizione (vec2)
    //  - attribute 1: texcoord  (vec2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

// --------------------------------------------------------------------------
// Supporto: disegna un quad fullscreen
// --------------------------------------------------------------------------
void RenderPipeline::renderQuad()
{
    glBindVertexArray(m_quadVAO);
    // Possiamo usare GL_TRIANGLE_FAN o GL_TRIANGLE_STRIP.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

// --------------------------------------------------------------------------
// Supporto: binding FBO e settaggio viewport
// --------------------------------------------------------------------------
void RenderPipeline::beginPingPong(GLuint fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, m_width, m_height);
}

// --------------------------------------------------------------------------
void RenderPipeline::endPingPong()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// --------------------------------------------------------------------------
// Scambia i ruoli tra ping e pong
// --------------------------------------------------------------------------
void RenderPipeline::swapPingPong()
{
    m_usePing = !m_usePing;
}
