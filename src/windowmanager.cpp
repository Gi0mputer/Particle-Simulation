#include "windowmanager.h"
#include <iostream>
#include <stdexcept> // Per std::runtime_error

WindowManager::WindowManager(const char* title) : isFullscreen(false) {
    if (!glfwInit()) {
        throw std::runtime_error("Errore nell'inizializzazione di GLFW!");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Dimensioni iniziali della finestra (metà dello schermo)
    windowedWidth = mode->width / 2;
    windowedHeight = mode->height / 2;
    windowedX = mode->width / 4;  // Posizione centrale
    windowedY = mode->height / 4;

    window = glfwCreateWindow(windowedWidth, windowedHeight, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Errore nella creazione della finestra!");
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Salva la posizione attuale della finestra
    glfwGetWindowPos(window, &windowedX, &windowedY);

    // Associa l'oggetto WindowManager alla finestra
    glfwSetWindowUserPointer(window, this);
}

WindowManager::~WindowManager() {
    glfwDestroyWindow(window);
    glfwTerminate();
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
    glViewport(0, 0, width, height);
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
