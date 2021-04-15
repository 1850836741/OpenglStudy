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

uniform vec3 viewPos;
//���屾����ɫ
uniform vec4 color;

uniform float fresnelScale;

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
   vec3 skyBoxColor = texture(skyBox, reflexDir).rgb;
   float tempScale = (max(dot(lightDir, worldNormal),0.0f)*0.5+0.5);

   float fresnel = fresnelScale + (1 - fresnelScale) * pow(1 - dot(viewDir,worldNormal),5.0);
   vec3 ambientColor = 0.05 * color.rgb;
   //����
   vec3 reflectDiff = skyBoxColor * tempScale;
   vec3 tempDiff =  color.rgb * tempScale;
   vec3 ambient_diff = ambientColor + ((reflectDiff - tempDiff) * fresnel + tempDiff) * lightScale;

   FragColor = vec4(ambient_diff,1.0f);

   //�Ƿ�ʹ����Ӱ
   if (useShadow){
       FragColor *= ShadowCalculation(worldPos);
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