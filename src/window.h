#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <fstream>

#include "app_config.h"

namespace vmr{

class Window{
private:
    GLFWwindow* _window;
    AppConfig* _appConfig;
    bool _framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
    bool isPressed(int key);
    
public:
    Window(std::string name, AppConfig* config);
    ~Window();
    GLFWwindow* window() {return _window; }
    bool& framebufferResized()       { return _framebufferResized; }
    const bool& framebufferResized() const { return _framebufferResized; }

    void handleKeystrokes();
    void pollEvents();
    bool shouldClose();

};
}