#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    //获取片段到光源的距离
    float lightDistance = length(FragPos.xyz - lightPos);

    //计算衰减
    lightDistance = lightDistance / far_plane;

    //写入深度
    gl_FragDepth = lightDistance;
}