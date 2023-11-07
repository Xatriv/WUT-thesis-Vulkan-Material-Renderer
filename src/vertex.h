#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <GLFW/glfw3.h>



namespace vmr{
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

};
}

