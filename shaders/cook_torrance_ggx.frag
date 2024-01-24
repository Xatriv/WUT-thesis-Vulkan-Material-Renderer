#version 450

#define PI 3.14159265

 
const float roughness = 0.5;
const float IOR = 1.5;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 position;
    vec3 lightPosition;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalMapSampler; 

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in mat3 TBNMatrix;


layout(location = 0) out vec4 fragmentColor;

float chi(float v)
{
    return v > 0 ? 1 : (tanh(8*v) + 1);
}


//Simplified reverse Screen-space Ambient Occlusion
float thickness(float maximumDistance, float falloff) {
    const int sampleCount = 7;
    float ao = 0.0;

    for (int i=0; i<sampleCount; i++) {
        ao += fract(sin(i/10))*maximumDistance;
    }

    return 1.0 - ao/float(sampleCount);
}


// Barre-Brisebois Approximated Subsurface Scattering
vec3 sss2(vec3 diffuse, vec3 N, vec3 V, vec3 L) { 
    float distortion = 1.0;
    vec3 lightVec = L + N * distortion;
    float power = 12.0;
    float powDot = pow(clamp(dot(V, lightVec), 0.0, 1.0), power);
    float lightAttenuation = 1 / pow(length(ubo.lightPosition-vertexPosition) / 2, 3.0); 
    float ambient = 1.0;
    float translucency = lightAttenuation * (powDot + ambient) * thickness(6.0, 0.6);
    float lightDiffuse = 0.0005;
    return diffuse * lightDiffuse * translucency;
}

// Cook-Torrance Specular
vec4 rs() {
    vec4 diffuseTex = texture(texSampler, vertexTexCoord);
    vec4 normalTex = texture(normalMapSampler, vertexTexCoord);

    vec3 N = normalize(normalTex.rgb);
    N = normalize(TBNMatrix * N);
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

    float chiOfActualNormal = chi(dot(normalize(N), L));
    float G = 0.5 / mix(2 * HdotL * HdotV, HdotL + HdotV, roughness) * chiOfActualNormal;
    // Unreal G_GGX approximation

    float brdf = G * F * D;

    float kd = F0;
    float ks = 1 - kd;

    float sinT = sqrt( 1 - NdotL * NdotL);

    vec4 ka = vec4(0.05, 0.05, 0.05, 1.0);

    vec4 fin = (ks * brdf * sinT + NdotL + ka) * diffuseTex;   // color-corrected specular
    vec3 sssCol = sss2(diffuseTex.rgb, N, V, L);
    sssCol = clamp(sssCol, 0.0, 1.0);
    return vec4(sssCol, 1.0) + fin ;
}


void main() {
    vec4 total = rs();
    
    fragmentColor = total;
}