#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 rgb;
    vec3 position;
    vec3 lightPosition;
} ubo;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec3 inputNormal;
layout(location = 2) in vec2 inputTextureCoord;

layout(location = 0) out vec3 vertexPosition;
layout(location = 1) out vec3 vertexNormal;
layout(location = 2) out vec2 vertexTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inputPosition, 1.0);
    vertexPosition = (ubo.model * vec4(inputPosition, 1.0)).xyz;
    vertexNormal = (ubo.model * vec4(normalize(inputNormal), 1.0)).xyz;
    vertexTexCoord = inputTextureCoord;
}