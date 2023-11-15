#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <GLFW/glfw3.h>



namespace vmr{
struct Vertex {
    //TODO texCoord unnecessary for unicolored light model - this causes validation warning
    //TODO so does tangent and bitangent for objects without normal maps
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    bool operator==(const Vertex& other) const;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();

};
}

