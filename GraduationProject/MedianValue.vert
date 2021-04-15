#version 430 core

//每块中的粒子为
layout(local_size_x = 128) in;
//速度
layout(rgba32f, binding = 0)uniform imageBuffer velocity_buffer;
//位置
layout(rgba32f, binding = 1)uniform imageBuffer position_buffer;

//粒子总数
uniform int ParticleSize;
//行数
uniform int lineNum;
//列数
uniform int columnNum;
//弹力系数
uniform float k;
//粒子间初始距离
uniform float restLength;
//约束
uniform float constraint;
//时间间隔
uniform float dt;
//合力
uniform vec3 force;
//质量
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
     //方向
     vec3 dir = pos - imageLoad(position_buffer, rightIndex).xyz;
     //距离
     float len = length(dir); 
     //发生形变
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }
  if (topIndex!=-1){
     //方向
     vec3 dir = pos - imageLoad(position_buffer, topIndex).xyz;
     //距离
     float len = length(dir); 
     //发生形变
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }

  if (leftIndex!=-1){
     //方向
     vec3 dir = pos - imageLoad(position_buffer, leftIndex).xyz;
     //距离
     float len = length(dir); 
     //发生形变
     if (len != restLength){
         allForce += -(len - restLength) * k * normalize(dir);
     }
  }

  if (bottomIndex!=-1){
     //方向
     vec3 dir = pos - imageLoad(position_buffer, bottomIndex).xyz;
     //距离
     float len = length(dir); 
     //发生形变
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
