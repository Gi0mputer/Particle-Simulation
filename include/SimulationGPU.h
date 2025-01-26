#pragma once

#include <vector>
#include <string>
#include "ISimulation.h"

class SimulationGPU : public ISimulation
{
public:
    SimulationGPU(int particleCount, int width, int height);
    ~SimulationGPU();

    void initialize() override;
    void update(float dt) override;

    // GPU-based: potremmo restituire un vettore vuoto in getParticles()
    // se non vogliamo scaricare i dati su CPU.
    const std::vector<Particle> &getParticles() const override
    {
        // Non carichiamo nulla su CPU
        static std::vector<Particle> dummy;
        return dummy;
    }

    int getParticleCount() const override
    {
        return m_particleCount;
    }
    
public:
    GLuint getParticleBufferID() const
    {
        return m_particleBuffer; // dove m_particleBuffer e' la SSBO/VBO
    }

private:
    void createComputeShader(const std::string &compPath);

    // Parametri
    int m_particleCount;
    int m_width, m_height;

    // handle per computeShader e buffer
    GLuint m_computeShaderID;
    GLuint m_programID;

    // SSBO o VBO per le particelle
    GLuint m_particleBuffer;

    bool m_initialized;

    // eventuali funzioni private
};
