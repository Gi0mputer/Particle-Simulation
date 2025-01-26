#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow* win)
    : window(win), leftPressed(false), mouseX(0), mouseY(0)
{}

void InputHandler::update()
{
    // check button
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(state == GLFW_PRESS && !leftPressed) {
        leftPressed = true;
    } else if(state == GLFW_RELEASE) {
        leftPressed = false;
    }
    // mouse pos
    glfwGetCursorPos(window, &mouseX, &mouseY);
}

bool InputHandler::isLeftClicked() const {
    return leftPressed;
}

std::pair<double,double> InputHandler::getMousePos() const {
    return {mouseX, mouseY};
}
