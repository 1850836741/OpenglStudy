#ifndef COMPONENT_H
#define COMPONENT_H
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#include"Configure.h"
#include<map>
#include<vector>
#include<string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GLFW/glfw3.h>
#include<limits.h>

class MonoBehaviour;
class Component;
class FrameObject;
class Collision;

/*
* 脚本基类
*/
class MonoBehaviour
{
public:
	//脚本关联对象
	FrameObject*  frameObject;
public:
	MonoBehaviour();

	MonoBehaviour(FrameObject* frameObject);

	//物体被创建
	virtual void start();

	//每一帧的变化
	virtual void update();

	//固定帧的变化
	virtual void fixUpdate();

	//碰撞时
	virtual void onCollision(Collision* collision);

	//碰撞之后
	virtual void afterCollision();

	//物体被回收
	virtual void end();
private:

};


/*
框架组件，单个物体对象添加对应的组件可以获取相对应的功能
Creater: chenyang
CreationTime: 2021/2/4
RevisionTime: 2021/2/4
*/
class Component {
public:
	virtual void doSomething();
	virtual void setFrameObject(FrameObject* frameObject);
};


/*
框架的对象类
Creater: chenyang
CreationTime: 2021/2/4
RevisionTime: 2021/2/4
*/
class FrameObject {
public:
	//Transform transform;

	//父对象
	FrameObject* parent;

	//子对象
	std::vector<FrameObject*> children;

	//组件集合
	std::map<std::string, Component*> componentMap;

	//脚本集合
	std::vector<MonoBehaviour> monoBehaviours;

public:
	FrameObject();
	void addComponent(std::string componentName, Component* component);
};


class Transform : public Component
{
public:
	//所属对象
	FrameObject* frameObject;
	//位置
	glm::vec3 position;
	//变换大小
	glm::vec3 scale;
	//旋转角度
	glm::vec3 rotation;
	//X轴
	glm::vec3 x;
	//Y轴
	glm::vec3 y;
	//Z轴
	glm::vec3 z;
public:
	Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f));

	//根据transform中的旋转角度来改变坐标系
	void updateSystem();
	//设置组件所依赖的物体对象
	void setFrameObject(FrameObject* frameObject);
	//平移
	void Translate(glm::vec3 trans);
	//旋转
	void Rotate(glm::vec3 trans);
	//放大缩小
	void Scale(glm::vec3 trans);

private:
	//根据Z轴来计算坐标系
	void calculateXYZ(glm::vec3 z);
};


/*
* 相机组件
*/
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
class Camera : public Component
{
public:
	FrameObject* frameObject;
	Transform* transform;

	//相机移动的速度
	float movementSpeed;
	//鼠标灵敏度
	float mouseSensitivity;
	//缩放度，鼠标滚轮进行缩放画面（前进后退的效果）
	float zoom;
	//相机是否可以自由移动 (键盘输入变为相机的位置变换)
	bool isFreeMovement;
	//第一次位置
	bool firstMouse = true;

	//上一次鼠标X位置
	float mouseLastX;
	//上一次鼠标Y位置
	float mouseLastY;

	//屏幕宽度
	int width;
	//屏幕高度
	int height;
	//近平面距离
	float nearPlane;
	//远平面距离
	float farPlane;
	//视角
	float visualAngle;

public:
	Camera(
		int width = WIDTH, int height = HEIGHT,
		float movementSpeed = 1.0f,
		float mouseSensitivity = 1.0f,
		float visualAngle = 45.0f);

	//初始化一些参数
	void init();
	//设置摄像机所依赖的物体对象
	void setFrameObject(FrameObject* frameObject);

	//获取投影变换矩阵
	glm::mat4 getProjectionMatrix();
	
	//获取物体坐标系的View变换矩阵
	glm::mat4 getViewMatrix();

	//鼠标滚轮拉近拉远视角的约束
	void ProcessMouseScroll(float zoffset);
	//鼠标移动
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	//键盘按键事件
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	//注册移动函数，使相机可以自由移动
	void setFreeMovement();

};

/*
* 顶点的信息
*/
struct Vertex {
	//位置
	glm::vec3 position;

	//法线坐标
	glm::vec3 normal;

	//纹理坐标
	glm::vec2 texCoords;

	//切线空间双切线之一,用于凹凸贴图
	glm::vec3 tangent;

	//切线空间双切线之一,用于凹凸贴图
	glm::vec3 bitangent;
};

/*
* 纹理贴图
*/
struct Texture {
	unsigned int id;
	std::string type;
	//纹理文件名，不是全部路径
	std::string path;
	//没有纹理所使用的颜色
	glm::vec4 color;
	//是否使用基础颜色
	bool isColor = false;
};



class Shader : public Component
{
public:
	//shader的ID标志
	unsigned int ID;
	const char* vertexPath;
	const char* fragmentPath;
	const char* geometryPath;
	const char* vertexCode;
	const char* fragmentCode;
	const char* geometryCode;

public:
	Shader(const char* vertexPath, const char* fragmentPath = nullptr, const char* geometryPath = nullptr);

	void init();
	/*
	* 编译，链接，绑定绑定shader文件
	* param ptr (shader的存储的映射位置)
	* param code (shader代码)
	*/
	void bindShader(unsigned int& ptr, const char* code, std::string type);

	/*
	* 为ID和shader代码建立联系
	*/
	void linkShader(unsigned int& vertex, unsigned int& fragment, unsigned int& geometry);
	/*
	* 检查shader文件编译，链接是否有错误
	* param shader (shader id)
	* param type (检查类型)
	*/
	void checkCompileErrors(GLuint shader, std::string type);

	/*
	* 激活该Shader
	*/
	void use();

	/*
	* 获取shader中变量的物理位置
	*/
	GLint getUniformLocation(const std::string& paramName) const;

	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const;
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const;

	void setVec2(const std::string& name, float x, float y) const;
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const;

	void setVec3(const std::string& name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const;

	void setVec4(const std::string& name, float x, float y, float z, float w) const;

	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const;

	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const;

	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	/*
	* 销毁内部指针(只是改变指向，真实的内存释放交给使用方)
	*/
	void Destory();

	~Shader();
private:

};



/*
* 物体顶点，法线，纹理的集合
*/
class Mesh : public Component
{
public:
	//顶点信息
	std::vector<Vertex> vertices;

	//索引数据
	std::vector<unsigned int> indices;

	//纹理数据
	std::vector<Texture> textures;

	//绑定的顶点映射
	unsigned int VAO;
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	/*
	* 分配GPU内存，并创建映射关系
	*/
	void init();

	/*
	* 绘制
	*/
	void Draw(Shader& shader, bool shadowJudge = false);

private:
	unsigned int VBO, EBO;
};

/*
* 模型类
*/
class Model : public Component
{
public:
	FrameObject* frameObject;
	std::vector<Texture> textures;
	std::vector<Mesh> meshs;
	std::string directory;
	bool gammaCorrection;
public:
	Model();

	//需传入完整路径
	Model(std::string const& path, bool gamma = false);

	void Draw(Shader& shader,bool shadowJudge = false);
private:
	//加载模组
	void loadModel(std::string const& path);

	/*
	* 解析Node
	*/
	void processNode(aiNode* node, const aiScene* scene);

	/*
	* 将aiMesh转化为mesh
	*/
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//加载材质
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	/*
    * 将纹理绑定至管线
    */
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	/*
	* 设置依赖对象
	*/
	void setFrameObject(FrameObject* frameObject);
};



/*
* CollisionalBody类
*/
class CollisionalBody : public Component
{
public:
	std::vector<glm::vec3> body;
	FrameObject* frameObject;

	glm::vec3 center;
public:
	CollisionalBody(FrameObject* frameObject = nullptr);
	void init();
	Collision CollisionalEnter();
	/*
	* 设置依赖对象
	*/
	void setFrameObject(FrameObject* frameObject);
private:

};


/*
* 记录碰撞信息
*/
class Collision
{
public:
	//碰撞点
	glm::vec3 collisionPoint;

	//碰撞物体
	FrameObject* one;

	//另一个碰撞物体
	FrameObject* other;
public:
	Collision() {}

private:

};

class FrameObjectFactory
{

public:
	FrameObjectFactory();
	~FrameObjectFactory();


	/*
	* 返回正方形mesh
	*/
	static Mesh getCubeMesh(std::vector<Texture> &textures, glm::vec3& point = WORLD_CENTER);

	/*
	* 返回球形mesh，points为球心
	*/
	static Mesh getSphereMesh(std::vector<Texture> &textures, glm::vec3& point = WORLD_CENTER);

	/*
	* todo 获取圆柱的
	*/
	static Mesh getCylindricalMesh(glm::vec3& point);
private:

};

/*
* 存储对象物质属性
*/
class Attribute : public Component {
public:
	bool useGravity;
	FrameObject* fameObject;
	//对象的合力
	glm::vec3 force;
	//对象的最终速度
	glm::vec3 speed;
	float mass;
public:
	Attribute(FrameObject* frameObject=nullptr, bool useGravity = false, float mass=1.0f);
	/*
	* 设置依赖对象
	*/
	void setFrameObject(FrameObject* frameObject);
};


class ParticleSystem
{
public:
	//粒子系统的位移
	glm::vec3 position;
	//计算shader
	Shader* computeShader;
	Shader* shader;
	//行数
	float lineNum;
	//列数
	float columnNum;
	unsigned int VAO;
	glm::vec4* particlePos;
	glm::vec4* particleSpeed;
	union
	{
		struct
		{
			GLuint position_buffer;
			GLuint velocity_buffer;
		};
		GLuint buffers[2];
	};

	union
	{
		struct
		{
			GLuint position_tbo;
			GLuint velocity_tbo;
		};
		GLuint tbos[2];
	};
public:
	ParticleSystem(glm::vec3 position, Shader* shader,Shader* computeShader,float lineNum,float columnNum);
	//初始化顶点索引
	//绑定一些数据
	void init();
	void bind();
	void Draw(glm::mat4 viewProjection,float dt);
private:

};
#endif