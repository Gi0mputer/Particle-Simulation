#pragma once

#include <glad/glad.h>
#include <algorithm>

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
    int    getParticleCount() const  { return m_activeParticles; }
    int    getMaxParticleCount() const { return m_maxParticles; }
    void   setActiveParticleCount(int count);

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
    
    // Motion tuning
    float getInertia() const { return m_inertia; }
    void setInertia(float val) { m_inertia = std::clamp(val, 0.0f, 0.999f); }
    float getRestitution() const { return m_restitution; }
    void setRestitution(float val) { m_restitution = std::clamp(val, 0.0f, 1.5f); }
    
    float getRandomWeight() const { return m_randomWeight; }
    void setRandomWeight(float val) { m_randomWeight = val; }

    // Boundary topology: 0=Toroidal, 1=Bounce, 2=Klein X-flip, 3=Klein Y-flip
    int  getBoundaryMode() const { return m_boundaryMode; }
    void setBoundaryMode(int mode) { m_boundaryMode = (mode < 0) ? 0 : (mode > 3 ? 3 : mode); }
    
    // Physarum enable/disable
    bool isPhysarumEnabled() const { return m_physarumEnabled; }
    void setPhysarumEnabled(bool enabled) { m_physarumEnabled = enabled; }
    
    float getPhysarumIntensity() const { return m_physarumIntensity; }
    void setPhysarumIntensity(float val) { m_physarumIntensity = val; }
    
    // Boids enable/disable
    bool isBoidsEnabled() const { return m_boidsEnabled; }
    void setBoidsEnabled(bool enabled) { m_boidsEnabled = enabled; }
    
    float getAlignmentWeight() const { return m_alignmentWeight; }
    void setAlignmentWeight(float val) { m_alignmentWeight = val; }
    
    float getSeparationWeight() const { return m_separationWeight; }
    void setSeparationWeight(float val) { m_separationWeight = val; }
    
    float getCohesionWeight() const { return m_cohesionWeight; }
    void setCohesionWeight(float val) { m_cohesionWeight = val; }
    
    float getBoidsRadius() const { return m_boidsRadius; }
    void setBoidsRadius(float val) { m_boidsRadius = val; }

    // Collisions
    void setCollisionsEnabled(bool enabled) { m_collisionsEnabled = enabled; }
    bool getCollisionsEnabled() const { return m_collisionsEnabled; }
    void setCollisionRadius(float radius) { m_collisionRadius = radius; }
    float getCollisionRadius() const { return m_collisionRadius; }

    // Colors
    void setColor1(float r, float g, float b) { m_color1[0]=r; m_color1[1]=g; m_color1[2]=b; }
    void setColor2(float r, float g, float b) { m_color2[0]=r; m_color2[1]=g; m_color2[2]=b; }
    float* getColor1() { return m_color1; }
    float* getColor2() { return m_color2; }

private:
    void createComputeShaders();
    void createTextures();
    void initializeParticles();
    void createGridBuffers();  // New: Grid initialization
    void rebuildGridIfNeeded();

private:
    int   m_maxParticles;
    int   m_activeParticles;
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

    // Parametri di simulazione
    float m_sensorDistance;
    float m_sensorAngle;
    float m_turnAngle;
    float m_speed;
    float m_inertia;
    float m_restitution;
    float m_randomWeight;
    int   m_boundaryMode;
    
    // Physarum control
    bool m_physarumEnabled;
    float m_physarumIntensity;
    
    // Boids control
    bool m_boidsEnabled;
    float m_alignmentWeight;
    float m_separationWeight;
    float m_cohesionWeight;
    float m_boidsRadius;  // Interaction radius
    bool  m_collisionsEnabled;
    float m_collisionRadius;
    
    // Colors
    float m_color1[3];
    float m_color2[3];

    // Spatial Grid for Boids (Atomic Linked List)
    int m_gridWidth;
    int m_gridHeight;
    float m_cellSize; // ~80-100 pixels
    
    GLuint m_gridHeadBuffer;      // Stores index of first particle in cell
    GLuint m_particleNextBuffer;  // Stores index of next particle in list
    
    // Shader for rebuilding grid
    GLuint m_gridResetProgramID;  // Clears the grid
    GLuint m_gridBuildProgramID;  // Atomic-adds particles
    
    // Profiling
    GLuint m_timeQueries[4]; // Start, Grid, Update, Blur
    void printPerformanceStats();
};
