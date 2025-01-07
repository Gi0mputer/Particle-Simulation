#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager(const char* title);
    ~WindowManager();

    GLFWwindow* getWindow() const;
    bool shouldClose() const;
    void handleInput();
    void swapBuffers();

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void toggleFullscreen();

    GLFWwindow* window;
    GLFWmonitor* monitor;
    int windowedX, windowedY, windowedWidth, windowedHeight;
    bool isFullscreen;
};

#endif // WINDOW_MANAGER_H
