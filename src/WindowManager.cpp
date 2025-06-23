#include "WindowManager.h"
#include <iostream>
#include <stdexcept> // Per std::runtime_error

WindowManager::WindowManager(const std::string& title) : isFullscreen(false) {
    std::cout << "[WindowManager] Inizializzazione..." << std::endl;
    
    if (!glfwInit()) {
        std::cout << "[WindowManager] ERRORE: Impossibile inizializzare GLFW" << std::endl;
        throw std::runtime_error("Impossibile inizializzare GLFW");
    }

    std::cout << "[WindowManager] Configurazione finestra..." << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Dimensioni iniziali della finestra (metà dello schermo)
    windowedWidth = mode->width / 2;
    windowedHeight = mode->height / 2;
    windowedX = mode->width / 4;  // Posizione centrale
    windowedY = mode->height / 4;

    std::cout << "[WindowManager] Creazione finestra..." << std::endl;
    window = glfwCreateWindow(windowedWidth, windowedHeight, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cout << "[WindowManager] ERRORE: Impossibile creare la finestra" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Impossibile creare la finestra");
    }

    std::cout << "[WindowManager] Impostazione contesto OpenGL..." << std::endl;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Salva la posizione attuale della finestra
    glfwGetWindowPos(window, &windowedX, &windowedY);

    // Associa l'oggetto WindowManager alla finestra
    glfwSetWindowUserPointer(window, this);

    std::cout << "[WindowManager] Inizializzazione completata" << std::endl;
}

WindowManager::~WindowManager() {
    std::cout << "[WindowManager] Pulizia risorse..." << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "[WindowManager] Risorse pulite" << std::endl;
}

GLFWwindow* WindowManager::getWindow() const {
    return window;
}

bool WindowManager::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void WindowManager::handleInput() {
    static bool f11Pressed = false; // Per evitare pressioni multiple

    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if (!f11Pressed) {
            toggleFullscreen();
            f11Pressed = true;
        }
    } else {
        f11Pressed = false;
    }
}

void WindowManager::swapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void WindowManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "[WindowManager] Ridimensionamento finestra: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);

    WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    wm->windowedWidth = width;
    wm->windowedHeight = height;
}

void WindowManager::toggleFullscreen() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (isFullscreen) {
        // Torna a finestra
        glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
    } else {
        // Salva dimensioni attuali della finestra
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        // Passa a fullscreen
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    isFullscreen = !isFullscreen;
}

int WindowManager::getWidth() const {
    return windowedWidth;
}
int WindowManager::getHeight() const {
    return windowedHeight;
}