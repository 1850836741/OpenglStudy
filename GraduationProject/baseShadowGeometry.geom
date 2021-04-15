#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
//6个光空间面的变换矩阵
uniform mat4 shadowMatrices[6];

//片段位置
out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        //指定发散出基本图形送到立方体贴图的哪个面，将图形二维化
        gl_Layer = face;
        //遍历三角形
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}