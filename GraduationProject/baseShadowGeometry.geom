#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
//6����ռ���ı任����
uniform mat4 shadowMatrices[6];

//Ƭ��λ��
out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        //ָ����ɢ������ͼ���͵���������ͼ���ĸ��棬��ͼ�ζ�ά��
        gl_Layer = face;
        //����������
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}