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
layout(location = 3) in vec3 inputTangent;
layout(location = 4) in vec3 inputBitangent;  

layout(location = 0) out vec3 vertexPosition;
layout(location = 1) out vec3 vertexNormal;
layout(location = 2) out vec2 vertexTexCoord;
layout(location = 3) out mat3 TBNMatrix;

void main() {
    vec3 T = normalize(vec3(ubo.model * vec4(inputTangent,   0.0)));
    vec3 B = normalize(vec3(ubo.model * vec4(inputBitangent, 0.0)));
    vec3 N = normalize(vec3(ubo.model * vec4(inputNormal,    0.0)));
    TBNMatrix = mat3(T, B, N);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inputPosition, 1.0);
    vertexPosition = (ubo.model * vec4(inputPosition, 1.0)).xyz;
    vertexNormal = (ubo.model * vec4(normalize(inputNormal), 1.0)).xyz;
    vertexTexCoord = inputTextureCoord;
}