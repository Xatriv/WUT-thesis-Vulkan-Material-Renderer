
#include "app_config.h"

namespace vmr {

AppConfig::AppConfig(std::string configPath) {
    std::ifstream i(configPath);
    if (i.fail()){
        throw std::runtime_error("Could not open file: " + configPath);
    }
    nlohmann::json jsonConfig; 
    i >> jsonConfig;
    _sphereModelPath = jsonConfig["path"]["sphereModel"];
    _displayModelPath = jsonConfig["path"]["displayModel"];
    _modelTexturePath = jsonConfig["path"]["modelTexture"];
    _modelNormalMapPath = jsonConfig["path"]["modelNormalMap"];
    _modelVertexShaderPath = jsonConfig["path"]["modelVertexShader"];
    _modelFragmentShaderPath = jsonConfig["path"]["modelFragmentShader"];
    _lightVertexShaderPath = jsonConfig["path"]["lightVertexShader"];
    _lightFragmentShaderPath = jsonConfig["path"]["lightFragmentShader"];
    _windowWidth = jsonConfig["windowSize"]["width"];
    _windowHeight = jsonConfig["windowSize"]["height"];
    _cameraSpeed = jsonConfig["camera"]["speed"];
    _lightSpeed = jsonConfig["lightSpeed"];
    _cameraFront = glm::vec3(
        jsonConfig["camera"]["front"]["x"],
        jsonConfig["camera"]["front"]["y"],
        jsonConfig["camera"]["front"]["z"]           
    );
    _cameraUp = glm::vec3(
        jsonConfig["camera"]["up"]["x"],
        jsonConfig["camera"]["up"]["y"],
        jsonConfig["camera"]["up"]["z"]           
    );
    _lightPosition = glm::vec3(
        jsonConfig["lightPosition"]["x"],
        jsonConfig["lightPosition"]["y"],
        jsonConfig["lightPosition"]["z"]
    );
    _observerPosition = glm::vec3(
        jsonConfig["observerPosition"]["x"],
        jsonConfig["observerPosition"]["y"],
        jsonConfig["observerPosition"]["z"]
    );
    _lastX = _windowWidth / 2;
    _lastY = _windowHeight / 2;
}
}