#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 p;
uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords)/length(vec4(0.0)-p);
}