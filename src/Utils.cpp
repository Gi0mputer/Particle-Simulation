#include "Utils.h"

namespace Utils
{

    // Costruttore: definisci un dt fisso
    TimestepManager::TimestepManager(double fixedDt)
        : m_fixedDt(fixedDt), m_accumulator(0.0), m_lastFrameTime(0.0)
    {
    }

    // Da chiamare all'inizio, es. con il valore iniziale di glfwGetTime()
    void TimestepManager::init(double currentTime)
    {
        m_lastFrameTime = currentTime;
    }

    // Aggiorna i dati con l'orario corrente
    // in modo da accumulare il tempo trascorso
    void TimestepManager::update(double currentTime)
    {
        double frameTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        m_accumulator += frameTime;
    }

    // Restituisce true se c'è ancora uno step da fare
    // (in tal caso, decrementa l'accumulatore).
    bool TimestepManager::hasSteps()
    {
        if (m_accumulator >= m_fixedDt)
        {
            m_accumulator -= m_fixedDt;
            return true;
        }
        return false;
    }

    // Ritorna la dimensione di ciascun step
    double TimestepManager::getStepDt() const
    {
        return m_fixedDt;
    }

    //------------------------------------------------------------
    // Conto degli fps
    FPSCounter::FPSCounter()
        : lastTime(glfwGetTime()), frameCount(0), fps(0), refreshNeeded(false)
    {
    }

    void FPSCounter::update()
    {
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - lastTime >= 1.0)
        {
            fps = frameCount;
            frameCount = 0;
            lastTime += 1.0;
            refreshNeeded = true;
        }
        else
        {
            refreshNeeded = false;
        }
    }

    bool FPSCounter::shouldRefreshTitle() const
    {
        return refreshNeeded;
    }

    int FPSCounter::getFPS() const
    {
        return fps;
    }

    //------------------------------------------------------------
    // Gestione delle dimensioni di sumulazione
    void SimulationManager::getSimulationSize(SimulationResolution resolution, int &simWidth, int &simHeight)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        switch (resolution)
        {
        case SimulationResolution::NATIVE:
            simWidth = mode->width;
            simHeight = mode->height;
            break;
        case SimulationResolution::UHD_4K:
            simWidth = 3840;
            simHeight = 2160;
            break;
        case SimulationResolution::FHD_1080:
            simWidth = 1920;
            simHeight = 1080;
            break;
        case SimulationResolution::HD_720:
            simWidth = 1280;
            simHeight = 720;
            break;
        default:
            simWidth = 1920;
            simHeight = 1080;
            break;
        }
    }

    //------------------------------------------------------------
    // Funzioni per inizializzare GLAD e configurare OpenGL
    bool initializeGLAD()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "[Utils] Failed to initialize GLAD!\n";
            return false;
        }
        std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
        return true;
    }

    void configureOpenGL()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Se vuoi abilitare il test di profondità
        // glEnable(GL_DEPTH_TEST);

        // Se vuoi dimensione punti più grande
        // glPointSize(5.0f);
    }

    // Esempio di altre funzioni se le vuoi
    // void clearScreen()
    // {
    //     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // }

    // void enableDebugCallback()
    // {
    //     ...
    // }
}
