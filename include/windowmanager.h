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
    void toggleFullscreen();
    bool getIsFullscreen() const { return isFullscreen; }
    void setFullscreen(bool enable);

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
    GLFWmonitor* monitor;
    int windowedX, windowedY, windowedWidth, windowedHeight;
    bool isFullscreen;
};
