//���Բ��ͼת��������պ�
#version 330 core
out vec4 FragColor;
in vec3 worldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    //atan(z,x)�����Ի��ȱ�ʾ��z/x�ķ�����
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    //asin������[-1,1]�ϵ�ȡֵΪ[-1.5,1.5]
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(worldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}