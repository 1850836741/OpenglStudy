#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 worldPos;
out vec3 worldNormal;


uniform mat4 viewProjection;
uniform mat4 model;

void main()
{
    texCoords = aTexCoords;
    worldPos = (model * vec4(aPos, 1.0)).xyz;
    worldNormal = mat3(model) * aNormal;   

    gl_Position =  viewProjection * vec4(worldPos, 1.0);
}