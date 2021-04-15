#ifndef CONTROLLER_H
#define CONTROLLER_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<memory>
#include<vector>
#include <map>
#include <glm/glm/detail/type_vec.hpp>
#include "Component.h"
#include <set>

class Scene;
class defaultControllerBehaviour;
class Window;
class SkyBox;
class Shadow;
class Pbr;

class ContextController {
public:
	//��������
	std::map<std::string, Scene*> Scenes;

	//��������Ľű�
	defaultControllerBehaviour* controllerBehaviour;

	//����
	Window* window;

	//��ǰ���õĳ���
	std::string nowScene;

	//������ɫ
	glm::vec4 backGround;

public:

	ContextController();
	/*
	* һЩ������Ԥ����
	*/
	void start();

	/*
	* ������߼���������⣬��ײ����
	*/
	void run();

	/*
	* ��ʼ��
	*/
	void init();

	/*
	* ���Ļ��ղ���
	*/
	void end();
};

class Scene {
public:
	//�����еĶ��󼯺�
	std::vector<FrameObject*> frameObjects;
	//��������
	std::string sceneName;
	bool useSky;
	bool useShadow;
	bool usePbr;
	SkyBox* skyBox;
	Shadow* shadow;
	Pbr* pbr;
	glm::vec3 lightPos;
public:
	Scene(std::string sceneName = DEFAULT_SCENE_NAME);
	Scene(std::vector<FrameObject*> frameObjects, std::string sceneName);
	void init();
};


class Window
{
public:
	//��������
	const char* title;
	//���
	float width;
	//�߶�
	float height;
	//����
	GLFWwindow* window;

public:
	Window(const char* title = DEFAULT_WINDOW_NAME.c_str(), GLFWwindow* window = NULL, float width = WIDTH, float height = HEIGHT);

	void init();

	void end();
private:

};

/*
* �������ű�
*/
class ControllerBehaviour
{
public:

	//���屻����
	virtual void start();

	//ÿһ֡�ı仯
	virtual void update();


	//�̶�֡�ı仯
	virtual void fixUpdate();

	//��ײʱ
	virtual void onCollision();

	//��ײ֮��
	virtual void afterCollision(std::set<FrameObject*>& set);

	//���屻����
	virtual void end();

private:

};

/*
* ϵͳĬ��ʵ�ֽű�
*/
class defaultControllerBehaviour : public ControllerBehaviour
{
public:
	//�ű����������Ŀ�����
	ContextController* contextController;
	//��������ʱ��������ھ����̶�֡�ĵ��ô���

	//����fixupdate
	float fixTime;

	//��ǰʱ��
	float currentTime;
	//ʱ����
	float deltaTime = 0.0f;
	//����ʱ��
	float lastTime = 0.0f;

	bool isEnd;
public:
	defaultControllerBehaviour(ContextController* contextController, float fixTime = FIXTIME);

	//���屻����
	void start();

	//ÿһ֡�ı仯
	void update();


	/*
	*�̶�֡�ı仯
	*/
	void fixUpdate();

	/*
	* �������ж���
	*/
	void draw(Scene* scene);

	//��ײʱ
	void onCollision();

	//��ײ֮��
	void afterCollision(std::set<FrameObject*>& set);

	//���屻����
	void end();

	/*
	*���㲢����ʱ��
	*/
	void calculateTime();

	/*
	* ��ȡ�󶨵������Ŀ������е�ǰ�����Ķ���vector
	*/
	std::vector<FrameObject*>& getNowSceneAllFrameObjects();

	/*
	* ��ȡ��ǰ�ĳ���
	*/
	Scene* getNowScene();

	/*
	* ע��ص��¼���ֻ��ע��һ�Σ�
	*/
	void preProcessInput();

	/*
	* ע��IO�¼�
	*/
	void processInput();
	

	~defaultControllerBehaviour();

private:

};

class SkyBox
{
public:
	//���ĵ�λ��
	glm::vec3 center;

	//���������ͼ·��
	std::string frontPath;
	std::string backPath;
	std::string topPath;
	std::string bottomPath;
	std::string leftPath;
	std::string rightPath;

	unsigned int skyBoxTextureID;
	unsigned int skyBoxVAO;

	Shader* shader;

public:
	SkyBox(std::string frontPath, std::string backPath, 
		std::string topPath, std::string bottomPath, 
		std::string leftPath, std::string rightPath, glm::vec3 center = glm::vec3(0.0f));

	void Draw();
	void initSomething();
	void loadCubemap();
private:

};

class Shadow
{
public:
	unsigned int shadowCubeMapID;
	GLuint shadow_width;
	GLuint shadow_height;
	Shader* simpleDepthShader;

public:
	Shadow(GLuint shadow_width = WIDTH, GLuint shadow_height = WIDTH);
	void init(std::vector<FrameObject*> &frameObjects,glm::vec3& lightPos);
	void RenderScene(std::vector<FrameObject*> &frameObjects);
private:

};

class Pbr
{
public:
	std::string hdrName;
	unsigned int pbrBoxTextureID;
	unsigned int pbrBoxVAO;
	//���ն�
	unsigned int irradianceMap;
	//������Shader
	Shader* equireToCubeMapShader;
	//����ͼShader
	Shader* irradianceShader;
	//Ԥ�˲�Shader
	Shader* prefilterShader;
	//Ԥ������������ͼ
	unsigned int prefilterMap;
	//2D LUT
	unsigned int brdfLUTTexture;
	Shader* brdfShader;
	Shader* pbrShader;

	unsigned int quadVAO;
public:
	Pbr(std::string hdrName);
	void init();
	void renderCube();
	void renderQuad();
private:

};

#endif 