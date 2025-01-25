#include <iostream>
#include <stdexcept>
#include <chrono> // Per misurare i tempi CPU
#include <string>

// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Header (da implementare a parte)
#include <WindowManager.h>    //Cosa cambia tra "" e <>?
#include <RenderPipeline.h>
#include <ISimulation.h>
#include <SimulationCPU.h>
#include <OpenGLUtils.h>
#include <SimulationManager.h>
#include <SimulationFactory.h>
//#include "InputHandler.h"

// Esempio di classe FPSCounter
// ---------------------------------------------------------------------------
class FPSCounter
{
public:
    FPSCounter()
        : lastTime(glfwGetTime()), frameCount(0), fps(0), refreshNeeded(false)
    {
    }

    void update()
    {
        double currentTime = glfwGetTime();
        frameCount++;

        // Se è passata 1 secondo
        if (currentTime - lastTime >= 1.0)
        {
            fps = frameCount; // frame totali nell'ultimo secondo
            frameCount = 0;
            lastTime += 1.0;
            refreshNeeded = true; // segnala che i dati sono cambiati
        }
        else
        {
            refreshNeeded = false;
        }
    }

    bool shouldRefreshTitle() const
    {
        return refreshNeeded;
    }

    int getFPS() const
    {
        return fps;
    }

private:
    double lastTime;
    int frameCount;
    int fps;
    bool refreshNeeded;
};

// MAIN
// ---------------------------------------------------------------------------
int main(int argc, char **argv)
{
    try
    {
        //// 1.  Creazione e inizializzazione finestra e contesto OpenGL
        WindowManager windowManager("Particle Simulation");
            GLFWwindow *window = windowManager.getWindow();

        //// 2.  Inizializza GLAD e config OpenGL //da commentare
        if (!OpenGLUtils::initializeGLAD())
        {
            throw std::runtime_error("Impossibile inizializzare GLAD."); // perche ritorno bool?
        }
        OpenGLUtils::configureOpenGL();

        //// 3.  Crea la simulazione
        int simWidth, simHeight;
        SimulationManager::getSimulationSize(SimulationResolution::HD_720, simWidth, simHeight);
        std::cout << "[Simulation] Dimensioni fisse: " << simWidth << " x " << simHeight << std::endl;

        // Decidi se usare CPU o GPU
        bool useGPU = false;
        std::cout << "[Simulation] Using " << (useGPU ? "GPU" : "CPU") << " version.\n";

        // 5) Crea la simulazione
        ISimulation* simulation = nullptr;
        simulation = SimulationFactory::createSimulation(useGPU ? SimulationType::GPU : SimulationType::CPU,
                                                10000, simWidth, simHeight);
        simulation->initialize(); // alloca e imposta la simulazione

   
        // 4) Creazione della RenderPipeline
        //    Nel costruttore si possono caricare e compilare gli shader,
        //    creare FBO e texture di ping-pong, ecc.
        // --------------------------------------------------------------------------
        // Ora crei la pipeline con questi valori
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();
        // (Facoltativo) se vuoi passare reference della simulazione per configurare, es:
        // renderPipeline.setSimulation(&simulation);


        // 5) Inizializza un eventuale sistema di input (mouse, keyboard)
        // --------------------------------------------------------------------------
        //InputHandler inputHandler(window);

        // 6) Variabili per il loop di gioco
        // -------------------------------------------------------------------
        // Gestione del timestep:
        //   - "fixedTimeStep" per la simulazione (ad es. 1/60 = 0.0166..)
        //   - "accumulator" accumula il tempo trascorso
        double fixedTimeStep = 1.0 / 60.0;
        double accumulator = 0.0;
        double lastFrameTime = glfwGetTime();

        // Per contare gli FPS
        FPSCounter fpsCounter;

        // 7) Loop principale
        // -------------------------------------------------------------------
        while (!windowManager.shouldClose())
        {
            // 7.1) Gestione input (tastiera, mouse)
            glfwPollEvents();
            //inputHandler.update();

            // Esempio: interazione con la simulazione
            // if (inputHandler.isMouseClicked()) {
            //     double mx, my;
            //     inputHandler.getMousePosition(mx, my);
            //     simulation.addParticle(mx, my);
            // }

            // 7.2) Calcolo deltaTime / accumulo per timestep fisso
            double currentTime = glfwGetTime();
            double frameTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            accumulator += frameTime;

            // 7.3) Update simulazione con step fisso (se preferisci step variabile, puoi semplificare)
            while (accumulator >= fixedTimeStep)
            {
                // Eventuale misurazione tempo CPU per l'update
                auto startSim = std::chrono::high_resolution_clock::now();
                // 6.2) Update simulazione (CPU mode)
                //     Se in futuro userai compute shader, la chiamata interna farà glDispatchCompute
                // ----------------------------------------------------------------------

                simulation->update(static_cast<float>(fixedTimeStep));

                auto endSim = std::chrono::high_resolution_clock::now();
                double msSim = std::chrono::duration<double, std::milli>(endSim - startSim).count();

                // Se vuoi stampare un log di debug sulle performance CPU di update
                // std::cout << "[Simulation] " << msSim << "ms\n";

                accumulator -= fixedTimeStep;
            }

            // 7.4) Render (misura il tempo di rendering su CPU)
            auto startRender = std::chrono::high_resolution_clock::now();

            // 6.3) Render con la pipeline
            //     La pipeline si occuperà di:
            //       - gestire ping-pong pass (trail, smoothing, blur, ecc.)
            //       - disegnare su schermo l'immagine finale
            // --------------------------------------------------------------------- 


            renderPipeline.render(*simulation,  windowManager.getWidth(), windowManager.getHeight());

            auto endRender = std::chrono::high_resolution_clock::now();
            double msRender = std::chrono::duration<double, std::milli>(endRender - startRender).count();
            // I valori stampati (o usati per modificare il titolo della finestra) sono tempi CPU, quindi se la GPU è impegnata su un calcolo interno e non eseguiamo un “glFinish” o uno “Sync/Wait”, potremmo misurare meno di quanto la GPU impiega davvero.
            // Per un vero benchmark GPU servirebbe l’uso di OpenGL timer queries (es. glGenQueries, glBeginQuery(GL_TIME_ELAPSED, queryID), glEndQuery(...), glGetQueryObjectui64v(...), ecc.).
            // Se hai glfwSwapInterval(1) attivo, la chiamata a swapBuffers() bloccherà a ~60 fps (o la frequenza del monitor), “nascondendo” parte del tuo tempo di rendering.
            // Se vuoi un framerate sbloccato, disattiva vsync (glfwSwapInterval(0)) e valuta un capping manuale se necessario.

            // 7.5) Aggiorna contatore FPS e, se necessario, aggiorna il titolo finestra
            fpsCounter.update();
            if (fpsCounter.shouldRefreshTitle())
            {
                int fps = fpsCounter.getFPS();

                // Nel titolo puoi mettere FPS, ms di rendering, ecc.
                std::string newTitle = "Particle Simulation - FPS: " + std::to_string(fps) +
                                       " | RenderTime: " + std::to_string(msRender) + "ms";
                //windowManager.setTitle(newTitle.c_str());
                //creare una piccola finestra con le statistiche
            }

            // 7.6) Swap buffer
            windowManager.swapBuffers();
        }

        // 8) Fine, cleanup automatico nel distruttore delle classi

        delete simulation;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return -1;
    }
}
