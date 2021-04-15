#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 worldPos;
in vec3 worldNormal;

uniform mat4 viewProjection;
uniform mat4 model;

//材质的反照率，物体反射光的能力
uniform vec3 albedo;
//金属率
uniform float metallic;
//粗糙度
uniform float roughness;
//光照强度（比例）
uniform float lightScale;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 viewPos;
uniform float fresnelScale;

const float PI = 3.14159265359;

//正态分布，roughness为粗糙度
float DistributionGGX(vec3 worldNormal, vec3 halfVertex, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(worldNormal, halfVertex), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
//几何函数，k是粗糙度基于几何函数是针对直接光照还是针对IBL光照的重映射(Remapping)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 worldNormal, vec3 viewDir, vec3 lightDir, float roughness)
{

    float NdotV = max(dot(worldNormal, viewDir), 0.0);
    float NdotL = max(dot(worldNormal, lightDir), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

//菲涅尔，cosTheta为视线和法线点乘积
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
//菲涅尔plus版本
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  


void main(){
    vec3 viewDir = (viewPos - worldPos);
    //反射方向
    vec3 reflectDir = reflect(-viewDir,worldNormal);


    vec3 F0 = vec3(fresnelScale); 
    F0 = mix(F0, albedo, metallic);

    //最终项光照，累加
    vec3 allLight = vec3(0.0f);

    vec3 lightDir = normalize(lightPos - worldPos);
    vec3 halfVertex = normalize(lightDir + viewDir);
    //光线距离
    float lightDistance = length(lightPos - worldPos);
    //光线衰减因子
    float attenuation = 1.0 / (lightDistance * lightDistance);
    //衰减后的光照
    vec3 radiance = lightColor * attenuation;

    //Cook-Torrance BRDF
    float NDF = DistributionGGX(worldNormal, halfVertex, roughness);   
    float G   = GeometrySmith(worldNormal, viewDir, lightDir, roughness);    
    vec3 F    = fresnelSchlick(max(dot(halfVertex, viewDir), 0.0), F0);

    
    vec3 nominator    = NDF * G * F;
    //0.001防止为0
    float denominator = 4 * max(dot(worldNormal, viewDir), 0.0) * max(dot(worldNormal, lightDir), 0.0) + 0.001;

    //高光
    vec3 specular = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    allLight = (kD*albedo/PI + kS * specular) * radiance * max(dot(worldNormal,lightDir),0.0);
    
    //菲涅尔效益plus版本, 用于环境光
    vec3 Fplus = fresnelSchlickRoughness(max(dot(worldNormal, viewDir), 0.0), F0, roughness);

    vec3 kSplus = Fplus;
    vec3 kDplus = 1.0 - kS;
    kDplus *= (1.0 - metallic);

    //对辐照度图采样
    vec3 irradiance = texture(irradianceMap, worldNormal).rgb;
    //辐照图漫反射
    vec3 diffuse = irradiance * albedo;

    //最大反射
    //对预滤波图和BRDF lut进行采样，并根据分割和近似将它们组合在一起，以获得IBL镜面反射部分。
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectDir,  roughness * MAX_REFLECTION_LOD).rgb; 
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(worldNormal, viewDir), 0.0), roughness)).rg;
    vec3 mirrorSpecular = prefilteredColor * (Fplus * brdf.x + brdf.y);

    vec3 mirrorAmbient = (kDplus * diffuse + mirrorSpecular) * lightScale;

    vec3 color = mirrorAmbient + allLight;

      // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0) * 3.0;
}