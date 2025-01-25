#include "OpenGLUtils.h"
#include <iostream>
#include <stdexcept>

namespace OpenGLUtils
{
    bool initializeGLAD()
    {
        // L'idea è che la finestra e il context OpenGL siano già creati prima (con GLFW)
        // e che abbiamo chiamato glfwMakeContextCurrent(window)
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "[OpenGLUtils] Failed to initialize GLAD!\n";
            return false;
        }
        std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
        return true;
    }

    void configureOpenGL()
    {
        // Esempio di impostazioni di base
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Se vuoi disegnare punti "smooth"
        // glEnable(GL_PROGRAM_POINT_SIZE);

        // Se vuoi il depth test
        // glEnable(GL_DEPTH_TEST);
        //glPointSize(5.0f);

        // Stabilisci la viewport, se vuoi
        // (In molti casi si fa altrove, ad es. dopo aver creato la finestra)
        //glViewport(0, 0, simWidth, simHeight);

    }

    // void OpenGLUtils::clearScreen()
    // {
    //     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);
    // }

    // Se vuoi un callback di debug
    // void enableDebugCallback() { ... }
}
