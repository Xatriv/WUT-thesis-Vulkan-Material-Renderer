#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 lightModel;
    mat4 view;
    mat4 proj;
    vec3 rgb;
    vec3 position;
    vec3 lightPosition;
    int shininess;
} ubo;

layout(location = 0) in vec3 vertexPosition;

layout(location = 0) out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(0.7, 0.7, 0.2, 1.0);
}