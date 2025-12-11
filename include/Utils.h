#pragma once

#include <iostream>
#include <string>
// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Utils
{
    //------------------------------------------------------------
    // Gestione del timestep
    class TimestepManager
    {
    public:
        explicit TimestepManager(double fixedDt);
        void init(double currentTime);
        void update(double currentTime);
        bool hasSteps();
        double getStepDt() const;

    private:
        double m_fixedDt;        // la dimensione di ogni step fisso
        double m_accumulator;    // accumulatore di tempo
        double m_lastFrameTime;  // ultimo "currentTime" usato
    };

    // Se vuoi gestire l'eventuale "time leftover"
    // per un interpolated rendering, potresti aggiungere un getAlpha() = m_accumulator / m_fixedDt;

    //------------------------------------------------------------
    // Conto degli fps
    class FPSCounter
    {
    public:
        FPSCounter();
        void update();
        bool shouldRefreshTitle() const;
        int getFPS() const;

    private:
        double lastTime;
        int frameCount;
        int fps;
        bool refreshNeeded;
    };




    //------------------------------------------------------------
    // Gestione delle dimensioni di sumulazione
    enum class SimulationResolution
    {
        HD_720,
        FHD_1080,
        QHD_1440,
        UHD_4K,
        NATIVE
    };
    class SimulationManager
    {
    public:
        static void getSimulationSize(SimulationResolution resolution, int &simWidth, int &simHeight);
    };

    //------------------------------------------------------------
    // Info hardware (GPU) per preset automatici
    struct HardwareInfo
    {
        std::string vendor;
        std::string renderer;
    };
    HardwareInfo getHardwareInfo();
    std::string makeHardwarePresetName(const HardwareInfo& info);

    //------------------------------------------------------------
    // Funzioni per inizializzare GLAD e configurare OpenGL
    bool initializeGLAD();
    void configureOpenGL();
}
