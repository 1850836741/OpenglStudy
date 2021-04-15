#version 430 core

//ÿ���е�����Ϊ
layout(local_size_x = 128) in;
//�ٶ�
layout(rgba32f, binding = 0)uniform imageBuffer velocity_buffer;
//λ��
layout(rgba32f, binding = 1)uniform imageBuffer position_buffer;

//��������
uniform int ParticleSize;
//����
uniform int lineNum;
//����
uniform int columnNum;
//����ϵ��
uniform float k;
//���Ӽ��ʼ����
uniform float restLength;
//Լ��
uniform float constraint;
//ʱ����
uniform float dt;
//����
uniform vec3 force;
//����
uniform float mass;
void main(){

   int index = int(gl_GlobalInvocationID.x);
   int rightIndex = (index+1)/columnNum == index/(columnNum) ? index+1:-1;
   int topIndex = index+columnNum >= ParticleSize?-1:index+columnNum;
   int leftIndex = (index-1)/columnNum == index/(columnNum) ? index-1:-1;
   int bottomIndex = index-columnNum >= 0?index-columnNum : -1;
   
   vec4 speedVec4 = imageLoad(velocity_buffer, index);
   vec4 posVec4 = imageLoad(position_buffer, index);

   vec3 allForce = force;
   vec3 allSpeed = speedVec4.xyz;
   vec3 pos = posVec4.xyz;
   if (rightIndex!=-1){
     //����
     vec3 dir = pos - imageLoad(position_buffer, rightIndex).xyz;
     //����
     float len = length(dir); 
     //�����α�
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }
  if (topIndex!=-1){
     //����
     vec3 dir = pos - imageLoad(position_buffer, topIndex).xyz;
     //����
     float len = length(dir); 
     //�����α�
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }

  if (leftIndex!=-1){
     //����
     vec3 dir = pos - imageLoad(position_buffer, leftIndex).xyz;
     //����
     float len = length(dir); 
     //�����α�
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }

  if (bottomIndex!=-1){
     //����
     vec3 dir = pos - imageLoad(position_buffer, bottomIndex).xyz;
     //����
     float len = length(dir); 
     //�����α�
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }

  vec3 a = allForce / mass;
  float temp = exp(-dt * constraint);
  allSpeed = allSpeed + a*dt;
  allSpeed *= temp;
  pos += allSpeed * dt;

  //posVec4 = vec4(posVec4.x+0.01,posVec4.y,posVec4.z,posVec4.w);
  //imageStore(position_buffer, int(gl_GlobalInvocationID.x),posVec4);
  //imageStore(velocity_buffer, int(gl_GlobalInvocationID.x),vec4(allSpeed,speedVec4.w));
  if (index == columnNum-1 || index == ParticleSize-1){
      imageStore(position_buffer, int(gl_GlobalInvocationID.x),posVec4);
      imageStore(velocity_buffer, int(gl_GlobalInvocationID.x),speedVec4);
  }else{
      imageStore(position_buffer, int(gl_GlobalInvocationID.x),vec4(pos,posVec4.w));
      imageStore(velocity_buffer, int(gl_GlobalInvocationID.x),vec4(allSpeed,posVec4.w));
  }
  
} 
