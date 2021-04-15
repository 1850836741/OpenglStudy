#version 330 core
in vec4 aPos;

uniform mat4 mvp;

void main(){
   gl_Position = mvp * aPos;
}