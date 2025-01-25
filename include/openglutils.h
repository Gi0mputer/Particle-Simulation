#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace OpenGLUtils
{
    // Inizializza GLAD
    bool initializeGLAD();

    // Configura alcuni stati di base di OpenGL
    void configureOpenGL();

    // (Facoltativo) Se vuoi funzioni di checking error, debug, ecc.
    // void enableDebugCallback();
}

