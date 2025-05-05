#include <iostream>
#include <stdexcept>
#include <chrono> // Per misurare i tempi CPU
#include <string>

// Includiamo prima GLAD (che deve essere inizializzato prima di GLFW)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Include dei file header del progetto
#include "WindowManager.h"  // Gestisce la creazione della finestra e il contesto OpenGL
#include "RenderPipeline.h" // Gestisce i pass di rendering (inclusi effetti post-process e ping-pong)
#include "SimulationGPU.h"  // Simulazione particellare basata su compute shader
#include "Utils.h"          // Funzioni di utilità, configurazione OpenGL, gestione timestep e FPS
#include "InputHandler.h"   // Gestione degli input (mouse, tastiera, ecc.)

void CoordinateConversion(GLFWwindow* window, int simWidth, int simHeight, double* mx, double* my);

int main(int argc, char **argv)
{
    try
    {
        std::cout << "1. Inizializzazione finestra..." << std::endl;
        WindowManager windowManager("Particle Simulation");
        GLFWwindow *window = windowManager.getWindow();
        
        std::cout << "2. Inizializzazione input handler..." << std::endl;
        InputHandler inputHandler(window);
        
        std::cout << "3. Inizializzazione FPS counter..." << std::endl;
        Utils::FPSCounter fpsCounter;
        
        std::cout << "4. Inizializzazione GLAD..." << std::endl;
        if (!Utils::initializeGLAD())
        {
            throw std::runtime_error("Impossibile inizializzare GLAD.");
        }
        
        std::cout << "5. Configurazione OpenGL..." << std::endl;
        Utils::configureOpenGL();

        std::cout << "6. Inizializzazione dimensioni simulazione..." << std::endl;
        int simWidth, simHeight;
        Utils::SimulationManager::getSimulationSize(Utils::SimulationResolution::HD_720, simWidth, simHeight);
        std::cout << "[Simulation] Dimensioni: " << simWidth << " x " << simHeight << std::endl;

        std::cout << "7. Inizializzazione simulazione GPU..." << std::endl;
        SimulationGPU simulation(1 * 1000 * 1000, simWidth, simHeight);
        simulation.initialize();

        std::cout << "8. Inizializzazione render pipeline..." << std::endl;
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();

        std::cout << "9. Inizializzazione timestep..." << std::endl;
        Utils::TimestepManager timeStepper(1.0 / 60.0);
        double startTime = glfwGetTime();
        timeStepper.init(startTime);

        std::cout << "10. Inizio loop principale..." << std::endl;
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
                
                auto [mouseX, mouseY] = inputHandler.getMousePos();
                CoordinateConversion(window, simWidth, simHeight,  &mouseX, &mouseY);

                simulation.update(dt, mouseX, mouseY, inputHandler.isLeftClicked());
            }

            // Esegue il rendering della scena: la pipeline legge i dati aggiornati dalla simulazione GPU
            renderPipeline.render(simulation, windowManager.getWidth(), windowManager.getHeight());

            fpsCounter.update();
            if (fpsCounter.shouldRefreshTitle())
            {
                int fps = fpsCounter.getFPS();
                std::string newTitle = "Particle Simulation - FPS: " + std::to_string(fps);
                // windowManager.setWindowTitle(newTitle); // da implementare
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

// Funzioni di utilità, configurazione OpenGL, gestione timestep e FPS
void CoordinateConversion(GLFWwindow* window, int simWidth, int simHeight, double* mx, double* my) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    *my = h - *my; // invert y
    *mx = (*mx / w) * simWidth;
    *my = (*my / h) * simHeight;
}