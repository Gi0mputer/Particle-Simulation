#pragma once

#include <glad/glad.h>

// Struttura GpuParticle
struct GpuParticle {
    float position[2];
    float angle;
    float speed;
};

class SimulationGPU
{
public:
    SimulationGPU(int particleCount, int width, int height);
    ~SimulationGPU();

    void initialize();
    void update(float dt, float mouseX, float mouseY, bool mousePressed, int mouseMode);

    // Accesso al buffer delle particelle (per eventuale debug drawing)
    GLuint getParticleBuffer() const { return m_particleBuffers[m_currentBuffer]; }
    int    getParticleCount() const  { return m_particleCount; }

    // Restituisce la texture finale (dopo l'ultimo pass). 
    GLuint getFinalTexture() const { return m_textureIDIn; }

    // Parameter Accessors
    float getSensorDistance() const { return m_sensorDistance; }
    void setSensorDistance(float val) { m_sensorDistance = val; }
    
    float getSensorAngle() const { return m_sensorAngle; }
    void setSensorAngle(float val) { m_sensorAngle = val; }
    
    float getTurnAngle() const { return m_turnAngle; }
    void setTurnAngle(float val) { m_turnAngle = val; }
    
    float getSpeed() const { return m_speed; }
    void setSpeed(float val) { m_speed = val; }
    
    float getRandomWeight() const { return m_randomWeight; }
    void setRandomWeight(float val) { m_randomWeight = val; }

private:
    void createComputeShaders();
    void createTextures();
    void initializeParticles();

private:
    int   m_particleCount;
    int   m_width;
    int   m_height;
    bool  m_initialized;

    // Buffer particelle in double buffering
    GLuint m_particleBuffers[2];
    int    m_currentBuffer;

    // Due texture per ping-pong
    GLuint m_textureIDIn;
    GLuint m_textureIDOut;

    // Shader compute
    GLuint m_updateProgramID;
    GLuint m_blurProgramID;

    // Parametri di simulazione (esempio)
    float m_sensorDistance;
    float m_sensorAngle;
    float m_turnAngle;
    float m_speed;
    float m_randomWeight;
};
