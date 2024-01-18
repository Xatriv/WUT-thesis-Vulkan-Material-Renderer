#pragma once

#include <fstream>
#include <json.hpp>
#include <glm/glm.hpp>

#define MOVEMENT_CAMERA 0
#define MOVEMENT_LIGHT 1

namespace vmr {
class AppConfig {
public:
    AppConfig(std::string configPath);

    std::string sphereModelPath()           const { return _sphereModelPath; }
    std::string displayModelPath()          const { return _displayModelPath; }
    std::string modelTexturePath()          const { return _modelTexturePath; }
    std::string modelNormalMapPath()        const { return _modelNormalMapPath; }
    std::string modelVertexShaderPath()     const { return _modelVertexShaderPath; }
    std::string modelFragmentShaderPath()   const { return _modelFragmentShaderPath; }
    std::string lightVertexShaderPath()     const { return _lightVertexShaderPath; }
    std::string lightFragmentShaderPath()   const { return _lightFragmentShaderPath; }
    int windowWidth()                       const { return _windowWidth; }
    int windowHeight()                      const { return _windowHeight; }
    float cameraSpeed()                     const { return _cameraSpeed; }
    float lightSpeed()                      const { return _lightSpeed; }
    glm::vec3 cameraFront()                 const { return _cameraFront; }
    glm::vec3 cameraUp()                    const { return _cameraUp; }
    glm::vec3 lightPosition()               const { return _lightPosition; } 
    glm::vec3 observerPosition()            const { return _observerPosition; }
    int movementMode()                      const { return _movementMode; }
    bool firstMouse()                       const { return _firstMouse; }
    double lastX()                          const { return _lastX; }
    double lastY()                          const { return _lastY; }
    float pitch()                           const { return _pitch; }
    float yaw()                             const { return _yaw; }

    glm::vec3 & lightPosition()             { return _lightPosition; }
    glm::vec3 & observerPosition()          { return _observerPosition; }
    glm::vec3 & cameraFront()               { return _cameraFront; }

    void movementMode(int newMovementMode)          { _movementMode = std::move(newMovementMode); }
    void firstMouse(bool newFirstMouse)             { _firstMouse = std::move(newFirstMouse); }
    void lastX(double newLastX)                     { _lastX = std::move(newLastX); }
    void lastY(double newLastY)                     { _lastY = std::move(newLastY); }
    void pitch(float newPitch)                      { _pitch = std::move(newPitch); }
    void yaw(float newYaw)                          { _yaw = std::move(newYaw); }

private:
    std::string _sphereModelPath;
    std::string _displayModelPath;
    std::string _modelTexturePath;
    std::string _modelNormalMapPath;
    std::string _modelVertexShaderPath;
    std::string _modelFragmentShaderPath;
    std::string _lightVertexShaderPath;
    std::string _lightFragmentShaderPath;
    int _windowWidth;
    int _windowHeight;
    float _cameraSpeed;
    float _lightSpeed;
    glm::vec3 _cameraFront;
    glm::vec3 _cameraUp;
    glm::vec3 _lightPosition;
    glm::vec3 _observerPosition;
    int _movementMode = MOVEMENT_CAMERA;
    bool _firstMouse = true;
    double _lastX;
    double _lastY;
    float _pitch=0.0f;
    float _yaw=0.0f;
};
}
