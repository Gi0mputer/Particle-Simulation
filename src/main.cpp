#include <iostream>
#include <stdexcept>
#include <chrono> // Per misurare i tempi CPU
#include <string>

// Includiamo prima GLAD (che deve essere inizializzato prima di GLFW)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Include dei file header del progetto
#include "WindowManager.h"     // Gestisce la creazione della finestra e il contesto OpenGL
#include "RenderPipeline.h"    // Gestisce i pass di rendering (inclusi effetti post-process e ping-pong)
#include "SimulationGPU.h"     // Simulazione particellare basata su compute shader
#include "Utils.h"             // Funzioni di utilità, configurazione OpenGL, gestione timestep e FPS
#include "InputHandler.h"      // Gestione degli input (mouse, tastiera, ecc.)


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

        //// 3. SIMULAZIONE
        int simWidth, simHeight;
        Utils::SimulationManager::getSimulationSize(Utils::SimulationResolution::HD_720, simWidth, simHeight);
        std::cout << "[Simulation] Dimensioni fisse: " << simWidth << " x " << simHeight << std::endl;
        // Crea e inizializza la simulazione GPU.
        // La classe SimulationGPU si occupa di gestire i buffer delle particelle e di aggiornare
        // la simulazione tramite compute shader.
        SimulationGPU simulation(100000, simWidth, simHeight); // Esempio: 10.000 particelle
        simulation.initialize();

        //// 4. RENDER PIPELINE
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();
       
        //// 5. TIMESTEPPER
        Utils::TimestepManager timeStepper(1.0/60.0);
        // Inizializza con l'ora attuale
        double startTime = glfwGetTime();
        timeStepper.init(startTime);

        //// 6. LOOP PRINCIPALE
        while (!windowManager.shouldClose())
        {
            // Gestione degli eventi (input, window events, ecc.)
            glfwPollEvents();
            inputHandler.update();

            // Aggiorna il gestore dei timestep con il tempo attuale
            double currentTime = glfwGetTime();
            timeStepper.update(currentTime);

            // Esegue gli aggiornamenti della simulazione a passo fisso
            while (timeStepper.hasSteps())
            {
                double dt = timeStepper.getStepDt();
                simulation.update(static_cast<float>(dt));
            }

            // Esegue il rendering della scena: la pipeline legge i dati aggiornati dalla simulazione GPU
            renderPipeline.render(simulation, windowManager.getWidth(), windowManager.getHeight());
             
            fpsCounter.update();
            if (fpsCounter.shouldRefreshTitle())
            {
                int fps = fpsCounter.getFPS();
                std::string newTitle = "Particle Simulation - FPS: " + std::to_string(fps);
                //windowManager.setWindowTitle(newTitle); // da implementare
            }
            // Swap dei buffer per visualizzare il frame renderizzato
            windowManager.swapBuffers();
        }

        // 7. PULIZIA E USCITA
        // Le risorse vengono liberate automaticamente dai destructors delle classi utilizzate.
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return -1;
    }
}



