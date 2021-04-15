#version 430 core

//每块中的粒子为
layout(local_size_x = 128) in;
//速度
layout(rgba32f, binding = 0)uniform imageBuffer velocity_buffer;
//位置
layout(rgba32f, binding = 1)uniform imageBuffer position_buffer;
//密度
//layout(rgba32f, binding = 1)uniform imageBuffer density_buffer;

//粒子总数
uniform int ParticleSize;
//质量
uniform float mass;
//核函数范围
uniform float kernelFunction;
//流体相关常数
uniform float K;
//初始静态密度
uniform float density0;
//粘度系数
uniform float u;
//间隔时间
uniform float dt;
//粒子半径
uniform float radiu;
//刚度
uniform float m_boundaryStiffness;
//阻尼
uniform float m_boundaryDampening;
//合力
uniform vec3 force;

uniform float right;
uniform float left;
uniform float top;
uniform float bottom;
uniform float front;
uniform float back;

uniform float loss;

const float PI = 3.14159265358979323846f;


void main(){
    int index = int(gl_GlobalInvocationID.x);
    vec4 posVec4 = imageLoad(position_buffer, index);
    vec4 speedVec4 = imageLoad(velocity_buffer, index);
    float densityNow = speedVec4.w;
    const int num = ParticleSize;


    //密度
    float densitySum = 0.0f;
    float tempD = mass * 315 / (64 * PI * pow(kernelFunction, 9));

    for (int i=0;i<num;i++){
        vec4 tempI = imageLoad(position_buffer, i);
        float r = distance(posVec4.xyz,tempI.xyz);
        if(r <= kernelFunction){
            densitySum += pow(kernelFunction*kernelFunction-r*r,3);
        }
    }
    densitySum *= tempD;
    //vec4 densityVec4 = vec4(densitySum);

    //imageStore(density_buffer, int(gl_GlobalInvocationID.x),densityVec4);

    ///用于计算压力项
    float computePressure = max(K*(densityNow - density0),0.0f);

    //压力项
    vec3 pSum = vec3(0.0f);
    float tempP = mass * 45 / (PI * pow(kernelFunction, 6));
    //粘力项
    vec3 vSum = vec3(0.0f);
    float tempV = mass * u * 45 / (PI*pow(kernelFunction,6));


    for(int i=0;i<num;i++){
       vec4 tempI = imageLoad(position_buffer, i);
       vec4 dir = posVec4-tempI;
       vec3 dirVec3 = dir.xyz;
       float r = length(dir.xyz); 
       vec4 tempSpeed = imageLoad(velocity_buffer, i);
       float tempdensity = tempSpeed.w;
       float tempPressure = max(K*(tempdensity - density0),0.0f);
       if(i!=index && r <= kernelFunction){
           pSum += (tempPressure + computePressure)/(2.0f * tempdensity * densityNow) * pow(kernelFunction-r,2) * normalize(dirVec3);
           vSum += (tempSpeed.xyz - speedVec4.xyz)/(tempdensity * densityNow) * (kernelFunction - r);
       }
    }
    pSum *= tempP;
    vSum *= tempV;
    //粘力
    

    vec3 allForce = (force + pSum + vSum);

    vec3 a = allForce / mass;
    vec3 speed = speedVec4.xyz + a*dt;
    //speed *= exp(-dt * 1.f);
    vec3 pos = posVec4.xyz + speed * dt;

    if (pos.x>right){
        pos = vec3(right,pos.y,pos.z);
        speed.x *= -1 * loss;
    }
    if (pos.x<left){
        pos = vec3(left,pos.y,pos.z);
        speed.x *= -1 * loss;
    }
    if (pos.y>top){
        pos = vec3(pos.x,top,pos.z);
        speed.y *= -1 * loss;
    }
    if (pos.y<bottom){
        pos = vec3(pos.x,bottom,pos.z);
        speed.y *= -1 * loss;
    }
    if (pos.z>front){
        pos = vec3(pos.x,pos.y,front);
        speed.z *= -1 * loss;
    }
    if (pos.z<back){
        pos = vec3(pos.x,pos.y,back);
        speed.z *= -1 * loss;
    }

    imageStore(position_buffer, int(gl_GlobalInvocationID.x),vec4(pos,posVec4.w));
    imageStore(velocity_buffer, int(gl_GlobalInvocationID.x),vec4(speed,densitySum));
}
