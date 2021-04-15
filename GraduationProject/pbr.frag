#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 worldPos;
in vec3 worldNormal;

uniform mat4 viewProjection;
uniform mat4 model;

//���ʵķ����ʣ����巴��������
uniform vec3 albedo;
//������
uniform float metallic;
//�ֲڶ�
uniform float roughness;
//����ǿ�ȣ�������
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

//��̬�ֲ���roughnessΪ�ֲڶ�
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
//���κ�����k�ǴֲڶȻ��ڼ��κ��������ֱ�ӹ��ջ������IBL���յ���ӳ��(Remapping)
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

//��������cosThetaΪ���ߺͷ��ߵ�˻�
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
//������plus�汾
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  


void main(){
    vec3 viewDir = (viewPos - worldPos);
    //���䷽��
    vec3 reflectDir = reflect(-viewDir,worldNormal);


    vec3 F0 = vec3(fresnelScale); 
    F0 = mix(F0, albedo, metallic);

    //��������գ��ۼ�
    vec3 allLight = vec3(0.0f);

    vec3 lightDir = normalize(lightPos - worldPos);
    vec3 halfVertex = normalize(lightDir + viewDir);
    //���߾���
    float lightDistance = length(lightPos - worldPos);
    //����˥������
    float attenuation = 1.0 / (lightDistance * lightDistance);
    //˥����Ĺ���
    vec3 radiance = lightColor * attenuation;

    //Cook-Torrance BRDF
    float NDF = DistributionGGX(worldNormal, halfVertex, roughness);   
    float G   = GeometrySmith(worldNormal, viewDir, lightDir, roughness);    
    vec3 F    = fresnelSchlick(max(dot(halfVertex, viewDir), 0.0), F0);

    
    vec3 nominator    = NDF * G * F;
    //0.001��ֹΪ0
    float denominator = 4 * max(dot(worldNormal, viewDir), 0.0) * max(dot(worldNormal, lightDir), 0.0) + 0.001;

    //�߹�
    vec3 specular = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    allLight = (kD*albedo/PI + kS * specular) * radiance * max(dot(worldNormal,lightDir),0.0);
    
    //������Ч��plus�汾, ���ڻ�����
    vec3 Fplus = fresnelSchlickRoughness(max(dot(worldNormal, viewDir), 0.0), F0, roughness);

    vec3 kSplus = Fplus;
    vec3 kDplus = 1.0 - kS;
    kDplus *= (1.0 - metallic);

    //�Է��ն�ͼ����
    vec3 irradiance = texture(irradianceMap, worldNormal).rgb;
    //����ͼ������
    vec3 diffuse = irradiance * albedo;

    //�����
    //��Ԥ�˲�ͼ��BRDF lut���в����������ݷָ�ͽ��ƽ����������һ���Ի��IBL���淴�䲿�֡�
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