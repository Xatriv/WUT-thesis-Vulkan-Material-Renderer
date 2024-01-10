#define GLFW_INCLUDE_VULKAN

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <GLFW/glfw3.h>

#include "basic_vertex.h"


namespace vmr{

bool BasicVertex::operator==(const BasicVertex& other) const {
    return pos == other.pos && normal == other.normal;
}

VkVertexInputBindingDescription BasicVertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(BasicVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> BasicVertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(BasicVertex, pos);
    
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(BasicVertex, normal);

    return attributeDescriptions;
}

}
