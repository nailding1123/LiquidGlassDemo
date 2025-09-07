#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 WorldPos;

uniform sampler2D backgroundTexture;
uniform sampler2D sdfTexture;

uniform float ref_height = 20.0;
uniform float ref_length = 30.0;
uniform float ref_border_width = 5.0;
uniform float ref_exposure = 1.0;
uniform float scale = 1.0;

vec4 getColorWithOffset(vec2 coord, vec2 offset) {
    vec2 screenSize = textureSize(backgroundTexture, 0);
    // 修正坐标映射：确保与背景捕获区域的屏幕坐标一致
    vec2 normalizedCoord = (coord + offset) / screenSize;
    normalizedCoord = clamp(normalizedCoord, 0.0, 1.0);
    
    vec4 color = texture(backgroundTexture, normalizedCoord);
    vec3 rgb = color.rgb * ref_exposure;
    return vec4(rgb, color.a);
}

float linear_map(float x, float y, float a, float b, float tsetnumber) {
    float ratio = (tsetnumber - x) / (y - x);
    return a + ratio * (b - a);
}

vec4 decodeSDFData(vec4 sdfData) {
    float distance = sdfData.r;
    // 修正法线解码：确保方向正确
    vec2 normal = (sdfData.gb - 0.5) * 2.0;
    return vec4(distance, normal, sdfData.a);
}

float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

void main() {
    vec2 screenSize = textureSize(backgroundTexture, 0);
    vec2 screenCoord = gl_FragCoord.xy;
    
    // 确保SDF纹理采样使用正确的纹理坐标
    vec4 sdfData = texture(sdfTexture, TexCoord);
    vec4 decoded = decodeSDFData(sdfData);
    
    float distance = decoded.r;
    vec2 normal = decoded.gb;
    
    // 修复：只有当距离小于阈值时才应用效果
    if (distance >= 0.99999) {
        FragColor = texture(backgroundTexture, TexCoord);
        return;
    }
    
    float dis = (1.0 - distance) * 50.0 * scale;
    float r_height = ref_height;
    float r_length = ref_length;
    
    if (dis < r_height) {
        float offsetVal = linear_map(r_height, 0.0, r_height, r_height - r_length, dis);
        float offset = dis - offsetVal;
        
        vec2 offset_normal = normal * offset;
        vec4 result = getColorWithOffset(screenCoord, offset_normal);
        
        if (dis <= ref_border_width) {
            float edgeRatio = 1.0 - (dis / ref_border_width);
            float smoothRatio = smoothstep(0.0, 1.0, edgeRatio);
            
            float angleFactor = abs(normal.x * normal.y);
            float highlight = smoothRatio * (0.3 + angleFactor * 0.7);
            
            result = result * (1.0 + highlight * 0.6);
        }
        
        FragColor = result;
    } else {
        FragColor = getColorWithOffset(screenCoord, vec2(0.0));
    }
}