#version 450

const vec3 lightColor = vec3(0.6, 0.6, 0.6);
const float ambientReflectionConstant = 0.2;
const float diffuseReflectionConstant = 0.5;
const float specularReflectionConstant = 1.0;
const int shininess = 8;



layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 rgb;
    vec3 position;
    vec3 lightPosition;
} ubo;


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 0) out vec4 fragmentColor;

void main()
{
    vec3 fragmentColor3 = ambientReflectionConstant * lightColor;

    fragmentColor3 += lightColor * max(dot(normalize(ubo.lightPosition-vertexPosition), vertexNormal), 0) * diffuseReflectionConstant;


    vec3 observerOrientedVector = normalize(ubo.position - vertexPosition);
    vec3 specularVector = reflect(-normalize(ubo.lightPosition-vertexPosition), normalize(vertexNormal));
    fragmentColor3 += specularReflectionConstant * pow(max(dot(observerOrientedVector, specularVector),0), shininess) * lightColor;

    fragmentColor = vec4(fragmentColor3 * ubo.rgb, 1.0);
}