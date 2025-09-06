#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

// 背景纹理
uniform sampler2D backgroundTexture;
// SDF纹理
uniform sampler2D sdfTexture;
// 时间
uniform float time;
// 扭曲强度
uniform float distortion;
// 相机位置
uniform vec3 viewPos;
// 屏幕分辨率
uniform vec2 resolution;

// 边缘散射参数
uniform float edgeScatter = 0.8;
uniform float blurRadius = 3.0;
uniform float thickness = 0.5;



// 计算边缘距离 - 使用球体SDF
float getEdgeDistance(vec2 uv) {
    vec2 center = vec2(0.5, 0.5);
    float radius = 0.4;
    float dist = distance(uv, center);
    return smoothstep(radius - 0.1, radius + 0.1, dist);
}

// 高斯模糊背景
vec4 blurBackground(vec2 uv, float radius) {
    vec4 color = vec4(0.0);
    float weightSum = 0.0;
    
    for (int i = -4; i <= 4; i++) {
        for (int j = -4; j <= 4; j++) {
            vec2 offset = vec2(float(i), float(j)) * radius / resolution;
            float weight = exp(-0.5 * (float(i*i + j*j)) / (radius * radius));
            color += texture(backgroundTexture, uv + offset) * weight;
            weightSum += weight;
        }
    }
    
    return color / weightSum;
}

// 边缘散射效果
vec4 applyEdgeScatter(vec2 uv, float sdfValue) {
    float edgeFactor = smoothstep(0.0, 0.1, abs(sdfValue - 0.5));
    
    // 基于边缘距离的模糊
    float blurAmount = blurRadius * (1.0 - edgeFactor) * edgeScatter;
    vec4 blurredBg = blurBackground(uv, blurAmount);
    
    // 边缘颜色偏移
    vec2 scatterOffset = vec2(
        cos(time * 2.0 + uv.y * 10.0) * 0.01,
        sin(time * 2.0 + uv.x * 10.0) * 0.01
    ) * (1.0 - edgeFactor);
    
    vec4 scatteredColor = texture(backgroundTexture, uv + scatterOffset);
    
    return mix(scatteredColor, blurredBg, edgeFactor);
}

// 液态扭曲
vec2 applyLiquidDistortion(vec2 uv, float sdfValue) {
    float wave = sin(time * 3.0 + uv.x * 8.0 + uv.y * 6.0) * 0.02;
    wave += sin(time * 2.0 + uv.x * 12.0) * 0.015;
    
    // 基于SDF的扭曲强度
    float distortionFactor = 1.0 - abs(sdfValue - 0.5) * 2.0;
    wave *= distortionFactor * distortion;
    
    return uv + vec2(wave, wave * 0.7);
}

// 菲涅尔效果
float fresnel(vec3 normal, vec3 viewDir) {
    float fresnelTerm = 1.0 - dot(normal, -viewDir);
    return pow(fresnelTerm, 3.0) * 0.5;
}

// 计算法线
vec3 calculateNormal(vec2 uv, float sdfValue) {
    float epsilon = 0.001;
    float dX = getEdgeDistance(uv + vec2(epsilon, 0.0)) - getEdgeDistance(uv - vec2(epsilon, 0.0));
    float dY = getEdgeDistance(uv + vec2(0.0, epsilon)) - getEdgeDistance(uv - vec2(0.0, epsilon));
    
    return normalize(vec3(dX, dY, 0.1));
}

void main() {
    vec2 uv = TexCoord;
    
    // 获取SDF值
    float sdfValue = getEdgeDistance(uv);
    
    // 计算扭曲UV
    vec2 distortedUV = applyLiquidDistortion(uv, sdfValue);
    
    // 获取背景颜色
    vec4 bgColor = applyEdgeScatter(distortedUV, sdfValue);
    
    // 计算玻璃效果
    vec3 normal = calculateNormal(uv, sdfValue);
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    
    // 菲涅尔反射
    float fresnelTerm = fresnel(normal, viewDir);
    
    // 透明度计算
    float alpha = 0.2 + fresnelTerm * 0.6;
    alpha *= smoothstep(0.3, 0.7, sdfValue);
    
    // 厚度效果
    float thicknessEffect = thickness * (1.0 - abs(sdfValue - 0.5) * 2.0);
    
    // 最终颜色
    vec4 glassColor = vec4(0.9, 0.95, 1.0, 1.0) * (1.0 + thicknessEffect);
    
    FragColor = mix(bgColor, glassColor, alpha);
    FragColor.a = alpha;
}