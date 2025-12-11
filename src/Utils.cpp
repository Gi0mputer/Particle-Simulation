#include "Utils.h"
#include <cctype>

namespace Utils
{
    namespace
    {
        // Converte una stringa hardware in un token sicuro per il filesystem
        std::string sanitizePresetToken(const std::string& input)
        {
            std::string out;
            out.reserve(input.size());
            for (char ch : input)
            {
                unsigned char c = static_cast<unsigned char>(ch);
                if (std::isalnum(c))
                {
                    out.push_back(static_cast<char>(std::tolower(c)));
                }
                else if (c == ' ' || c == '-' || c == '_' || c == '.')
                {
                    out.push_back('_');
                }
            }

            // Rimuove underscore ripetuti
            std::string compact;
            compact.reserve(out.size());
            bool prevUnderscore = false;
            for (char ch : out)
            {
                if (ch == '_')
                {
                    if (!prevUnderscore)
                    {
                        compact.push_back(ch);
                        prevUnderscore = true;
                    }
                }
                else
                {
                    compact.push_back(ch);
                    prevUnderscore = false;
                }
            }

            // Trim finale
            while (!compact.empty() && compact.back() == '_')
            {
                compact.pop_back();
            }

            return compact;
        }
    }

    //------------------------------------------------------------
    // Gestione del timestep
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
        case SimulationResolution::QHD_1440:
            simWidth = 2560;
            simHeight = 1440;
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

    //------------------------------------------------------------
    // Info hardware (GPU) per preset automatici
    HardwareInfo getHardwareInfo()
    {
        HardwareInfo info;
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        info.vendor = vendor ? vendor : "unknown_vendor";
        info.renderer = renderer ? renderer : "unknown_renderer";
        return info;
    }

    std::string makeHardwarePresetName(const HardwareInfo& info)
    {
        std::string base = info.vendor;
        if (!info.renderer.empty())
        {
            if (!base.empty())
            {
                base += "_";
            }
            base += info.renderer;
        }

        if (base.empty())
        {
            base = "unknown_hw";
        }

        std::string sanitized = sanitizePresetToken(base);
        if (sanitized.empty())
        {
            sanitized = "unknown_hw";
        }
        if (sanitized.size() > 48)
        {
            sanitized.resize(48);
        }
        return sanitized;
    }

    // {
    //     ...
    // }
}
