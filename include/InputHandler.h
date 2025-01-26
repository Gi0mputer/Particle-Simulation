#pragma once

#include <utility>

// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class InputHandler {
public:
    InputHandler(GLFWwindow* win);
    void update();

    bool isLeftClicked() const;
    std::pair<double,double> getMousePos() const;

    
private:
    GLFWwindow* window;
    bool leftPressed;
    double mouseX, mouseY;
};
