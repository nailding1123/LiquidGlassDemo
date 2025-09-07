#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform vec2 resolution;

float circleSDF(vec2 p, float radius) {
    return length(p) - radius;
}

vec4 generateCircleSDF() {
    // 修正坐标映射：确保与背景捕获区域和渲染坐标系统一致
    // vTexCoord: [0,1] 纹理坐标，(0,0)左下角，(1,1)右上角
    // 转换为以中心为原点的标准化坐标
    vec2 p = (vTexCoord - 0.5) * 2.0;
    
    float radius = 0.5;
    float distance = circleSDF(p, radius);
    
    // 计算法线方向用于折射效果
    float epsilon = 0.001;
    float dx = circleSDF(p + vec2(epsilon, 0.0), radius) - 
               circleSDF(p - vec2(epsilon, 0.0), radius);
    float dy = circleSDF(p + vec2(0.0, epsilon), radius) - 
               circleSDF(p - vec2(0.0, epsilon), radius);
    
    vec2 normalDir = normalize(vec2(dx, dy));
    if (length(vec2(dx, dy)) < 0.001) {
        normalDir = normalize(p);
    }
    
    // 标准化距离值到[0,1]范围
    float normalizedDist = distance / radius;
    normalizedDist = clamp(normalizedDist, -1.0, 1.0);
    
    float distanceValue = (normalizedDist + 1.0) * 0.5;
    vec2 directionValue = normalDir * 0.5 + 0.5;
    
    return vec4(distanceValue, directionValue.x, directionValue.y, 1.0);
}

void main() {
    FragColor = generateCircleSDF();
}