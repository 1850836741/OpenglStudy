#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 model;
uniform mat4 viewProjection;

//光源信息
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float specularScale;

uniform vec3 viewPos;

//是否使用color（否，则使用纹理）
uniform bool useColor;
uniform vec4 color;

uniform float far_plane;
uniform bool useShadow;

uniform sampler2D default_texture;

//阴影纹理
uniform samplerCube shadows;

//地板
uniform bool isPlane;


//使用颜色的话
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos);
vec3 default_notUseColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos);
float ShadowCalculation(vec3 fragPos);
void main(){

   vec3 worldPos = (model * vec4(pos,1.0f)).xyz;
   vec3 worldNormal = (model * vec4(normal,1.0f)).xyz;
   if(isPlane){
       worldNormal = transpose(inverse(mat3(model))) * (-1.0 * normal).xyz;
   }
   

   //光线逆方向
   vec3 lightDir = normalize(lightPos - worldPos);
   vec3 ambient_diff;
   if(useColor){
      ambient_diff = default_useColor_diff(lightDir,worldNormal,worldPos);
   }else{
      ambient_diff = default_notUseColor_diff(lightDir,worldNormal,worldPos);
   }

   //视野逆方向
   vec3 viewDir = normalize(viewPos - worldPos);

   //半程向量，优化光照
   vec3 halfVertex = normalize(viewDir + lightDir);

   //高光项
   vec3 specular = pow(max(dot(halfVertex, worldNormal),0.0f),specularScale) * lightColor;
   
   FragColor = vec4(ambient_diff + specular, 1.0f);

}

//返回环境光和漫反射光
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos){
  vec3 ambient = 0.05 * color.rgb;
  //将光线效果从[-1,1]，映射到[0,1],提高细节与光照强度
  vec3 diffWeight = (max(dot(lightD, worldNormal),0.0f)) * color.rgb;
  if (useShadow){
       diffWeight *= ShadowCalculation(worldPos);
   }
  return ambient + diffWeight;
}

vec3 default_notUseColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos){

  vec3 tempColor = texture(default_texture, texCoords).rgb;
  vec3 ambient = 0.05 * tempColor;
  vec3 diffWeight = (max(dot(lightD, worldNormal),0.0f)) * tempColor;
  //是否使用阴影
   if (useShadow){
       diffWeight *= ShadowCalculation(worldPos);
   }
  return ambient + diffWeight;
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
    float shadow = currentDepth -  bias > closestDepth ? 0.3 : 1.0;
    return shadow;
}