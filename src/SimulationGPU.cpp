#include "SimulationGPU.h"
#include <stdexcept>
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <cmath>


SimulationGPU::SimulationGPU(int particleCount, int width, int height)
    : m_particleCount(particleCount), m_width(width), m_height(height),
      m_computeShaderID(0), m_programID(0), m_particleBuffer(0), m_initialized(false)
{
}

SimulationGPU::~SimulationGPU()
{
    // Cleanup
    glDeleteProgram(m_programID);
    glDeleteBuffers(1, &m_particleBuffer);
}

void SimulationGPU::initialize()
{
    if (m_initialized)
        return;

    // 1) Crea un buffer di particelle in GPU (SSBO)
    glGenBuffers(1, &m_particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffer);

    // Alloca spazio per m_particleCount particelle GpuParticle
    std::vector<GpuParticle> initialData(m_particleCount);

    // Generazione di dati iniziali
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(m_width));
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(m_height));
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * static_cast<float>(M_PI));
    std::uniform_real_distribution<float> distSpeed(10.0f, 20.0f); // Velocità tra 10 e 20

    for (int i = 0; i < m_particleCount; ++i)
    {
        float x = distX(rng);
        float y = distY(rng);
        float angle = distAngle(rng);
        float speed = distSpeed(rng);

        initialData[i] = GpuParticle{ x, y, angle, speed };
    }

    // Caricamento dei dati su GPU
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 m_particleCount * sizeof(GpuParticle),
                 initialData.data(),
                 GL_DYNAMIC_DRAW);

    // Bind del buffer su binding 0 (per il compute shader)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 2) Crea e compila il compute shader
    createComputeShader("shaders/particleUpdate.comp"); // Assicurati che il path sia corretto

    m_initialized = true;
}

void SimulationGPU::update(float dt)
{
    if (!m_initialized)
        return;

    // Usa il programma del compute shader
    glUseProgram(m_programID);

    // Passa dt come uniform
    GLint dtLoc = glGetUniformLocation(m_programID, "uDeltaTime");
    if (dtLoc >= 0)
    {
        glUniform1f(dtLoc, dt);
    }

    // Passa le dimensioni della simulazione, se necessario
    GLint simSizeLoc = glGetUniformLocation(m_programID, "uSimSize");
    if (simSizeLoc >= 0)
    {
        glUniform2f(simSizeLoc, static_cast<float>(m_width), static_cast<float>(m_height));
    }


    GLint gravityStrengthLocation = glGetUniformLocation(m_programID, "uGravityStrength");
    GLint randomStrengthLocation = glGetUniformLocation(m_programID, "uRandomStrength");
    glUniform1f(gravityStrengthLocation, 1.0f); // inizia con 1.0 per un effetto evidente
    glUniform1f(randomStrengthLocation, 0.1f); // variazione casuale moderata


    // Binding del buffer su binding=0 (assicurati che corrisponda al compute shader)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffer);

    // Calcola il numero di gruppi di thread
    GLuint groupCount = (m_particleCount + 255) / 256; // Se local_size_x=256

    // Dispatch del compute shader
    glDispatchCompute(groupCount, 1, 1);

    // Barriera per assicurare che l'aggiornamento del buffer sia completato
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Unbind del buffer e del programma
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glUseProgram(0);
}

void SimulationGPU::createComputeShader(const std::string& compPath)
{
    // 1) Leggi il file .comp
    std::ifstream file(compPath);
    if (!file.is_open())
    {
        throw std::runtime_error("Compute shader file not found: " + compPath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string compSource = buffer.str();
    const char* compCode = compSource.c_str();

    // 2) Compila il compute shader
    GLuint compShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compShader, 1, &compCode, nullptr);
    glCompileShader(compShader);

    // Check per errori di compilazione
    GLint success;
    glGetShaderiv(compShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(compShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Compute shader compile error:\n") + infoLog);
    }

    // 3) Crea il programma del compute shader
    m_programID = glCreateProgram();
    glAttachShader(m_programID, compShader);
    glLinkProgram(m_programID);

    // Check per errori di link
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Compute program link error:\n") + infoLog);
    }

    // Cleanup del shader compilato
    glDeleteShader(compShader);
}
