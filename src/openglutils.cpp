#include "openglutils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void OpenGLUtils::initializeGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Errore nell'inizializzazione di GLAD!");
    }
    std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
}

void OpenGLUtils::configureOpenGL() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);
}

void OpenGLUtils::clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
