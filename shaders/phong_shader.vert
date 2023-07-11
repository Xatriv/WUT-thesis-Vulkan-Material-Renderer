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

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec3 inputNormal;

layout(location = 0) out vec3 vertexPosition;
layout(location = 1) out vec3 vertexNormal;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inputPosition, 1.0);
    vertexPosition = (ubo.model * vec4(inputPosition, 1.0)).xyz;
    vertexNormal = (ubo.model * vec4(normalize(inputNormal), 1.0)).xyz;
}