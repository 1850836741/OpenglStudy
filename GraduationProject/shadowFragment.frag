#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;
//����������
uniform sampler2D diffuseTexture;
//��Ӱ����
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;


float ShadowCalculation(vec3 fragPos)
{
    //��Դָ��Ƭ��
    vec3 fragToLight = fragPos - lightPos;
    //��������Ӱ3D�������
    float closestDepth = texture(depthMap, fragToLight).r;
    //��Ŀǰ����[0,1]֮������Է�Χ�������ǽ���ת����ԭʼ���ֵ
    closestDepth *= far_plane;
    //��Դָ��Ƭ�εľ���
    float currentDepth = length(fragToLight);
    //�����ݵ�ƫ����
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}