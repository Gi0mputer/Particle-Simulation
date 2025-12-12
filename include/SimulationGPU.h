#pragma once

#include <glad/glad.h>
#include <algorithm>

// Struttura GpuParticle
struct GpuParticle {
    float position[2];
    float angle;
    float speed;
    float species;     // 0.0, 1.0, etc.
    float _pad[3];     // Padding to reach 32 bytes (2x vec4 alignment friendly)
};

class SimulationGPU
{
public:

    SimulationGPU(int particleCount, int width, int height);
    ~SimulationGPU();

    void initialize();
    void update(float dt, float mouseX, float mouseY, bool mousePressed, int mouseMode);

    enum class TextureFormat { R8, RG8, RGBA8 };

    // Accesso al buffer delle particelle (per eventuale debug drawing)
    GLuint getParticleBuffer() const { return m_particleBuffers[m_currentBuffer]; }
    int    getParticleCount() const  { return m_activeParticles; }
    int    getMaxParticleCount() const { return m_maxParticles; }
    void   setActiveParticleCount(int count);

    // Resizes simulation and changes texture format upon request
    void   resize(int width, int height, TextureFormat format);
    TextureFormat getTextureFormat() const { return m_textureFormat; }

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
    void setSpeed(float val) { m_speed = std::clamp(val, m_speedMin, m_speedMax); }
    float getSpeedMin() const { return m_speedMin; }
    float getSpeedMax() const { return m_speedMax; }
    void setSpeedRange(float minVal, float maxVal) {
        m_speedMin = std::max(0.0f, minVal);
        m_speedMax = std::max(m_speedMin + 1.0f, maxVal);
        m_speed = std::clamp(m_speed, m_speedMin, m_speedMax);
    }

    // Trails / post
    float getTrailFade() const { return m_trailFade; }
    void setTrailFade(float val) { m_trailFade = std::clamp(val, 0.5f, 0.9999f); }
    float getToneExposure() const { return m_toneExposure; }
    void setToneExposure(float val) { m_toneExposure = std::clamp(val, 0.01f, 20.0f); }
    float getAutoDimThreshold() const { return m_autoDimThreshold; }
    void setAutoDimThreshold(float val) { m_autoDimThreshold = std::clamp(val, 0.0f, 1.0f); }
    float getAutoDimStrength() const { return m_autoDimStrength; }
    void setAutoDimStrength(float val) { m_autoDimStrength = std::clamp(val, 0.0f, 1.0f); }
    float getAutoDimGlobal() const { return m_autoDimGlobal; }
    void setAutoDimGlobal(float val) { m_autoDimGlobal = std::clamp(val, 0.0f, 20.0f); }
    
    // Motion tuning
    float getInertia() const { return m_inertia; }
    void setInertia(float val) { m_inertia = std::clamp(val, 0.0f, 0.999f); }
    float getRestitution() const { return m_restitution; }
    void setRestitution(float val) { m_restitution = std::clamp(val, 0.0f, 1.5f); }
    
    float getRandomWeight() const { return m_randomWeight; }
    void setRandomWeight(float val) { m_randomWeight = val; }

    // Boundary topology: 0=Toroidal, 1=Bounce, 2=Klein X-flip, 3=Klein Y-flip
    int  getBoundaryMode() const { return m_boundaryMode; }
    void setBoundaryMode(int mode) { m_boundaryMode = (mode < 0) ? 0 : (mode > 2 ? 2 : mode); }
    
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

    // Mouse forces
    void setMouseFalloff(int type) { m_mouseFalloff = type; }
    void setMouseStrength(float s) { m_mouseStrength = s; }
    void setMouseGaussianSigma(float s) { m_mouseGaussianSigma = s; }
    void setMouseOscFreq(float f) { m_mouseOscFreq = f; }
    void setMouseRingOverlay(bool enabled) { m_mouseRingOverlay = enabled; }
    void setMouseRingRadius(float r) { m_mouseRingRadius = r; }

    // Colors
    void setColor1(float r, float g, float b) { m_color1[0]=r; m_color1[1]=g; m_color1[2]=b; }
    void setColor2(float r, float g, float b) { m_color2[0]=r; m_color2[1]=g; m_color2[2]=b; }
    float* getColor1() { return m_color1; }
    float* getColor2() { return m_color2; }
    
    // Advanced Color Behavior
    void setColorOffset(float val) { m_colorOffset = val; }
    float getColorOffset() const { return m_colorOffset; }
    void setColorSource(int src) { m_colorSource = (src < 0) ? 0 : (src > 2 ? 2 : src); }
    int  getColorSource() const { return m_colorSource; }
    void setColorSpeedRange(float minVal, float maxVal) { 
        m_colorSpeedMin = minVal; 
        m_colorSpeedMax = std::max(maxVal, minVal + 1.0f); 
    }
    float getColorSpeedMin() const { return m_colorSpeedMin; }
    float getColorSpeedMax() const { return m_colorSpeedMax; }
    float getAutoColorSpeedMin() const { return m_autoSpeedMin; }
    float getAutoColorSpeedMax() const { return m_autoSpeedMax; }
    bool  hasAutoSpeedStats() const { return m_autoSpeedValid; }

private:
    void createComputeShaders();
    void createTextures();
    void initializeParticles();
    void createGridBuffers();  // New: Grid initialization
    void rebuildGridIfNeeded();

private:
    int   m_maxParticles;
    int   m_activeParticles;
    int   m_targetParticles;
    bool  m_rampingUp;

    int   m_width;
    int   m_height;
    TextureFormat m_textureFormat;
    bool  m_initialized;

    void  resetParticlePositions(int startIdx, int count);

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
    float m_speedMin;
    float m_speedMax;
    float m_speed;
    float m_trailFade;
    float m_toneExposure;
    float m_autoDimThreshold;
    float m_autoDimStrength;
    float m_autoDimGlobal;
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

    // Mouse forces
    int   m_mouseFalloff;
    float m_mouseStrength;
    float m_mouseGaussianSigma;
    float m_mouseOscFreq;
    bool  m_mouseRingOverlay;
    float m_mouseRingRadius;
    
    // Colors
    float m_color1[3];
    float m_color2[3];
    float m_colorOffset;
    int   m_colorSource;     // 0=angle, 1=speed manual, 2=speed auto
    float m_colorSpeedMin;
    float m_colorSpeedMax;
    float m_autoSpeedMin;
    float m_autoSpeedMax;
    bool  m_autoSpeedValid;
    float m_speedSampleInterval;
    float m_speedSampleTimer;
    int   m_speedSampleCount;

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
