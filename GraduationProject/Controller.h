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
	//场景容器
	std::map<std::string, Scene*> Scenes;

	//控制整体的脚本
	defaultControllerBehaviour* controllerBehaviour;

	//窗口
	Window* window;

	//当前作用的场景
	std::string nowScene;

	//背景颜色
	glm::vec4 backGround;

public:

	ContextController();
	/*
	* 一些场景的预处理
	*/
	void start();

	/*
	* 框架主逻辑，包括检测，碰撞检测等
	*/
	void run();

	/*
	* 初始化
	*/
	void init();

	/*
	* 最后的回收操作
	*/
	void end();
};

class Scene {
public:
	//场景中的对象集合
	std::vector<FrameObject*> frameObjects;
	//场景名字
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
	//窗口名字
	const char* title;
	//宽度
	float width;
	//高度
	float height;
	//窗口
	GLFWwindow* window;

public:
	Window(const char* title = DEFAULT_WINDOW_NAME.c_str(), GLFWwindow* window = NULL, float width = WIDTH, float height = HEIGHT);

	void init();

	void end();
private:

};

/*
* 控制器脚本
*/
class ControllerBehaviour
{
public:

	//物体被创建
	virtual void start();

	//每一帧的变化
	virtual void update();


	//固定帧的变化
	virtual void fixUpdate();

	//碰撞时
	virtual void onCollision();

	//碰撞之后
	virtual void afterCollision(std::set<FrameObject*>& set);

	//物体被回收
	virtual void end();

private:

};

/*
* 系统默认实现脚本
*/
class defaultControllerBehaviour : public ControllerBehaviour
{
public:
	//脚本关联上下文控制器
	ContextController* contextController;
	//下面三个时间参数用于决定固定帧的调用次数

	//用于fixupdate
	float fixTime;

	//当前时间
	float currentTime;
	//时间间隔
	float deltaTime = 0.0f;
	//最后的时间
	float lastTime = 0.0f;

	bool isEnd;
public:
	defaultControllerBehaviour(ContextController* contextController, float fixTime = FIXTIME);

	//物体被创建
	void start();

	//每一帧的变化
	void update();


	/*
	*固定帧的变化
	*/
	void fixUpdate();

	/*
	* 绘制所有对象
	*/
	void draw(Scene* scene);

	//碰撞时
	void onCollision();

	//碰撞之后
	void afterCollision(std::set<FrameObject*>& set);

	//物体被回收
	void end();

	/*
	*计算并更新时间
	*/
	void calculateTime();

	/*
	* 获取绑定的上下文控制器中当前场景的对象vector
	*/
	std::vector<FrameObject*>& getNowSceneAllFrameObjects();

	/*
	* 获取当前的场景
	*/
	Scene* getNowScene();

	/*
	* 注册回调事件（只需注册一次）
	*/
	void preProcessInput();

	/*
	* 注册IO事件
	*/
	void processInput();
	

	~defaultControllerBehaviour();

private:

};

class SkyBox
{
public:
	//中心点位置
	glm::vec3 center;

	//各个面的贴图路径
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
	//辐照度
	unsigned int irradianceMap;
	//立方体Shader
	Shader* equireToCubeMapShader;
	//辐照图Shader
	Shader* irradianceShader;
	//预滤波Shader
	Shader* prefilterShader;
	//预过滤立方体贴图
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