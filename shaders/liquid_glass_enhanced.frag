#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform float time;
uniform float distortion;
uniform vec2 resolution;
uniform sampler2D backgroundTexture;
uniform sampler2D sdfTexture;

// Enhanced liquid glass with SDF and backdrop support - Based on Android implementation

// SDF functions
float sdCircle(vec2 p, float r) {
    return length(p) - r;
}

float sdBox(vec2 p, vec2 b) {
    vec2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// Liquid distortion
vec2 getLiquidDistortion(vec2 uv, float time) {
    vec2 distortion = vec2(0.0);
    
    // Multiple wave layers for realistic liquid effect
    distortion.x += sin(uv.y * 8.0 + time * 2.0) * 0.02;
    distortion.y += cos(uv.x * 6.0 + time * 1.5) * 0.02;
    
    distortion.x += sin(uv.y * 16.0 + time * 3.0) * 0.01;
    distortion.y += cos(uv.x * 12.0 + time * 2.2) * 0.01;
    
    return distortion * distortion;
}

// Edge detection based on SDF
float getEdgeFactor(float sdf, float edgeWidth) {
    return smoothstep(edgeWidth, -edgeWidth, sdf);
}

// Fresnel effect
float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Background blur approximation
vec3 blurBackground(vec2 uv, sampler2D tex, float blurSize) {
    vec3 color = vec3(0.0);
    float samples = 0.0;
    
    for(float x = -blurSize; x <= blurSize; x += 1.0) {
        for(float y = -blurSize; y <= blurSize; y += 1.0) {
            vec2 offset = vec2(x, y) / resolution;
            color += texture(tex, uv + offset).rgb;
            samples += 1.0;
        }
    }
    
    return color / samples;
}

void main() {
    vec2 uv = TexCoord;
    
    // Sample SDF from texture (generated from view hierarchy)
    float sdf = texture(sdfTexture, uv).r;
    
    // Get liquid distortion
    vec2 distortion = getLiquidDistortion(uv, time);
    vec2 distortedUV = uv + distortion;
    
    // Ensure UV coordinates stay within bounds
    distortedUV = clamp(distortedUV, 0.0, 1.0);
    
    // Sample background with distortion
    vec3 backgroundColor = texture(backgroundTexture, distortedUV).rgb;
    
    // Apply blur based on depth from SDF
    float blurAmount = smoothstep(0.0, 0.5, abs(sdf)) * 0.02;
    backgroundColor = blurBackground(distortedUV, backgroundTexture, blurAmount * 10.0);
    
    // Normal calculations for lighting
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    // Calculate edge factor for glass rim
    float edgeFactor = getEdgeFactor(sdf, 0.05);
    
    // Fresnel effect for glass reflection
    float fresnel = fresnelSchlick(max(dot(V, N), 0.0), 0.02);
    
    // Liquid thickness based on SDF
    float thickness = 1.0 - smoothstep(-0.1, 0.1, sdf);
    
    // Glass color with depth
    vec3 glassColor = vec3(0.7, 0.85, 1.0);
    vec3 absorptionColor = vec3(0.4, 0.6, 0.9);
    
    // Combine background with glass effect
    vec3 finalColor = mix(backgroundColor, glassColor * backgroundColor, thickness * 0.3);
    
    // Add fresnel reflection at edges
    finalColor = mix(finalColor, vec3(1.0), fresnel * edgeFactor);
    
    // Calculate alpha based on thickness and edge
    float alpha = 0.3 + thickness * 0.4 + fresnel * 0.3;
    alpha = clamp(alpha, 0.2, 0.9);
    
    FragColor = vec4(finalColor, alpha);
}