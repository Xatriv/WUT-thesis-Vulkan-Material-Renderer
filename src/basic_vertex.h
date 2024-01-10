#pragma once

namespace vmr{
struct BasicVertex {
    glm::vec3 pos;
    glm::vec3 normal;


    bool operator==(const BasicVertex& other) const;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

};
}

