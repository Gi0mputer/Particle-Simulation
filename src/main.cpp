#include <iostream> // Include per std::cerr e std::cout
#include <glad/glad.h> // Include per OpenGL
#include <GLFW/glfw3.h> // Include per GLFW
#include "Simulation.h" // Simulazione delle particelle
#include "Renderer.h" // Rendering delle particelle

int main() {
    // Inizializza GLFW
    if (!glfwInit()) {
        std::cerr << "Errore nell'inizializzazione di GLFW!" << std::endl;
        return -1;
    }

    // Configurazione del contesto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // OpenGL 4.5
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core Profile

    // Creazione della finestra
    GLFWwindow* window = glfwCreateWindow(800, 600, "Particle Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Errore nella creazione della finestra!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inizializza GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Errore nell'inizializzazione di GLAD!" << std::endl;
        return -1;
    }

    // Configura OpenGL
    glViewport(0, 0, 800, 600);
    glEnable(GL_PROGRAM_POINT_SIZE); // Abilita la modifica della dimensione dei punti
    glPointSize(5.0f); // Imposta la dimensione dei punti

    // Inizializza la simulazione e il renderer
    Simulation sim(1000, 800, 600); // 1000 particelle
    Renderer renderer(sim.getParticles()); // Renderer con le particelle iniziali

    // Loop principale
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Colore di sfondo
        glClear(GL_COLOR_BUFFER_BIT); // Pulisce il buffer dello schermo

        // Aggiorna la simulazione
        sim.update(0.01f);

        // Aggiorna i dati del buffer con le posizioni aggiornate
        renderer.updateBuffer(sim.getParticles());

        // Disegna le particelle
        renderer.draw();

        // Swap buffers e gestione degli eventi
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}