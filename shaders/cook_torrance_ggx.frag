#version 450

#define PI 3.14159265

 
const float roughness = 0.2;
const float IOR = 0.5;
const float kd = 0.5;


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

float saturate(float value) {
    return clamp(value, 0.0, 1.0);
}

// Cook-Torrance Specular
float rs() {

    vec3 N = vertexNormal;
    vec3 V = normalize(ubo.position - vertexPosition);
    vec3 L = normalize(ubo.lightPosition - ubo.position);
    vec3 H = normalize(V + L); //Half-vector, between viewer and the light

    float HdotN = saturate(dot(H, N));
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float VdotH = saturate(dot(V, H));
    float LdotH = saturate(dot(L, H));

    float alpha = pow(roughness, 2);
    float dPower = 2 / pow(roughness, 2) - 2; //modified formula with roughness in place of alpha
    float viewAttenuation = 2 * HdotN * NdotV / VdotH;
    float lightAttenuation = 2 * HdotN * NdotL / LdotH; // maybe VdotH instead of LdotH?
    float F0 = abs(pow(IOR - 1, 2) / pow(IOR + 1, 2));

    float D = 1 / (PI * pow(alpha, 2)) * pow(HdotN, dPower);
    float G = min(1, min(viewAttenuation, lightAttenuation));
    float F = F0 + (1 - F0) * pow(1 - VdotH, 5);

    return D * G * F / (4 * NdotL * NdotV);
}


void main() {
    float spec = rs();
    fragmentColor = vec4(spec, spec, spec, 1.0);
}