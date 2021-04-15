#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform samplerCube skyBox;

uniform mat4 model;
uniform mat4 viewProjection;

//光源信息
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightScale;
uniform float specularScale;


uniform vec3 viewPos;

//物体本身颜色
uniform vec4 color;

//反射程度
uniform float reflexScale;

//折射率
uniform float refractionRatio;
//折射程度
uniform float refractionScale;

uniform float far_plane;
uniform bool useShadow;

//阴影纹理
uniform samplerCube shadows;

float ShadowCalculation(vec3 fragPos);

void main(){
   vec3 worldPos = (model * vec4(pos,1.0f)).xyz;
   vec3 worldNormal = (model * vec4(normal,1.0f)).xyz;

   //光线逆方向
   vec3 lightDir = normalize(lightPos - worldPos);
   //视线逆方向
   vec3 viewDir = normalize(viewPos - worldPos);
   //反射采样向量
   vec3 reflexDir = normalize(reflect(-viewDir,normalize(worldNormal)));
   //折射采样向量
   vec3 refractionDir = normalize(refract(-viewDir,normalize(worldNormal),refractionRatio));

   //天空盒颜色
   vec3 skyBoxColor = texture(skyBox,reflexDir).rgb;
   float tempScale = (max(dot(lightDir, worldNormal),0.0f)*0.5+0.5);
   //原本自己颜色的正确漫反射
   vec3 ambient_color = 0.05 * color.rgb;

   vec3 reflex_color = texture(skyBox,reflexDir).rgb;
   vec3 refraction_color = texture(skyBox, refractionDir).rgb;

   vec3 finColor = (reflex_color * reflexScale 
   + refraction_color * refractionScale 
   + color.rgb * (1-reflexScale-refractionScale)) * tempScale;
   //环境光加漫反射光
   vec3 ambient_diff = ambient_color + finColor * lightScale ;

   vec3 halfVertex = normalize(viewDir + lightDir);
   vec3 specular = pow(max(dot(halfVertex,worldNormal),0.0f),specularScale) * lightColor;

   

   //是否使用阴影
   if (useShadow){
       FragColor = vec4((ambient_diff + specular) * ShadowCalculation(worldPos),1.0f);
   }else{
       FragColor = vec4((ambient_diff + specular),1.0f);
   }
}

float ShadowCalculation(vec3 fragPos)
{
    //光源指向片段
    vec3 fragToLight = fragPos - lightPos;
    //对万向阴影3D纹理采样
    float closestDepth = texture(shadows, fragToLight).r;
    //它目前处于[0,1]之间的线性范围。让我们将其转换回原始深度值
    closestDepth *= far_plane;
    //光源指向片段的距离
    float currentDepth = length(fragToLight);
    //解决锯齿的偏移量
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 0.0 : 1.0;
    return shadow;
}