#include "SimulationGPU.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdlib> // rand()
#include <iostream>
#include "InputHandler.h"

#include <GLFW/glfw3.h> // se ti serve per glfwGetTime

static std::string readFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint compileShader(const std::string &source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Controlla errori di compilazione
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::string shaderTypeStr = (shaderType == GL_COMPUTE_SHADER ? "COMPUTE" : "UNKNOWN");
        throw std::runtime_error("Shader compile error (" + shaderTypeStr + "):\n" + infoLog);
    }
    return shader;
}

// --------------------------------------------------

SimulationGPU::SimulationGPU(int particleCount, int width, int height)
    : m_particleCount(particleCount), m_width(width), m_height(height), m_initialized(false), m_currentBuffer(0), m_textureIDIn(0), m_textureIDOut(0), m_updateProgramID(0), m_blurProgramID(0), m_sensorDistance(0), m_sensorAngle(0), m_turnAngle(0), m_speed(0), m_randomWeight(0)
{
    m_particleBuffers[0] = 0;
    m_particleBuffers[1] = 0;
}

SimulationGPU::~SimulationGPU()
{
    // Rilascia risorse
    if (m_updateProgramID)
    {
        glDeleteProgram(m_updateProgramID);
    }
    if (m_blurProgramID)
    {
        glDeleteProgram(m_blurProgramID);
    }

    glDeleteTextures(1, &m_textureIDIn);
    glDeleteTextures(1, &m_textureIDOut);
    glDeleteBuffers(2, m_particleBuffers);
}

void SimulationGPU::initialize()
{
    std::cout << "[SimulationGPU] Inizializzazione..." << std::endl;
    
    std::cout << "[SimulationGPU] Creazione shader..." << std::endl;
    createComputeShaders();
    
    std::cout << "[SimulationGPU] Creazione texture..." << std::endl;
    createTextures();
    
    std::cout << "[SimulationGPU] Creazione buffer particelle..." << std::endl;
    createParticleBuffers();
    
    std::cout << "[SimulationGPU] Inizializzazione completata" << std::endl;
}

void SimulationGPU::initializeParameters()
{
    m_sensorDistance = 0.1f;
    m_sensorAngle = 0.1f;
    m_turnAngle = 0.1f;
    m_speed = 0.1f;
    m_randomWeight = 0.1f;
}

void SimulationGPU::update(float dt, double mouseX, double mouseY, int mouseState)
{
    if (!m_initialized)
        return;
    // Clear the texture before starting
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_textureIDIn);
    glClearTexImage(m_textureIDIn, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_textureIDOut);
    glClearTexImage(m_textureIDOut, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // PASS 1: update e deposit su m_textureIDIn
    {
        glUseProgram(m_updateProgramID);

        // Impostazione delle uniform obbligatorie per la logica "physarum"
        GLint sdLoc = glGetUniformLocation(m_updateProgramID, "uSensorDistance");
        GLint saLoc = glGetUniformLocation(m_updateProgramID, "uSensorAngle");
        GLint taLoc = glGetUniformLocation(m_updateProgramID, "uTurnAngle");
        GLint rwLoc = glGetUniformLocation(m_updateProgramID, "uRandomWeight");
        GLint incLoc = glGetUniformLocation(m_updateProgramID, "uIncrement");

        if (sdLoc >= 0)
            glUniform1f(sdLoc, m_sensorDistance); // Per esempio 10.0f
        if (saLoc >= 0)
            glUniform1f(saLoc, m_sensorAngle); // Per esempio 0.25f
        if (taLoc >= 0)
            glUniform1f(taLoc, m_turnAngle); // Per esempio 0.3f
        if (rwLoc >= 0)
            glUniform1f(rwLoc, m_randomWeight); // Per esempio 0.1f

        // Se vuoi animare l'hue basandoti sul tempo, puoi fare:
        static float incVal = 0.0f;
        incVal += 0.1f; // oppure += dt * qualche_fattore
        if (incLoc >= 0)
            glUniform1f(incLoc, incVal);

        // Uniform
        GLint mouseLoc = glGetUniformLocation(m_updateProgramID, "uMousePos");
        if (mouseLoc >= 0)
        {
            glUniform2f(mouseLoc, static_cast<GLfloat>(mouseX), static_cast<GLfloat>(mouseY));
        }

        GLint mouseStat = glGetUniformLocation(m_updateProgramID, "uMouseStat");
        if (mouseStat >= 0)
        {
            glUniform1i(mouseStat, static_cast<GLint>(mouseState));
        }

        // Uniform attraction
        GLint attLoc = glGetUniformLocation(m_updateProgramID, "uMouseAttraction");
        if (attLoc >= 0)
            glUniform1f(attLoc, 500.0f); // param di forza

        // Radius
        GLint radLoc = glGetUniformLocation(m_updateProgramID, "uMouseRadius");
        if (radLoc >= 0)
            glUniform1f(radLoc, 1000.0f); // raggio di influenza

        glUniform1f(glGetUniformLocation(m_updateProgramID, "uDt"), dt);
        glUniform1i(glGetUniformLocation(m_updateProgramID, "uParticleCount"), m_particleCount);
        glUniform2f(glGetUniformLocation(m_updateProgramID, "uSimSize"),
                    (float)m_width, (float)m_height);

        int nextBuffer = 1 - m_currentBuffer;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffers[m_currentBuffer]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffers[nextBuffer]);

        // Binda la texture "m_textureIDIn" come read_write (deposit)
        glBindImageTexture(2, m_textureIDIn, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        // Binda la texture "m_textureIDOut" come read_write (deposit)
        glBindImageTexture(3, m_textureIDOut, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        // dispatch
        GLuint groupSize = 128; // corrisponde al local_size_x
        GLuint numGroups = (m_particleCount + groupSize - 1) / groupSize;
        glDispatchCompute(numGroups, 1, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        // swap buffer particelle
        m_currentBuffer = nextBuffer;
    }

    std::swap(m_textureIDIn, m_textureIDOut);
    // PASS 2: blur (legge m_textureIDIn, scrive m_textureIDOut)
    {
        glUseProgram(m_blurProgramID);

        // Uniform
        glUniform2i(glGetUniformLocation(m_blurProgramID, "uImageSize"), m_width, m_height);
        static int frameCount = 0;
        frameCount++;
        glUniform1i(glGetUniformLocation(m_blurProgramID, "uFrameCount"), frameCount);

        // Binda inImage su binding=0 (readonly), outImage su binding=1 (writeonly)
        // blur.comp => layout(rgba8, binding=0) uniform readonly image2D inImage;
        //           => layout(rgba8, binding=1) uniform writeonly image2D outImage;
        glBindImageTexture(0, m_textureIDIn, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        glBindImageTexture(1, m_textureIDOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        // dispatch 2D
        GLuint gx = (m_width + 15) / 16;
        GLuint gy = (m_height + 15) / 16;
        glDispatchCompute(gx, gy, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(m_updateProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Update compute shader link error:\n" + std::string(infoLog));
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
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(m_blurProgramID, 512, nullptr, infoLog);
            throw std::runtime_error("Blur compute shader link error:\n" + std::string(infoLog));
        }
        glDeleteShader(blurShader);
    }
}

void SimulationGPU::createTextures()
{
    std::cout << "[SimulationGPU] Creazione texture RGBA32F..." << std::endl;
    
    glGenTextures(1, &m_textureIDIn);
    glBindTexture(GL_TEXTURE_2D, m_textureIDIn);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glGenTextures(1, &m_textureIDOut);
    glBindTexture(GL_TEXTURE_2D, m_textureIDOut);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    std::cout << "[SimulationGPU] Texture create con successo" << std::endl;
}

void SimulationGPU::createParticleBuffers()
{
    std::cout << "[SimulationGPU] Creazione buffer particelle..." << std::endl;
    
    glGenBuffers(1, &m_particleBufferIn);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferIn);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &m_particleBufferOut);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferOut);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    std::cout << "[SimulationGPU] Buffer particelle creati con successo" << std::endl;
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
        particles[i].speed = minSpeed + r * (maxSpeed - minSpeed);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffers[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    particles.size() * sizeof(GpuParticle),
                    particles.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
