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
* �ű�����
*/
class MonoBehaviour
{
public:
	//�ű���������
	FrameObject*  frameObject;
public:
	MonoBehaviour();

	MonoBehaviour(FrameObject* frameObject);

	//���屻����
	virtual void start();

	//ÿһ֡�ı仯
	virtual void update();

	//�̶�֡�ı仯
	virtual void fixUpdate();

	//��ײʱ
	virtual void onCollision(Collision* collision);

	//��ײ֮��
	virtual void afterCollision();

	//���屻����
	virtual void end();
private:

};


/*
���������������������Ӷ�Ӧ��������Ի�ȡ���Ӧ�Ĺ���
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
��ܵĶ�����
Creater: chenyang
CreationTime: 2021/2/4
RevisionTime: 2021/2/4
*/
class FrameObject {
public:
	//Transform transform;

	//������
	FrameObject* parent;

	//�Ӷ���
	std::vector<FrameObject*> children;

	//�������
	std::map<std::string, Component*> componentMap;

	//�ű�����
	std::vector<MonoBehaviour> monoBehaviours;

public:
	FrameObject();
	void addComponent(std::string componentName, Component* component);
};


class Transform : public Component
{
public:
	//��������
	FrameObject* frameObject;
	//λ��
	glm::vec3 position;
	//�任��С
	glm::vec3 scale;
	//��ת�Ƕ�
	glm::vec3 rotation;
	//X��
	glm::vec3 x;
	//Y��
	glm::vec3 y;
	//Z��
	glm::vec3 z;
public:
	Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f));

	//����transform�е���ת�Ƕ����ı�����ϵ
	void updateSystem();
	//����������������������
	void setFrameObject(FrameObject* frameObject);
	//ƽ��
	void Translate(glm::vec3 trans);
	//��ת
	void Rotate(glm::vec3 trans);
	//�Ŵ���С
	void Scale(glm::vec3 trans);

private:
	//����Z������������ϵ
	void calculateXYZ(glm::vec3 z);
};


/*
* ������
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

	//����ƶ����ٶ�
	float movementSpeed;
	//���������
	float mouseSensitivity;
	//���Ŷȣ������ֽ������Ż��棨ǰ�����˵�Ч����
	float zoom;
	//����Ƿ���������ƶ� (���������Ϊ�����λ�ñ任)
	bool isFreeMovement;
	//��һ��λ��
	bool firstMouse = true;

	//��һ�����Xλ��
	float mouseLastX;
	//��һ�����Yλ��
	float mouseLastY;

	//��Ļ���
	int width;
	//��Ļ�߶�
	int height;
	//��ƽ�����
	float nearPlane;
	//Զƽ�����
	float farPlane;
	//�ӽ�
	float visualAngle;

public:
	Camera(
		int width = WIDTH, int height = HEIGHT,
		float movementSpeed = 1.0f,
		float mouseSensitivity = 1.0f,
		float visualAngle = 45.0f);

	//��ʼ��һЩ����
	void init();
	//������������������������
	void setFrameObject(FrameObject* frameObject);

	//��ȡͶӰ�任����
	glm::mat4 getProjectionMatrix();
	
	//��ȡ��������ϵ��View�任����
	glm::mat4 getViewMatrix();

	//������������Զ�ӽǵ�Լ��
	void ProcessMouseScroll(float zoffset);
	//����ƶ�
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	//���̰����¼�
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	//ע���ƶ�������ʹ������������ƶ�
	void setFreeMovement();

};

/*
* �������Ϣ
*/
struct Vertex {
	//λ��
	glm::vec3 position;

	//��������
	glm::vec3 normal;

	//��������
	glm::vec2 texCoords;

	//���߿ռ�˫����֮һ,���ڰ�͹��ͼ
	glm::vec3 tangent;

	//���߿ռ�˫����֮һ,���ڰ�͹��ͼ
	glm::vec3 bitangent;
};

/*
* ������ͼ
*/
struct Texture {
	unsigned int id;
	std::string type;
	//�����ļ���������ȫ��·��
	std::string path;
	//û��������ʹ�õ���ɫ
	glm::vec4 color;
	//�Ƿ�ʹ�û�����ɫ
	bool isColor = false;
};



class Shader : public Component
{
public:
	//shader��ID��־
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
	* ���룬���ӣ��󶨰�shader�ļ�
	* param ptr (shader�Ĵ洢��ӳ��λ��)
	* param code (shader����)
	*/
	void bindShader(unsigned int& ptr, const char* code, std::string type);

	/*
	* ΪID��shader���뽨����ϵ
	*/
	void linkShader(unsigned int& vertex, unsigned int& fragment, unsigned int& geometry);
	/*
	* ���shader�ļ����룬�����Ƿ��д���
	* param shader (shader id)
	* param type (�������)
	*/
	void checkCompileErrors(GLuint shader, std::string type);

	/*
	* �����Shader
	*/
	void use();

	/*
	* ��ȡshader�б���������λ��
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
	* �����ڲ�ָ��(ֻ�Ǹı�ָ����ʵ���ڴ��ͷŽ���ʹ�÷�)
	*/
	void Destory();

	~Shader();
private:

};



/*
* ���嶥�㣬���ߣ�����ļ���
*/
class Mesh : public Component
{
public:
	//������Ϣ
	std::vector<Vertex> vertices;

	//��������
	std::vector<unsigned int> indices;

	//��������
	std::vector<Texture> textures;

	//�󶨵Ķ���ӳ��
	unsigned int VAO;
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	/*
	* ����GPU�ڴ棬������ӳ���ϵ
	*/
	void init();

	/*
	* ����
	*/
	void Draw(Shader& shader, bool shadowJudge = false);

private:
	unsigned int VBO, EBO;
};

/*
* ģ����
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

	//�贫������·��
	Model(std::string const& path, bool gamma = false);

	void Draw(Shader& shader,bool shadowJudge = false);
private:
	//����ģ��
	void loadModel(std::string const& path);

	/*
	* ����Node
	*/
	void processNode(aiNode* node, const aiScene* scene);

	/*
	* ��aiMeshת��Ϊmesh
	*/
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//���ز���
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	/*
    * �������������
    */
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	/*
	* ������������
	*/
	void setFrameObject(FrameObject* frameObject);
};



/*
* CollisionalBody��
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
	* ������������
	*/
	void setFrameObject(FrameObject* frameObject);
private:

};


/*
* ��¼��ײ��Ϣ
*/
class Collision
{
public:
	//��ײ��
	glm::vec3 collisionPoint;

	//��ײ����
	FrameObject* one;

	//��һ����ײ����
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
	* ����������mesh
	*/
	static Mesh getCubeMesh(std::vector<Texture> &textures, glm::vec3& point = WORLD_CENTER);

	/*
	* ��������mesh��pointsΪ����
	*/
	static Mesh getSphereMesh(std::vector<Texture> &textures, glm::vec3& point = WORLD_CENTER);

	/*
	* todo ��ȡԲ����
	*/
	static Mesh getCylindricalMesh(glm::vec3& point);
private:

};

/*
* �洢������������
*/
class Attribute : public Component {
public:
	bool useGravity;
	FrameObject* fameObject;
	//����ĺ���
	glm::vec3 force;
	//����������ٶ�
	glm::vec3 speed;
	float mass;
public:
	Attribute(FrameObject* frameObject=nullptr, bool useGravity = false, float mass=1.0f);
	/*
	* ������������
	*/
	void setFrameObject(FrameObject* frameObject);
};


class ParticleSystem
{
public:
	//����ϵͳ��λ��
	glm::vec3 position;
	//����shader
	Shader* computeShader;
	Shader* shader;
	//����
	float lineNum;
	//����
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
	//��ʼ����������
	//��һЩ����
	void init();
	void bind();
	void Draw(glm::mat4 viewProjection,float dt);
private:

};
#endif