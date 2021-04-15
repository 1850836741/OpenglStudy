#ifndef CONFIGURE_H
#define CONFIGURE_H

#include<glm/glm.hpp>
/*
ȫ�ֵ����ñ���
Creater: chenyang
CreationTime: 2021/2/4
RevisionTime: 2021/2/4
*/

using namespace glm;
//�������������������
static vec3 WORLD_CENTER(0.0f,0.0f,0.0f);

//��Ļ���
static int WIDTH = 1280;
//��Ļ�߶�
static int HEIGHT = 720;

//��Ļ����ɫ
static glm::vec4 BACKGROUND_COLOR = glm::vec4(0.0,0.1,1.6,1.0);

static std::string DEFAULT_WINDOW_NAME = "defaultWindow";
static std::string DEFAULT_SCENE_NAME = "defaultSceneName";

static float FIXTIME = 0.1;

//���Ե����ϱ�ߣ����ڸ������������������ϵ���ô�
static vec3 UP_STANDARD(0.0f,1.0f,0.0f);


//�����������
static std::string VERTEX_CHECK_TYPE = "VERTEX";
static std::string FRAGMENT_CHECK_TYPE = "FRAGMENT";
static std::string GEOMETRY_CHECK_TYPE = "GEOMETRY";


//����������
static std::string TEXTURE_DIFFUSE = "texture_diffuse";
//�߹�����
static std::string TEXTURE_SPECULAR = "texture_specular";
//����������
static std::string TEXTURE_NORMAL = "texture_normal";
//�߶�����
static std::string TEXTURE_HEIGHT = "texture_height";
//��պ�����
static std::string TEXTURE_SKYBOX = "skyBox";
//������Ӱ��ͼ
static std::string TEXTURE_SHADOW = "shadows";
//���շ���
static std::string TEXTURE_IRRADIANCE = "irradianceMap";
//Ԥ������������ͼ
static std::string TEXTURE_PREFILTER = "prefilterMap";
//2D LUT
static std::string TEXTURE_BRDFLUT = "brdfLUT";


//�Ƿ���opengl��Ȳ���
static bool DEPTH_TEST = false;

//������ײ���ľ�ȷֵ
static float EXACT_VALUE = 0.05f;

//һ��γ��������Ŀ
static const int SPHERE_LINE_NUMBER = 100;
//һ������������Ŀ
static const int SPHERE_COLUMN_NUMBER = 100;
//Բ�İ뾶
static const float SPHERER = 1.0f;
//����ֵ
const float PI = 3.14159265358979323846f;

//�������
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