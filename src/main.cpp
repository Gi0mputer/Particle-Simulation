#include <iostream>
#include <stdexcept>
#include <chrono> // Per misurare i tempi CPU
#include <string>

// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Header delle classi create
#include "WindowManager.h"
#include "RenderPipeline.h"
#include "ISimulation.h"
#include "SimulationCPU.h"
#include "Utils.h"
#include "SimulationFactory.h"
#include "Utils.h"
#include "InputHandler.h"

int main(int argc, char **argv)
{
    try
    {
        //// 1. FINESTRA E CONTESTO
        WindowManager windowManager("Particle Simulation");
        GLFWwindow *window = windowManager.getWindow();
        // Gestione input
        InputHandler inputHandler(window);
        // Contatore FPS
        Utils::FPSCounter fpsCounter;
        // Inizializza GLAD e config OpenGL
        if (!Utils::initializeGLAD()){
            throw std::runtime_error("Impossibile inizializzare GLAD."); // perche ritorno bool?
        }
        Utils::configureOpenGL();

        //// 3.  SIMULAZIONE
        int simWidth, simHeight;
        Utils::SimulationManager::getSimulationSize(Utils::SimulationResolution::HD_720, simWidth, simHeight);
        std::cout << "[Simulation] Dimensioni fisse: " << simWidth << " x " << simHeight << std::endl;
        // Decidi se usare CPU o GPU
        bool useGPU = true;
        std::cout << "[Simulation] Using " << (useGPU ? "GPU" : "CPU") << " version.\n";
        ISimulation *simulation = SimulationFactory::createSimulation(
            useGPU ? SimulationType::GPU : SimulationType::CPU,
            10000000, simWidth, simHeight);
        simulation->initialize();

        //// 4. RENDER PIPELINE
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();
        // (Facoltativo) se vuoi passare reference della simulazione per configurare, es:
        // renderPipeline.setSimulation(&simulation);

        //// 5. TIMESTEPPER
        Utils::TimestepManager timeStepper(1.0/60.0);
        // Inizializza con l'ora attuale
        double startTime = glfwGetTime();
        timeStepper.init(startTime);

        //// 6. LOOP PRINCIPALE
        while (!windowManager.shouldClose())
        {
            
            glfwPollEvents();
            inputHandler.update();

            double now = glfwGetTime();
            timeStepper.update(now);

            while(timeStepper.hasSteps()) {
                double dt = timeStepper.getStepDt();
                simulation->update((float)dt);
            }

            renderPipeline.render(*simulation, windowManager.getWidth(), windowManager.getHeight());
            // I valori stampati (o usati per modificare il titolo della finestra) sono tempi CPU, quindi se la GPU è impegnata su un calcolo interno e non eseguiamo un “glFinish” o uno “Sync/Wait”, potremmo misurare meno di quanto la GPU impiega davvero.
            // Per un vero benchmark GPU servirebbe l’uso di OpenGL timer queries (es. glGenQueries, glBeginQuery(GL_TIME_ELAPSED, queryID), glEndQuery(...), glGetQueryObjectui64v(...), ecc.).
            // Se hai glfwSwapInterval(1) attivo, la chiamata a swapBuffers() bloccherà a ~60 fps (o la frequenza del monitor), “nascondendo” parte del tuo tempo di rendering.
            // Se vuoi un framerate sbloccato, disattiva vsync (glfwSwapInterval(0)) e valuta un capping manuale se necessario.

            fpsCounter.update();
            if (fpsCounter.shouldRefreshTitle())
            {
                int fps = fpsCounter.getFPS();
            }

            windowManager.swapBuffers();
        }

        // cleanup automatico nel distruttore delle classi
        delete simulation;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return -1;
    }
}
