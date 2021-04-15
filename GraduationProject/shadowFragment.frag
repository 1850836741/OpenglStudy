#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;
//漫反射纹理
uniform sampler2D diffuseTexture;
//阴影纹理
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;


float ShadowCalculation(vec3 fragPos)
{
    //光源指向片段
    vec3 fragToLight = fragPos - lightPos;
    //对万向阴影3D纹理采样
    float closestDepth = texture(depthMap, fragToLight).r;
    //它目前处于[0,1]之间的线性范围。让我们将其转换回原始深度值
    closestDepth *= far_plane;
    //光源指向片段的距离
    float currentDepth = length(fragToLight);
    //解决锯齿的偏移量
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}