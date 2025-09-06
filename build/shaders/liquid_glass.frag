#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

// 背景纹理
uniform sampler2D backgroundTexture;
// 时间
uniform float time;
// 扭曲强度
uniform float distortion;
uniform vec2 resolution;

// 玻璃物理参数
uniform float refractiveIndex = 1.5;  // 玻璃的折射率
uniform float edgeBrightness = 2.0;     // 边缘亮度
uniform float fresnelPower = 3.0;       // 菲涅尔指数
uniform float thickness = 0.1;          // 玻璃厚度

// 计算球体遮罩和法线
vec4 getSphereData(vec2 uv) {
    vec2 center = vec2(0.5, 0.5);
    float radius = 0.4;
    float dist = distance(uv, center);
    
    // 计算球体遮罩
    float mask = 1.0 - smoothstep(radius - 0.02, radius + 0.02, dist);
    
    // 计算球面法线
    vec3 normal = vec3(0.0);
    if (dist < radius) {
        float z = sqrt(radius * radius - dist * dist);
        normal = normalize(vec3(uv - center, z));
    }
    
    return vec4(normal, mask);
}

// 基于Navier-Stokes的简化流体扭曲
vec2 calculateFluidDistortion(vec2 uv, vec3 normal, vec3 viewDir, float time) {
    // 物理折射计算
    vec3 refractedDir = refract(viewDir, normal, 1.0 / refractiveIndex);
    vec2 distortionOffset = refractedDir.xy * distortion * 0.1;
    
    // 多分辨率噪声纹理模拟
    vec2 flowUV = uv * 2.0 - 1.0;
    float wave1 = sin(flowUV.x * 6.0 + flowUV.y * 4.0 + time * 1.5) * 0.008;
    float wave2 = sin(flowUV.x * 12.0 - flowUV.y * 8.0 + time * 2.2) * 0.005;
    float wave3 = cos(flowUV.x * 18.0 + flowUV.y * 12.0 + time * 0.8) * 0.003;
    
    // 涡旋效果
    vec2 vortex = vec2(
        sin(flowUV.y * 3.0 + time * 0.5) * 0.01,
        cos(flowUV.x * 3.0 + time * 0.7) * 0.01
    );
    
    // 组合所有效果
    vec2 fluidDistortion = distortionOffset + vec2(wave1 + wave2, wave2 + wave3) + vortex;
    
    // 时间平滑处理
    float smoothFactor = 0.95; // 平滑系数
    return mix(uv, uv + fluidDistortion, smoothFactor);
}

// 计算边缘散射效果
vec3 calculateEdgeScattering(vec2 uv, vec4 sphereData) {
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float radius = 0.4;
    
    // 计算边缘距离
    float edgeDist = abs(dist - radius) / thickness;
    
    // 菲涅尔边缘高亮
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    float fresnel = pow(1.0 - abs(dot(normal, viewDir)), fresnelPower);
    
    // 边缘散射光
    float edgeGlow = exp(-edgeDist * edgeDist * 10.0);
    vec3 edgeColor = vec3(0.8, 0.9, 1.0) * edgeGlow * edgeBrightness;
    
    // 厚度相关的透明度
    float thicknessAlpha = 1.0 - (dist / radius) * thickness;
    
    return edgeColor * fresnel * mask;
}

// PBR边缘散射效果
vec3 calculatePBRScattering(vec2 uv, vec4 sphereData, vec3 viewDir) {
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float radius = 0.4;
    
    // 精确的边缘距离计算
    float edgeDist = smoothstep(radius - 0.05, radius + 0.05, dist);
    
    // 基于物理的菲涅尔效应
    float F0 = 0.02; // 基础反射率
    float cosTheta = max(dot(normal, viewDir), 0.0);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    
    // 次表面散射
    float sss = exp(-edgeDist * 8.极0) * (1.0 - edgeDist);
    vec3 subsurfaceColor = vec3(0.4, 0.6, 0.8) * sss * 2.0;
    
    // 环境光遮蔽
    float ao = 1.0 - smoothstep(0.3, 0.4, dist);
    
    // 边缘光晕效果
    float edgeGlow = exp(-edgeDist * edgeDist * 15.0) * fresnel;
    vec3 edgeColor = vec3(0.7, 0.85, 1.0) * edgeGlow * edgeBrightness;
    
    return (edgeColor + subsurfaceColor) * fresnel * ao * mask;
}

// 计算玻璃透明度
float calculateGlassAlpha(vec2 uv, vec4 sphereData) {
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    
    // 基于视角的透明度
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    float fresnel = pow(1.0 - abs(dot(normal, viewDir)), 2.0);
    
    // 边缘更透明
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float radius = 0.4;
    float edgeFade = smoothstep(radius, radius - 0.1, dist);
    
    return mix(0.9, 0.3, fresnel * edgeFade) * mask;
}

void main() {
    vec2 uv = TexCoord;
    
    // 获取球体数据（法线和遮罩）
    vec4 sphereData = getSphereData(uv);
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    
    if (mask < 0.01) {
        // 完全透明区域显示原始背景
        FragColor = texture(backgroundTexture, uv);
        return;
    }
    
    // 计算视角方向
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    
    // 计算流体扭曲的UV坐标
    vec2 distortedUV = calculateFluidDistortion(uv, normal, viewDir, time);
    
    // 采样扭曲后的背景
    vec4 backgroundColor = texture(backgroundTexture, distortedUV);
    
    // 计算PBR散射效果
    vec3 scattering = calculatePBRScattering(uv, sphereData, viewDir);
    
    // 计算玻璃透明度
    float glassAlpha = calculateGlassAlpha(uv, sphereData);
    
    // 玻璃基色和吸收
    vec3 glassTint = vec3(0.95, 0.98, 1.0);
    vec3 absorption = exp(-vec3(0.1, 0.2, 0.3) * thickness * 2.0);
    
    // 最终颜色合成
    vec3 finalColor = backgroundColor.rgb * glassTint * absorption + scattering;
    
    FragColor = vec4(finalColor, glassAlpha);
}