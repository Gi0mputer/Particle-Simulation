#include "SimulationGPU.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdlib> // rand()
#include <iostream>

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
    : m_particleCount(particleCount)
    , m_width(width)
    , m_height(height)
    , m_initialized(false)
    , m_currentBuffer(0)
    , m_textureIDIn(0)
    , m_textureIDOut(0)
    , m_updateProgramID(0)
    , m_blurProgramID(0)
    , m_sensorDistance(20.0f) // Distanza sensore
    , m_sensorAngle(0.785f)   // 45 gradi
    , m_turnAngle(0.785f)     // 45 gradi
    , m_speed(100.0f)         // Velocita base
    , m_randomWeight(0.05f)   // Random noise
{
    m_particleBuffers[0] = 0;
    m_particleBuffers[1] = 0;
}

SimulationGPU::~SimulationGPU()
{
    // Rilascia risorse
    if (m_updateProgramID) {
        glDeleteProgram(m_updateProgramID);
    }
    if (m_blurProgramID) {
        glDeleteProgram(m_blurProgramID);
    }

    glDeleteTextures(1, &m_textureIDIn);
    glDeleteTextures(1, &m_textureIDOut);
    glDeleteBuffers(2, m_particleBuffers);
}

void SimulationGPU::initialize()
{
    if (m_initialized) return;

    createComputeShaders();
    createTextures();

    // Crea i due SSBO per le particelle
    glGenBuffers(2, m_particleBuffers);
    size_t bufferSize = m_particleCount * sizeof(GpuParticle);

    for(int i=0; i<2; ++i) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffers[i]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_COPY);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Inizializza le particelle nel buffer[0]
    initializeParticles();
    m_currentBuffer = 0;

    m_initialized = true;
}

void SimulationGPU::update(float dt, float mouseX, float mouseY, bool mousePressed, int mouseMode)
{
    if (!m_initialized) return;

    // PASS 1: update e deposit su m_textureIDIn
    {
       glUseProgram(m_updateProgramID);

       // Uniform
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uDt"), dt);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uParticleCount"), m_particleCount);
       glUniform2f(glGetUniformLocation(m_updateProgramID, "uSimSize"), (float)m_width, (float)m_height);
       
       // Physarum Settings
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSensorDistance"), m_sensorDistance);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSensorAngle"), m_sensorAngle);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uTurnAngle"), m_turnAngle);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uSpeed"), m_speed);
       glUniform1f(glGetUniformLocation(m_updateProgramID, "uRandomWeight"), m_randomWeight);

       // Mouse Interaction
       glUniform2f(glGetUniformLocation(m_updateProgramID, "uMousePos"), mouseX, mouseY);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uMousePressed"), mousePressed ? 1 : 0);
       glUniform1i(glGetUniformLocation(m_updateProgramID, "uMouseMode"), mouseMode);

       int nextBuffer = 1 - m_currentBuffer;
       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffers[m_currentBuffer]);
       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffers[nextBuffer]);

       // Binda la texture "m_textureIDIn" come read_write (deposit)
       glBindImageTexture(2, m_textureIDIn, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

       // dispatch
       GLuint groupSize = 128; 
       GLuint numGroups = (m_particleCount + groupSize - 1) / groupSize;
       glDispatchCompute(numGroups, 1, 1);

       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

       // swap buffer particelle
       m_currentBuffer = nextBuffer;
    }

    // PASS 2: blur (legge m_textureIDIn, scrive m_textureIDOut)
    {
       glUseProgram(m_blurProgramID);

       // Uniform
       glUniform2i(glGetUniformLocation(m_blurProgramID, "uImageSize"), m_width, m_height);
       glUniform1f(glGetUniformLocation(m_blurProgramID, "uFade"), 0.99f);

       // Binda inImage su binding=0 (readonly), outImage su binding=1 (writeonly)
       // blur.comp => layout(rgba8, binding=0) uniform readonly image2D inImage;
       //           => layout(rgba8, binding=1) uniform writeonly image2D outImage;
       glBindImageTexture(0, m_textureIDIn,  0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
       glBindImageTexture(1, m_textureIDOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

       // dispatch 2D
       GLuint gx = (m_width  + 15) / 16;
       GLuint gy = (m_height + 15) / 16;
       glDispatchCompute(gx, gy, 1);

       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

       // swap le due texture
       // cosi' m_textureIDIn conterra' il risultato finale
       std::swap(m_textureIDIn, m_textureIDOut);
    }
}

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
            throw std::runtime_error("Update compute shader link error:\n" 
                                     + std::string(infoLog));
        }
        glDeleteShader(compShader);
    }

    // blur.comp
    {
        std::string compSource = readFile("shaders/blur.comp");
        GLuint blurShader = compileShader(compSource, GL_COMPUTE_SHADER);

        m_blurProgramID = glCreateProgram();
        glAttachShader(m_blurProgramID, blurShader);
        glLinkProgram(m_blurProgramID);

        GLint success;
        glGetProgramiv(m_blurProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_blurProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Blur compute shader link error:\n" 
                                     + std::string(infoLog));
        }
        glDeleteShader(blurShader);
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
    std::vector<GpuParticle> particles(m_particleCount);
    for (int i = 0; i < m_particleCount; ++i)
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
