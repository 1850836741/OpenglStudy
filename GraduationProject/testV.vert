#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 viewProjection;

//光源信息
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

//是否使用color（否，则使用纹理）
uniform bool useColor;
uniform vec4 color;

uniform float far_plane;
uniform bool useShadow;

uniform sampler2D default_texture;
//阴影纹理
uniform samplerCube shadows;

//使用颜色的话
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal);
vec3 default_notUseColor_diff(vec3 lightD,vec3 worldNormal);
float ShadowCalculation(vec3 fragPos);

//最终颜色
out vec4 default_finallyColor;

void main(){
   vec3 worldPos = (model * vec4(aPos,1.0f)).xyz;
   vec3 worldNormal = (model * vec4(aNormal,1.0f)).xyz;
   //光线逆方向
   vec3 lightDir = normalize(lightPos - worldPos);
   vec3 ambient_diff;
   if(useColor){
      ambient_diff = default_useColor_diff(lightDir, worldNormal);
   }else{
      ambient_diff = default_notUseColor_diff(lightDir, worldNormal);
   }

   //视野逆方向
   vec3 viewDir = normalize(viewPos - aPos);

   //出射光
   vec3 reflectDir = reflect(-lightDir, worldNormal);

   //高光项
   vec3 specular = pow(max(dot(viewDir, reflectDir),0.0f),8.0f) * lightColor;


   default_finallyColor = vec4(ambient_diff + specular, 1.0f);

   //是否使用阴影
   if (useShadow){
       default_finallyColor *= ShadowCalculation(worldPos);
   }
}

//返回环境光和漫反射光
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal){
  vec3 ambient = 0.05 * color.rgb;
  vec3 diff = max(dot(lightD, worldNormal),0.0f) * color.rgb * lightColor;
  return ambient + diff;
}

vec3 default_notUseColor_diff(vec3 lightD,vec3 worldNormal){

  vec3 tempColor = texture(default_texture, aTexCoords).rgb;
  vec3 ambient = 0.05 * tempColor;
  vec3 diff = max(dot(lightD, worldNormal),0.0f) * tempColor * lightColor;
  return ambient + diff;
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