#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    //��ȡƬ�ε���Դ�ľ���
    float lightDistance = length(FragPos.xyz - lightPos);

    //����˥��
    lightDistance = lightDistance / far_plane;

    //д�����
    gl_FragDepth = lightDistance;
}