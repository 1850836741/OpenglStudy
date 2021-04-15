#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 viewProjection;

out vec3 pos;
out vec3 normal;
out vec2 texCoords;
out vec3 tangent;
out vec3 bitangent;

void main(){
  pos = aPos;
  normal = aNormal;
  texCoords = aTexCoords;
  tangent = aTangent;
  bitangent = aBitangent;
  gl_Position = viewProjection * model * vec4(aPos,1.0f);
}