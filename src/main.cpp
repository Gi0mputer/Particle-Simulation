#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "simulation.h"    // Simulation class
#include "renderer.h"      // VAO/VBO management
#include "shaderprogram.h" // Shader program management
#include "mode.h"          // Enum class SimulationMode

int main(int argc, char **argv)
{
    // 1) Inizializza GLFW
    if (!glfwInit())
    {
        std::cerr << "Errore nell'inizializzazione di GLFW!" << std::endl;
        return -1;
    }

    // 2) Configura OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3) Crea la finestra
    GLFWwindow *window = glfwCreateWindow(800, 600, "Particle Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Errore nella creazione della finestra!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 4) Inizializza GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Errore nell'inizializzazione di GLAD!" << std::endl;
        return -1;
    }

    // 5) Viewport e altre impostazioni
    glViewport(0, 0, 800, 600);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);

    // Imposta la modalità (per ora SOLO "SIMPLE")
    SimulationMode mode = SimulationMode::SIMPLE;

    // 6) Inizializza la simulazione su CPU
    Simulation sim(1000, 800, 600);

    // 7) Inizializza il renderer (VAO/VBO)
    Renderer renderer(sim.getParticles());

    // 8) Crea uno shader program base (per disegnare punti)
    ShaderProgram myShader("shaders/basic.vert", "shaders/basic.frag");

    // 9) Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 9a) Aggiorna la simulazione (CPU)
        sim.update(0.01f);

        // 9b) Aggiorna i dati del buffer
        renderer.updateBuffer(sim.getParticles());

        // 9c) Usa lo shader banale
        myShader.use();

        // 9d) Disegna i punti
        renderer.draw();

        // 9e) Swap buffers e poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 10) Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
