#include "SimulationGPU.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdlib> // rand()
#include <iostream>
#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h> // se ti serve per glfwGetTime

static std::string readFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint compileShader(const std::string &source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Controlla errori di compilazione
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::string shaderTypeStr = (shaderType == GL_COMPUTE_SHADER ? "COMPUTE" : "UNKNOWN");
        throw std::runtime_error("Shader compile error (" + shaderTypeStr + "):\n" + infoLog);
    }
    return shader;
}

// --------------------------------------------------

SimulationGPU::SimulationGPU(int particleCount, int width, int height)
    : m_maxParticles(particleCount)
    , m_activeParticles(particleCount)
    , m_width(width)
    , m_height(height)
    , m_initialized(false)
    , m_currentBuffer(0)
    , m_textureIDIn(0)
    , m_textureIDOut(0)
    , m_updateProgramID(0)
    , m_blurProgramID(0)
    , m_sensorDistance(20.0f)
    , m_sensorAngle(0.785f)
    , m_turnAngle(0.785f)
    , m_speed(100.0f)
    , m_trailFade(0.99f)
    , m_toneExposure(3.0f)
    , m_autoDimThreshold(0.25f)
    , m_autoDimStrength(0.5f)
    , m_autoDimGlobal(4.0f)
    , m_inertia(0.85f)
    , m_restitution(1.0f)
    , m_randomWeight(0.05f)
    , m_boundaryMode(0)
    , m_physarumEnabled(false)
    , m_physarumIntensity(1.0f)
    , m_colorOffset(0.0f) // New
    , m_boidsEnabled(false)
    , m_alignmentWeight(1.0f)
    , m_separationWeight(1.2f)
    , m_cohesionWeight(1.0f)
    , m_boidsRadius(50.0f)
    , m_collisionsEnabled(false)
    , m_collisionRadius(30.0f)
    , m_gridWidth(0)
    , m_gridHeight(0)
    , m_cellSize(40.0f)
    , m_gridHeadBuffer(0)
    , m_particleNextBuffer(0)
    , m_gridResetProgramID(0)
    , m_gridBuildProgramID(0)
{
    m_color1[0] = 0.0f; m_color1[1] = 1.0f; m_color1[2] = 1.0f; // Cyan
    m_color2[0] = 1.0f; m_color2[1] = 0.0f; m_color2[2] = 1.0f; // Magenta
    m_particleBuffers[0] = 0;
    m_particleBuffers[1] = 0;
}

SimulationGPU::~SimulationGPU()
{
    // Rilascia risorse
    if (m_updateProgramID) glDeleteProgram(m_updateProgramID);
    if (m_blurProgramID) glDeleteProgram(m_blurProgramID);
    if (m_gridResetProgramID) glDeleteProgram(m_gridResetProgramID);
    if (m_gridBuildProgramID) glDeleteProgram(m_gridBuildProgramID);

    glDeleteTextures(1, &m_textureIDIn);
    glDeleteTextures(1, &m_textureIDOut);
    glDeleteBuffers(2, m_particleBuffers);
    glDeleteBuffers(1, &m_gridHeadBuffer);
    glDeleteBuffers(1, &m_particleNextBuffer);
    
    glDeleteQueries(4, m_timeQueries);
}

void SimulationGPU::initialize()
{
    if (m_initialized) return;

    createComputeShaders();
    createTextures();
    createGridBuffers();

    // Crea i due SSBO per le particelle
    glGenBuffers(2, m_particleBuffers);
    size_t bufferSize = static_cast<size_t>(m_maxParticles) * sizeof(GpuParticle);

    for(int i=0; i<2; ++i) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffers[i]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_COPY);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Inizializza le particelle nel buffer[0]
    initializeParticles();
    m_currentBuffer = 0;

    m_initialized = true;
    
    // Performance Queries
    glGenQueries(4, m_timeQueries);
}

void SimulationGPU::setActiveParticleCount(int count)
{
    int clamped = std::max(1, std::min(count, m_maxParticles));
    if (clamped == m_activeParticles) return;

    // If we are enabling more particles, seed them with random values.
    if (clamped > m_activeParticles) {
        int start = m_activeParticles;
        int toInit = clamped - m_activeParticles;

        std::vector<GpuParticle> particles(toInit);
        for (int i = 0; i < toInit; ++i)
        {
            particles[i].position[0] = static_cast<float>(rand() % m_width);
            particles[i].position[1] = static_cast<float>(rand() % m_height);
            particles[i].angle = static_cast<float>(rand()) / RAND_MAX * 6.28318530718f;

            float minSpeed = 10.0f, maxSpeed = 100.0f;
            float r = static_cast<float>(rand()) / RAND_MAX;
            particles[i].speed = minSpeed + r*(maxSpeed - minSpeed);
        }

        GLintptr offset = static_cast<GLintptr>(start) * sizeof(GpuParticle);
        GLsizeiptr sizeBytes = static_cast<GLsizeiptr>(toInit) * sizeof(GpuParticle);
        for (int i = 0; i < 2; ++i) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffers[i]);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeBytes, particles.data());
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    m_activeParticles = clamped;
}

// --------------------------------------------------
void SimulationGPU::update(float dt, float mouseX, float mouseY, bool mousePressed, int mouseMode)
{
    if (!m_initialized) return;
    
    const int activeCount = m_activeParticles;

    // 0. Start Timer
    glQueryCounter(m_timeQueries[0], GL_TIMESTAMP);

    // --- PASS 0: Grid Reset & Build (needed for Boids or Collisions) ---
    if (m_boidsEnabled || m_collisionsEnabled) {
        rebuildGridIfNeeded();
        // ... (Calls Grid Shaders)
        glUseProgram(m_gridResetProgramID);
        int numCells = m_gridWidth * m_gridHeight;
        glUniform1i(glGetUniformLocation(m_gridResetProgramID, "uNumCells"), numCells);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_gridHeadBuffer);
        glDispatchCompute((numCells + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(m_gridBuildProgramID);
        glUniform1i(glGetUniformLocation(m_gridBuildProgramID, "uParticleCount"), activeCount);
        glUniform1f(glGetUniformLocation(m_gridBuildProgramID, "uCellSize"), m_cellSize);
        glUniform1i(glGetUniformLocation(m_gridBuildProgramID, "uGridWidth"), m_gridWidth);
        glUniform1i(glGetUniformLocation(m_gridBuildProgramID, "uGridHeight"), m_gridHeight);
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffers[m_currentBuffer]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_gridHeadBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_particleNextBuffer);
        
        glDispatchCompute((activeCount + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    
    // 1. Grid Done
    glQueryCounter(m_timeQueries[1], GL_TIMESTAMP);

    // --- PASS 1: Particle Update & Deposit ---
    {
       glUseProgram(m_updateProgramID);
       
       // ... Uniforms and Bindings ...
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uDt"), dt);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uParticleCount"), activeCount);
       glUniform2f(glGetUniformLocation(m_updateProgramID, "uSimSize"), (float)m_width, (float)m_height);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uBoundaryMode"), m_boundaryMode);
       
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uPhysarumEnabled"), m_physarumEnabled ? 1 : 0);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uPhysarumIntensity"), m_physarumIntensity);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSensorDistance"), m_sensorDistance);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSensorAngle"), m_sensorAngle);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uTurnAngle"), m_turnAngle);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSpeed"), m_speed);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uInertia"), m_inertia);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uRestitution"), m_restitution);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uRandomWeight"), m_randomWeight);
       
       // Colors
       glUniform3fv(glGetUniformLocation(m_updateProgramID, "uColor1"), 1, m_color1);
       glUniform3fv(glGetUniformLocation(m_updateProgramID, "uColor2"), 1, m_color2);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uColorOffset"), m_colorOffset);

       glUniform1i(glGetUniformLocation(m_updateProgramID, "uCollisionsEnabled"), m_collisionsEnabled ? 1 : 0);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uCollisionRadius"), m_collisionRadius);

       glUniform1i(glGetUniformLocation(m_updateProgramID, "uBoidsEnabled"), m_boidsEnabled ? 1 : 0);
       if (m_boidsEnabled || m_collisionsEnabled) {
           glUniform1f(glGetUniformLocation(m_updateProgramID, "uAlignmentWeight"), m_alignmentWeight);
           glUniform1f(glGetUniformLocation(m_updateProgramID, "uSeparationWeight"), m_separationWeight);
           glUniform1f(glGetUniformLocation(m_updateProgramID, "uCohesionWeight"), m_cohesionWeight);
           glUniform1f(glGetUniformLocation(m_updateProgramID, "uBoidsRadius"), m_boidsRadius);
           glUniform1f(glGetUniformLocation(m_updateProgramID, "uCellSize"), m_cellSize);
           glUniform1i(glGetUniformLocation(m_updateProgramID, "uGridWidth"), m_gridWidth);
           glUniform1i(glGetUniformLocation(m_updateProgramID, "uGridHeight"), m_gridHeight);
           
           glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_gridHeadBuffer);
           glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_particleNextBuffer);
       }

       glUniform2f(glGetUniformLocation(m_updateProgramID, "uMousePos"), mouseX, mouseY);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uMousePressed"), mousePressed ? 1 : 0);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uMouseMode"), mouseMode);

       int nextBuffer = 1 - m_currentBuffer;
       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffers[m_currentBuffer]);
       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffers[nextBuffer]);

       glBindImageTexture(2, m_textureIDIn, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

       GLuint groupSize = 128; 
       GLuint numGroups = (activeCount + groupSize - 1) / groupSize;
       glDispatchCompute(numGroups, 1, 1);

       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
       m_currentBuffer = nextBuffer;
    }
    
    // 2. Update Done
    glQueryCounter(m_timeQueries[2], GL_TIMESTAMP);

    // --- PASS 2: Blur ---
    {
       glUseProgram(m_blurProgramID);

       glUniform2i(glGetUniformLocation(m_blurProgramID, "uImageSize"), m_width, m_height);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uFade"), m_trailFade);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uToneExposure"), m_toneExposure);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uAutoDimThreshold"), m_autoDimThreshold);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uAutoDimStrength"), m_autoDimStrength);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uAutoDimGlobal"), m_autoDimGlobal);

       glBindImageTexture(0, m_textureIDIn,  0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
       glBindImageTexture(1, m_textureIDOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

       GLuint gx = (m_width  + 15) / 16;
       GLuint gy = (m_height + 15) / 16;
       glDispatchCompute(gx, gy, 1);

       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

       std::swap(m_textureIDIn, m_textureIDOut);
    }
    
    // 3. Blur Done
    glQueryCounter(m_timeQueries[3], GL_TIMESTAMP);
    
    printPerformanceStats();
}

// --------------------------------------------------
void SimulationGPU::createComputeShaders()
{
    // update.comp
    {
        std::string compSource = readFile("shaders/update.comp");
        GLuint compShader = compileShader(compSource, GL_COMPUTE_SHADER);

        m_updateProgramID = glCreateProgram();
        glAttachShader(m_updateProgramID, compShader);
        glLinkProgram(m_updateProgramID);

        GLint success;
        glGetProgramiv(m_updateProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_updateProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Update shader link error:\n" + std::string(infoLog));
        }
        glDeleteShader(compShader);
    }

    // blur.comp
    {
        std::string compSource = readFile("shaders/blur.comp");
        GLuint compShader = compileShader(compSource, GL_COMPUTE_SHADER);

        m_blurProgramID = glCreateProgram();
        glAttachShader(m_blurProgramID, compShader);
        glLinkProgram(m_blurProgramID);

        GLint success;
        glGetProgramiv(m_blurProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_blurProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Blur shader link error:\n" + std::string(infoLog));
        }
        glDeleteShader(compShader);
    }

    // grid_reset.comp
    {
        std::string compSource = readFile("shaders/grid_reset.comp");
        GLuint compShader = compileShader(compSource, GL_COMPUTE_SHADER);

        m_gridResetProgramID = glCreateProgram();
        glAttachShader(m_gridResetProgramID, compShader);
        glLinkProgram(m_gridResetProgramID);

        GLint success;
        glGetProgramiv(m_gridResetProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_gridResetProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Grid Reset shader link error:\n" + std::string(infoLog));
        }
        glDeleteShader(compShader);
    }

    // grid_build.comp
    {
        std::string compSource = readFile("shaders/grid_build.comp");
        GLuint compShader = compileShader(compSource, GL_COMPUTE_SHADER);

        m_gridBuildProgramID = glCreateProgram();
        glAttachShader(m_gridBuildProgramID, compShader);
        glLinkProgram(m_gridBuildProgramID);

        GLint success;
        glGetProgramiv(m_gridBuildProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_gridBuildProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Grid Build shader link error:\n" + std::string(infoLog));
        }
        glDeleteShader(compShader);
    }
}

void SimulationGPU::createTextures()
{
    // Creiamo due texture RGBA8
    glGenTextures(1, &m_textureIDIn);
    glBindTexture(GL_TEXTURE_2D, m_textureIDIn);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Seconda texture per ping-pong
    glGenTextures(1, &m_textureIDOut);
    glBindTexture(GL_TEXTURE_2D, m_textureIDOut);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void SimulationGPU::initializeParticles()
{
    std::vector<GpuParticle> particles(m_maxParticles);
    for (int i = 0; i < m_maxParticles; ++i)
    {
        particles[i].position[0] = static_cast<float>(rand() % m_width);
        particles[i].position[1] = static_cast<float>(rand() % m_height);

        particles[i].angle = static_cast<float>(rand()) / RAND_MAX * 6.28318530718f;

        float minSpeed = 10.0f, maxSpeed = 100.0f;
        float r = static_cast<float>(rand()) / RAND_MAX;
        particles[i].speed = minSpeed + r*(maxSpeed - minSpeed);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffers[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    particles.size() * sizeof(GpuParticle),
                    particles.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationGPU::createGridBuffers()
{
    // Calcola dimensione griglia
    m_gridWidth = (m_width + (int)m_cellSize - 1) / (int)m_cellSize;
    m_gridHeight = (m_height + (int)m_cellSize - 1) / (int)m_cellSize;
    int numCells = m_gridWidth * m_gridHeight;
    
    // GridHead (init to -1)
    std::vector<int> initialHeads(numCells, -1);
    glGenBuffers(1, &m_gridHeadBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gridHeadBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numCells * sizeof(int), initialHeads.data(), GL_DYNAMIC_DRAW);
    
    // ParticleNext
    glGenBuffers(1, &m_particleNextBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleNextBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxParticles * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::cout << "[Grid] Initialized " << m_gridWidth << "x" << m_gridHeight 
              << " cells (" << numCells << ") for Spatial Hashing." << std::endl;
}

void SimulationGPU::rebuildGridIfNeeded()
{
    if (!(m_boidsEnabled || m_collisionsEnabled)) return;

    float targetRadius = std::max(m_boidsRadius, m_collisionRadius);
    float desiredCell = std::max(10.0f, targetRadius * 0.8f);
    desiredCell = std::min(desiredCell, 80.0f);

    if (std::abs(desiredCell - m_cellSize) < 0.1f) return;

    m_cellSize = desiredCell;

    if (m_gridHeadBuffer) {
        glDeleteBuffers(1, &m_gridHeadBuffer);
        m_gridHeadBuffer = 0;
    }
    if (m_particleNextBuffer) {
        glDeleteBuffers(1, &m_particleNextBuffer);
        m_particleNextBuffer = 0;
    }

    createGridBuffers();
}

void SimulationGPU::printPerformanceStats()
{
    GLint available = 0;
    // Check if Last query is available
    glGetQueryObjectiv(m_timeQueries[3], GL_QUERY_RESULT_AVAILABLE, &available);
    if (!available) return;

    GLuint64 times[4];
    glGetQueryObjectui64v(m_timeQueries[0], GL_QUERY_RESULT, &times[0]);
    glGetQueryObjectui64v(m_timeQueries[1], GL_QUERY_RESULT, &times[1]);
    glGetQueryObjectui64v(m_timeQueries[2], GL_QUERY_RESULT, &times[2]);
    glGetQueryObjectui64v(m_timeQueries[3], GL_QUERY_RESULT, &times[3]);
    
    double gridMs = (times[1] - times[0]) / 1000000.0;
    double updateMs = (times[2] - times[1]) / 1000000.0;
    double blurMs = (times[3] - times[2]) / 1000000.0;
    
    static int logCounter = 0;
    if (logCounter++ % 60 == 0) {
         std::cout << "[GPU] Grid: " << gridMs << "ms | Update: " << updateMs 
                   << "ms | Blur: " << blurMs << "ms" << std::endl;
    }
}
