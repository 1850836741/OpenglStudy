#ifndef CONFIGURE_H
#define CONFIGURE_H

#include<glm/glm.hpp>
/*
全局的配置变量
Creater: chenyang
CreationTime: 2021/2/4
RevisionTime: 2021/2/4
*/

using namespace glm;
//场景中世界的中心坐标
static vec3 WORLD_CENTER(0.0f,0.0f,0.0f);

//屏幕宽度
static int WIDTH = 1280;
//屏幕高度
static int HEIGHT = 720;

//屏幕背景色
static glm::vec4 BACKGROUND_COLOR = glm::vec4(0.0,0.1,1.6,1.0);

static std::string DEFAULT_WINDOW_NAME = "defaultWindow";
static std::string DEFAULT_SCENE_NAME = "defaultSceneName";

static float FIXTIME = 0.1;

//绝对的向上标尺，用于各个组件计算自身坐标系等用处
static vec3 UP_STANDARD(0.0f,1.0f,0.0f);


//检测错误的类型
static std::string VERTEX_CHECK_TYPE = "VERTEX";
static std::string FRAGMENT_CHECK_TYPE = "FRAGMENT";
static std::string GEOMETRY_CHECK_TYPE = "GEOMETRY";


//漫反射纹理
static std::string TEXTURE_DIFFUSE = "texture_diffuse";
//高光纹理
static std::string TEXTURE_SPECULAR = "texture_specular";
//法向量纹理
static std::string TEXTURE_NORMAL = "texture_normal";
//高度纹理
static std::string TEXTURE_HEIGHT = "texture_height";
//天空盒纹理
static std::string TEXTURE_SKYBOX = "skyBox";
//万向阴影贴图
static std::string TEXTURE_SHADOW = "shadows";
//光照幅度
static std::string TEXTURE_IRRADIANCE = "irradianceMap";
//预过滤立方体贴图
static std::string TEXTURE_PREFILTER = "prefilterMap";
//2D LUT
static std::string TEXTURE_BRDFLUT = "brdfLUT";


//是否开启opengl深度测试
static bool DEPTH_TEST = false;

//用于碰撞检测的精确值
static float EXACT_VALUE = 0.05f;

//一个纬度粒子数目
static const int SPHERE_LINE_NUMBER = 100;
//一个经度粒子数目
static const int SPHERE_COLUMN_NUMBER = 100;
//圆的半径
static const float SPHERER = 1.0f;
//Π的值
const float PI = 3.14159265358979323846f;

//组件名称
static std::string TRANSFORM = "transform";
static std::string MESH = "mesh";
static std::string MODEL = "model";
static std::string MATERIAL = "material";
static std::string SHADER = "shader";
static std::string CAMERA = "camera";
static std::string COLLISIONALBODY = "collisionalBody";
static std::string ATTRIBUTE = "attribute";

static std::string DEFAULT_RESOURCE_PATH = "./resource/";



static float ENERGY_LOSS = 0.95f;
static glm::vec3 GRAVITY = glm::vec3(0.0f,-0.098f,0.0f);
#endif