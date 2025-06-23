#pragma once

// Librerie OpenGL e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager(const char* title);
    ~WindowManager();

    GLFWwindow* getWindow() const;
    bool shouldClose() const;
    void handleInput();
    void swapBuffers();

    int getWidth() const;
    int getHeight() const;

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void toggleFullscreen();

    GLFWwindow* window;
    GLFWmonitor* monitor;
    int windowedX, windowedY, windowedWidth, windowedHeight;
    bool isFullscreen;
};
