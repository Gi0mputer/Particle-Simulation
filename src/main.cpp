#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Header personalizzati
#include "openglutils.h"  
#include "windowmanager.h" 
#include "simulationmanager.h" 

#include "simulation.h"   
#include "renderer.h"     
#include "shaderprogram.h" 

#include "mode.h"        

int main(int argc, char** argv) {
    // Crea la finestra
    WindowManager windowManager("Particle Simulation");
    GLFWwindow* window = windowManager.getWindow();

    // Configura OpenGL
    OpenGLUtils::initializeGLAD();
    OpenGLUtils::configureOpenGL();

    // Calcola dimensioni simulazione
    int simWidth, simHeight;
    SimulationManager::getSimulationSize(SimulationResolution::HD, simWidth, simHeight);
    std::cout << "[Simulation] Dimensioni fisse: " << simWidth << " x " << simHeight << std::endl;

    // Inizializza simulazione e renderer
    Simulation simulation(100, simWidth, simHeight);
    Renderer renderer(simulation.getParticles(), simWidth, simHeight);
    ShaderProgram particleShader("shaders/basic.vert", "shaders/basic.frag");
    ShaderProgram trailShader("shaders/trail.vert", "shaders/trail.frag");

    // Main loop
    while (!windowManager.shouldClose()) {
        // Gestione input
        windowManager.handleInput();

        // Aggiorna la simulazione
        simulation.update(0.01f);

        // --- Passo 1: Aggiorna la texture delle tracce ---
        renderer.beginPingPong();
        trailShader.use();
        glBindTexture(GL_TEXTURE_2D, renderer.getCurrentTexture());
        glUniform2f(glGetUniformLocation(trailShader.getID(), "uScreenSize"), (float)simWidth, (float)simHeight);
        renderer.drawParticles();
        renderer.endPingPong();

        // Scambia i framebuffer per il ping-pong
        renderer.swapPingPong();

        // --- Passo 2: Disegna le particelle sullo schermo ---
        OpenGLUtils::clearScreen();
        particleShader.use();
        glUniform2f(glGetUniformLocation(particleShader.getID(), "uScreenSize"), (float)simWidth, (float)simHeight);
        glBindTexture(GL_TEXTURE_2D, renderer.getCurrentTexture());
        renderer.drawParticles();

        // Swap buffer e gestione eventi
        windowManager.swapBuffers();
    }

    return 0;
}
