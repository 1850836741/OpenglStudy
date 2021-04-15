//广角圆柱图转立方体天空盒
#version 330 core
out vec4 FragColor;
in vec3 worldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    //atan(z,x)返回以弧度表示的z/x的反正切
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    //asin函数在[-1,1]上的取值为[-1.5,1.5]
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(worldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}