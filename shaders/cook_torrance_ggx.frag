#version 450

#define PI 3.14159265

 
const float roughness = 0.5;
const float IOR = 1.5;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 rgb;
    vec3 position;
    vec3 lightPosition;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord ;

layout(location = 0) out vec4 fragmentColor;

float chi(float v)
{
    return v > 0 ? 1 : 0;
}


// Cook-Torrance Specular
vec4 rs() {

    vec3 N = normalize(vertexNormal);
    vec3 V = normalize(ubo.position - vertexPosition);
    vec3 L = normalize(ubo.lightPosition - vertexPosition);
    vec3 H = normalize(V + L); //Half-vector, between viewer and the light

    float HdotN = max(0.0, dot(H, N));
    float NdotV = max(0.0, dot(N, V));
    float NdotL = max(0.0, dot(N, L));
    float HdotL = max(0.0, dot(H, L));
    float HdotV = max(0.0, dot(H, V));

    float alpha = roughness * roughness;
    float HdotN2 = HdotN * HdotN;
    float alpha2 = alpha;
    float dDenominator =  1 + HdotN2 * ( alpha2 - 1 );
    float D = alpha2 / (PI * dDenominator * dDenominator); 
    //GGX distribution

    float F0 = pow((IOR - 1.0) / (IOR + 1.0), 2);
    float F = F0 + (1.0 - F0) * pow(1.0 - dot(H, V), 5);
    //Schlick-Fresnel

    float G = 0.5 / mix(2 * HdotL * HdotV, HdotL + HdotV, roughness) * chi(NdotL); //TODO too dim for rougness==1.0
    // Unreal G_GGX approximation

    float brdf = G * F * D;

    float kd = F0;
    float ks = 1 - kd;

    float sinT = sqrt( 1 - NdotL * NdotL);

    // return ks * brdf * sinT + kd * ubo.rgb * NdotL;
    return ks * brdf * sinT +  texture(texSampler, vertexTexCoord) * NdotL;

}


void main() {
    vec4 total = rs();
    
    fragmentColor = total;
}