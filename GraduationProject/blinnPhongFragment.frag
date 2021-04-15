#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 model;
uniform mat4 viewProjection;

//��Դ��Ϣ
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float specularScale;

uniform vec3 viewPos;

//�Ƿ�ʹ��color������ʹ������
uniform bool useColor;
uniform vec4 color;

uniform float far_plane;
uniform bool useShadow;

uniform sampler2D default_texture;

//��Ӱ����
uniform samplerCube shadows;

//�ذ�
uniform bool isPlane;


//ʹ����ɫ�Ļ�
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos);
vec3 default_notUseColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos);
float ShadowCalculation(vec3 fragPos);
void main(){

   vec3 worldPos = (model * vec4(pos,1.0f)).xyz;
   vec3 worldNormal = (model * vec4(normal,1.0f)).xyz;
   if(isPlane){
       worldNormal = transpose(inverse(mat3(model))) * (-1.0 * normal).xyz;
   }
   

   //�����淽��
   vec3 lightDir = normalize(lightPos - worldPos);
   vec3 ambient_diff;
   if(useColor){
      ambient_diff = default_useColor_diff(lightDir,worldNormal,worldPos);
   }else{
      ambient_diff = default_notUseColor_diff(lightDir,worldNormal,worldPos);
   }

   //��Ұ�淽��
   vec3 viewDir = normalize(viewPos - worldPos);

   //����������Ż�����
   vec3 halfVertex = normalize(viewDir + lightDir);

   //�߹���
   vec3 specular = pow(max(dot(halfVertex, worldNormal),0.0f),specularScale) * lightColor;
   
   FragColor = vec4(ambient_diff + specular, 1.0f);

}

//���ػ�������������
vec3 default_useColor_diff(vec3 lightD,vec3 worldNormal,vec3 worldPos){
  vec3 ambient = 0.05 * color.rgb;
  //������Ч����[-1,1]��ӳ�䵽[0,1],���ϸ�������ǿ��
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
  //�Ƿ�ʹ����Ӱ
   if (useShadow){
       diffWeight *= ShadowCalculation(worldPos);
   }
  return ambient + diffWeight;
}

float ShadowCalculation(vec3 fragPos)
{
    //��Դָ��Ƭ��
    vec3 fragToLight = fragPos - lightPos;
    //��������Ӱ3D�������
    float closestDepth = texture(shadows, fragToLight).r;
    //��Ŀǰ����[0,1]֮������Է�Χ�������ǽ���ת����ԭʼ���ֵ
    closestDepth *= far_plane;
    //��Դָ��Ƭ�εľ���
    float currentDepth = length(fragToLight);
    //�����ݵ�ƫ����
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 0.3 : 1.0;
    return shadow;
}