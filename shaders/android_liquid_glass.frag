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
uniform float refractiveIndex = 1.5;
uniform float edgeBrightness = 2.0;
uniform float fresnelPower = 3.0;
uniform float thickness = 0.1;

// 基于Android实现的增强功能
uniform float blurRadius = 0.01;
uniform float edgeWidth = 0.05;

// SDF计算（基于Android的8ssedt算法简化）
float sdCircle(vec2 p, vec2 center, float radius) {
    return length(p - center) - radius;
}

// 高斯模糊背景（Android背景捕获技术）
vec3 gaussianBackgroundBlur(vec2 uv, sampler2D tex, float radius) {
    vec3 color = vec3(0.0);
    float totalWeight = 0.0;
    
    // 5x5高斯核，基于Android的模糊实现
    for(int i = -2; i <= 2; i++) {
        for(int j = -2; j <= 2; j++) {
            vec2 offset = vec2(float(i), float(j)) * radius / resolution;
            float weight = exp(-float(i*i + j*j) / 4.0);
            color += texture(tex, uv + offset).rgb * weight;
            totalWeight += weight;
        }
    }
    
    return color / totalWeight;
}

// 边缘检测（基于SDF梯度）
float getEdgeFactor(float sdf) {
    float edge = abs(dFdx(sdf)) + abs(dFdy(sdf));
    return smoothstep(0.0, 0.1, edge);
}

// 菲涅尔效果（基于Android的反射模型）
float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// 液态扭曲（基于Android的波纹算法）
vec2 getLiquidDistortion(vec2 uv, float time, float intensity) {
    vec2 distortion = vec2(0.0);
    
    // 多层波纹，模拟Android的液态效果
    float wave1 = sin(uv.x * 12.0 + uv.y * 8.0 + time * 2.5) * 0.015;
    float wave2 = cos(uv.x * 8.0 - uv.y * 12.0 + time * 1.8) * 0.01;
    float wave3 = sin(uv.x * 20.0 + uv.y * 15.0 + time * 3.2) * 0.008;
    
    distortion.x = (wave1 + wave2 + wave3) * intensity;
    distortion.y = (wave1 * 0.8 + wave2 * 1.2 + wave3 * 0.6) * intensity;
    
    return distortion;
}

// 计算球体数据（法线和遮罩）
vec4 getSphereData(vec2 uv) {
    vec2 center = vec2(0.5, 0.5);
    float radius = 0.4;
    float sdf = sdCircle(uv, center, radius);
    
    // 计算遮罩（基于SDF）
    float mask = 1.0 - smoothstep(-0.02, 0.02, sdf);
    
    // 计算法线（基于SDF梯度）
    vec3 normal = vec3(0.0);
    if (abs(sdf) < radius) {
        vec2 gradient = normalize(uv - center);
        float z = sqrt(max(0.0, radius * radius - length(uv - center) * length(uv - center)));
        normal = normalize(vec3(gradient, z));
        
        // 添加液态表面波动
        vec2 liquidWave = getLiquidDistortion(uv, time, 0.02);
        normal.xy += liquidWave;
        normal = normalize(normal);
    }
    
    return vec4(normal, mask);
}

// 计算边缘散射（基于Android的边缘光散射）
vec3 calculateEdgeScattering(vec2 uv, vec4 sphereData) {
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    vec2 center = vec2(0.5, 0.5);
    float radius = 0.4;
    
    // 计算边缘距离
    float sdf = sdCircle(uv, center, radius);
    float edgeDist = abs(sdf);
    
    // 视角方向
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    
    // 菲涅尔边缘高亮
    float fresnel = pow(1.0 - abs(dot(normal, viewDir)), fresnelPower);
    
    // 边缘散射光（基于Android的散射模型）
    float edgeGlow = exp(-edgeDist * edgeDist * 20.0);
    vec3 edgeColor = vec3(0.8, 0.9, 1.0) * edgeGlow * edgeBrightness;
    
    // 添加彩虹色边缘（基于角度和厚度）
    float hueShift = fresnel + (1.0 - edgeDist / 0.1);
    vec3 rainbowColor = vec3(
        0.5 + 0.5 * cos(hueShift * 6.28318 + time),
        0.5 + 0.5 * cos(hueShift * 6.28318 + 2.09439 + time * 0.8),
        0.5 + 0.5 * cos(hueShift * 6.28318 + 4.18879 + time * 1.2)
    );
    
    return mix(edgeColor, rainbowColor * 0.4, fresnel) * mask;
}

// 计算玻璃透明度（基于Android的透明度模型）
float calculateGlassAlpha(vec2 uv, vec4 sphereData) {
    vec3 normal = sphereData.xyz;
    float mask = sphereData.w;
    
    // 基于视角的透明度
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    float fresnel = pow(1.0 - abs(dot(normal, viewDir)), 2.0);
    
    // 边缘更透明
    vec2 center = vec2(0.5, 0.5);
    float sdf = sdCircle(uv, center, 0.4);
    float edgeFade = smoothstep(0.02, -0.02, sdf);
    
    // 动态透明度变化（模拟Android的液态呼吸效果）
    float timeAlpha = sin(time * 0.8) * 0.05 + 0.95;
    
    return mix(0.85, 0.2, fresnel * edgeFade) * mask * timeAlpha;
}

// 背景混合（基于Android的混合模式）
vec3 blendBackground(vec2 uv, vec2 distortedUV, sampler2D tex) {
    // 根据距离决定是否模糊
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float radius = 0.4;
    float blurFactor = smoothstep(radius - 0.3, radius - 0.1, dist);
    
    vec3 backgroundColor;
    if (blurFactor > 0.3) {
        // 边缘区域使用高斯模糊（Android技术）
        backgroundColor = gaussianBackgroundBlur(distortedUV, tex, blurRadius);
    } else {
        // 中心区域使用清晰背景
        backgroundColor = texture(tex, distortedUV).rgb;
    }
    
    return backgroundColor;
}

void main() {
    vec2 uv = TexCoord;
    
    // 获取球体数据
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
    
    // 计算液态扭曲的UV坐标
    vec2 liquidDistortion = getLiquidDistortion(uv, time, distortion * 0.1);
    vec2 distortedUV = uv + liquidDistortion;
    
    // 确保UV坐标在范围内
    distortedUV = clamp(distortedUV, 0.0, 1.0);
    
    // 混合背景（清晰+模糊）
    vec3 backgroundColor = blendBackground(uv, distortedUV, backgroundTexture);
    
    // 计算边缘散射
    vec3 edgeScattering = calculateEdgeScattering(uv, sphereData);
    
    // 计算玻璃透明度
    float glassAlpha = calculateGlassAlpha(uv, sphereData);
    
    // 玻璃基色（动态变化的淡蓝色调）
    vec3 glassTint = vec3(0.92, 0.97, 1.0) + vec3(0.02, 0.01, 0.0) * sin(time * 0.5);
    
    // 最终颜色合成（基于Android的混合模式）
    vec3 finalColor = backgroundColor * glassTint + edgeScattering;
    
    // 添加微妙的辉光效果（Android边缘光技术）
    finalColor += edgeScattering * 0.15;
    
    FragColor = vec4(finalColor, glassAlpha);
}