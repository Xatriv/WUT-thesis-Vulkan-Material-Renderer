#include "window.h"


namespace vmr{

Window::Window(std::string name, AppConfig* config) : _appConfig(config){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //don't create opengl context

    _window = glfwCreateWindow(_appConfig->windowWidth(), _appConfig->windowHeight(), name.c_str(), nullptr, nullptr);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_window, mouseMovementCallback);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

Window::~Window(){
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Window::handleKeystrokes(){ 
    if (isPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(_window, true);
    if (isPressed(GLFW_KEY_1)) _appConfig->movementMode(MOVEMENT_CAMERA);
    if (isPressed(GLFW_KEY_2)) _appConfig->movementMode(MOVEMENT_LIGHT);
    if (_appConfig->movementMode() == MOVEMENT_LIGHT){
        if (isPressed(GLFW_KEY_W)) _appConfig->lightPosition().x += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_S)) _appConfig->lightPosition().x += -_appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_A)) _appConfig->lightPosition().y += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_D)) _appConfig->lightPosition().y += -_appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_Q)) _appConfig->lightPosition().z += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_E)) _appConfig->lightPosition().z += -_appConfig->lightSpeed();
    } else if (_appConfig->movementMode() == MOVEMENT_CAMERA) {
        if (isPressed(GLFW_KEY_W)) _appConfig->observerPosition() += _appConfig->cameraSpeed() * _appConfig->cameraFront();
        if (isPressed(GLFW_KEY_S)) _appConfig->observerPosition() -= _appConfig->cameraSpeed() * _appConfig->cameraFront();
        if (isPressed(GLFW_KEY_A)) _appConfig->observerPosition() -= glm::normalize(glm::cross(_appConfig->cameraFront(), _appConfig->cameraUp())) * _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_D)) _appConfig->observerPosition() += glm::normalize(glm::cross(_appConfig->cameraFront(), _appConfig->cameraUp())) * _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_Q)) _appConfig->observerPosition().z += _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_E)) _appConfig->observerPosition().z += -_appConfig->cameraSpeed();
    }
}

bool Window::isPressed(int key) {
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}


void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}

void Window::mouseMovementCallback(GLFWwindow* window, double xpos, double ypos){
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    if (app->_appConfig->firstMouse()) {
        app->_appConfig->lastX(xpos);
        app->_appConfig->lastY(ypos);
        app->_appConfig->firstMouse(false); 
    }

    float xoffset = xpos - app->_appConfig->lastX();
    float yoffset = ypos - app->_appConfig->lastY(); 
    app->_appConfig->lastX(xpos);
    app->_appConfig->lastY(ypos);

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    app->_appConfig->yaw(app->_appConfig->yaw() - xoffset);
    app->_appConfig->pitch(app->_appConfig->pitch() - yoffset);

    app->_appConfig->pitch(std::clamp(app->_appConfig->pitch(), -89.0f, 89.0f));
    
    app->_appConfig->cameraFront().x = cos(glm::radians(app->_appConfig->yaw())) * cos(glm::radians(app->_appConfig->pitch()));
    app->_appConfig->cameraFront().y = sin(glm::radians(app->_appConfig->yaw())) * cos(glm::radians(app->_appConfig->pitch()));
    app->_appConfig->cameraFront().z = sin(glm::radians(app->_appConfig->pitch()));
}  

}