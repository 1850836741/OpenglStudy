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

//��Դ��Ϣ
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightScale;
uniform float specularScale;


uniform vec3 viewPos;

//���屾����ɫ
uniform vec4 color;

//����̶�
uniform float reflexScale;

//������
uniform float refractionRatio;
//����̶�
uniform float refractionScale;

uniform float far_plane;
uniform bool useShadow;

//��Ӱ����
uniform samplerCube shadows;

float ShadowCalculation(vec3 fragPos);

void main(){
   vec3 worldPos = (model * vec4(pos,1.0f)).xyz;
   vec3 worldNormal = (model * vec4(normal,1.0f)).xyz;

   //�����淽��
   vec3 lightDir = normalize(lightPos - worldPos);
   //�����淽��
   vec3 viewDir = normalize(viewPos - worldPos);
   //�����������
   vec3 reflexDir = normalize(reflect(-viewDir,normalize(worldNormal)));
   //�����������
   vec3 refractionDir = normalize(refract(-viewDir,normalize(worldNormal),refractionRatio));

   //��պ���ɫ
   vec3 skyBoxColor = texture(skyBox,reflexDir).rgb;
   float tempScale = (max(dot(lightDir, worldNormal),0.0f)*0.5+0.5);
   //ԭ���Լ���ɫ����ȷ������
   vec3 ambient_color = 0.05 * color.rgb;

   vec3 reflex_color = texture(skyBox,reflexDir).rgb;
   vec3 refraction_color = texture(skyBox, refractionDir).rgb;

   vec3 finColor = (reflex_color * reflexScale 
   + refraction_color * refractionScale 
   + color.rgb * (1-reflexScale-refractionScale)) * tempScale;
   //��������������
   vec3 ambient_diff = ambient_color + finColor * lightScale ;

   vec3 halfVertex = normalize(viewDir + lightDir);
   vec3 specular = pow(max(dot(halfVertex,worldNormal),0.0f),specularScale) * lightColor;

   

   //�Ƿ�ʹ����Ӱ
   if (useShadow){
       FragColor = vec4((ambient_diff + specular) * ShadowCalculation(worldPos),1.0f);
   }else{
       FragColor = vec4((ambient_diff + specular),1.0f);
   }
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
    float shadow = currentDepth -  bias > closestDepth ? 0.0 : 1.0;
    return shadow;
}