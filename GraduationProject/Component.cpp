#include "Component.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#include "gl3w.h"
void Component::doSomething()
{
	//todo
}

void Component::setFrameObject(FrameObject* frameObject)
{
}


/*
* Transform类
*/
Transform::Transform(glm::vec3 position, glm::vec3 z)
	:position(position),
	z(z) {
	calculateXYZ(z);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
};

void Transform::updateSystem() {
	this->z.x = sin(glm::radians(rotation.y / PI)) * cos(glm::radians(rotation.x / PI));
	this->z.y = sin(glm::radians(rotation.x / PI));
	this->z.z = cos(glm::radians(rotation.y / PI)) * cos(glm::radians(rotation.x / PI));
	
	calculateXYZ(z);
}

void Transform::setFrameObject(FrameObject* frameObject) {
	this->frameObject = frameObject;
}

void Transform::calculateXYZ(glm::vec3 z) {
	this->z = glm::normalize(z);
	this->x = glm::normalize(glm::cross(z, UP_STANDARD));
	this->y = -glm::normalize(glm::cross(x, z));
}

void Transform::Translate(glm::vec3 trans){
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model,trans);
	position = model * glm::vec4(position,1.0f);

	std::vector<FrameObject*> vec = this->frameObject->children;
	Transform* tra = nullptr;
	for (int i = 0; i < vec.size(); i++)
	{
		tra = dynamic_cast<Transform*>(vec[i]->componentMap[TRANSFORM]);
		tra->position = model * glm::vec4(tra->position, 1.0f);
	}
	tra = nullptr;
	delete tra;
}
void Transform::Rotate(glm::vec3 trans) {
	glm::mat4 modelX = glm::mat4(1.0f);
	glm::mat4 modelY = glm::mat4(1.0f);
	glm::mat4 modelZ = glm::mat4(1.0f);
	glm::rotate(modelX, glm::radians(trans.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::rotate(modelY, glm::radians(trans.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::rotate(modelZ, glm::radians(trans.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 allModel = modelX * modelY * modelZ;
	position = allModel * glm::vec4(position, 1.0f);

	std::vector<FrameObject*> vec = this->frameObject->children;
	Transform* tra = nullptr;
	for (int i = 0; i < vec.size(); i++)
	{
		tra = dynamic_cast<Transform*>(vec[i]->componentMap[TRANSFORM]);
		tra->position = allModel * glm::vec4(tra->position, 1.0f);
	}
	tra = nullptr;
	delete tra;
}
void Transform::Scale(glm::vec3 trans) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, trans);
	position = model * glm::vec4(position, 1.0f);

	std::vector<FrameObject*> vec = this->frameObject->children;
	Transform* tra = nullptr;
	for (int i = 0; i < vec.size(); i++)
	{
		tra = dynamic_cast<Transform*>(vec[i]->componentMap[TRANSFORM]);
		tra->position = model * glm::vec4(tra->position, 1.0f);
	}
	tra = nullptr;
	delete tra;
}
/*
* Camera类
*/
Camera::Camera(
	int width, int height,
	float movementSpeed,
	float mouseSensitivity,
	float visualAngle)
	: movementSpeed(movementSpeed),
	mouseSensitivity(mouseSensitivity),
	width(width),
	height(height),
	visualAngle(visualAngle) {
	init();
}

void Camera::init() {
	nearPlane = 0.1f;
	farPlane = 100.0f;
}

void Camera::setFrameObject(FrameObject* frameObject) {
	this->frameObject = frameObject;
	transform = dynamic_cast<Transform*>(frameObject->componentMap[TRANSFORM]);
}

glm::mat4 Camera::getProjectionMatrix() {
	return glm::perspective(glm::radians(visualAngle), (float)width / (float)height, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() {
	glm::vec3 position = this->transform->position;
	return glm::lookAt(position, position + transform->z, UP_STANDARD);
}

void Camera::ProcessMouseScroll(float zoffset)
{
	zoom -= (float)zoffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	//位移乘以鼠标灵敏度
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;
	//鼠标x方向移动等同于绕y轴旋转
	transform->rotation.y -= xoffset;
	//鼠标y方向移动等同于绕x轴旋转
	transform->rotation.x += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	//开启了约束螺旋
	if (constrainPitch)
	{
		if (transform->rotation.x > 89.0f)
			transform->rotation.x = 89.0f;
		if (transform->rotation.x < -89.0f)
			transform->rotation.x = -89.0f;
	}

	transform->updateSystem();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	//最终移动速度
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		transform->position += transform->z * velocity;
	if (direction == BACKWARD)
		transform->position -= transform->z * velocity;
	if (direction == LEFT)
		transform->position -= transform->x * velocity;
	if (direction == RIGHT)
		transform->position += transform->x * velocity;
}

void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - mouseLastX;
	float yoffset = mouseLastY - ypos;

	mouseLastX = xpos;
	mouseLastY = ypos;

	ProcessMouseMovement(xoffset, yoffset);
	ProcessMouseScroll(yoffset);
}

void Camera::setFreeMovement() {
	isFreeMovement = true;
}


/*
* Mesh类
*/
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	init();
}

void Mesh::init() {

	//创建内存及映射关系
	glad_glGenVertexArrays(1, &VAO);
	glad_glGenBuffers(1, &VBO);
	glad_glGenBuffers(1, &EBO);

	//绑定该VAO
	glad_glBindVertexArray(VAO);

	//绑定顶点映射
	glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//绑定数据
	glad_glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//绑定索引映射
	glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//绑定数据
	glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//向opengl解释如何读取数据
	//顶点位置
	glad_glEnableVertexAttribArray(0);
	glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//顶点法线
	glad_glEnableVertexAttribArray(1);
	glad_glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//顶点纹理坐标
	glad_glEnableVertexAttribArray(2);
	glad_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	//顶点切线空间中的双切线
	glad_glEnableVertexAttribArray(3);
	glad_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glad_glEnableVertexAttribArray(4);
	glad_glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glad_glBindVertexArray(0);

}

void Mesh::Draw(Shader& shader, bool shadowJudge) {
	//漫反射贴图数量
	int diffuseNumber = 0;
	//高光贴图数量
	int specularNumber = 0;
	//法线贴图数量
	int normalNumber = 0;
	//高度贴图数量
	int heightNumber = 0;
	for (int i = 0; i < textures.size(); i++)
	{
		if (shadowJudge)
		{
			break;
		}
		if (textures[i].isColor)
		{    
			shader.setBool("useColor", textures[i].isColor);
			shader.setVec4("color", textures[i].color);
			break;
		}
		//激活这块区域
		glad_glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == TEXTURE_DIFFUSE)
			number = std::to_string(++diffuseNumber);
		else if (name == TEXTURE_SPECULAR)
			number = std::to_string(++specularNumber);
		else if (name == TEXTURE_NORMAL)
			number = std::to_string(++normalNumber);
		else if (name == TEXTURE_HEIGHT)
			number = std::to_string(++heightNumber);
		//天空盒纹理
		else if (name == TEXTURE_SKYBOX)
		{
			glad_glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
			glad_glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
			continue;
		}
		//阴影贴图
		else if(name == TEXTURE_SHADOW)
		{
			glad_glUniform1i(glad_glGetUniformLocation(shader.ID, name.c_str()), i);
			glad_glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
			continue;
		}
		else if(name == TEXTURE_IRRADIANCE)
		{
			glad_glUniform1i(glad_glGetUniformLocation(shader.ID, name.c_str()), i);
			glad_glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
			continue;
		}
		else if (name == TEXTURE_PREFILTER)
		{
			glad_glUniform1i(glad_glGetUniformLocation(shader.ID, name.c_str()), i);
			glad_glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
			continue;
		}
		else if (name == TEXTURE_BRDFLUT)
		{
			glad_glUniform1i(glad_glGetUniformLocation(shader.ID, name.c_str()), i);
			glad_glBindTexture(GL_TEXTURE_2D, textures[i].id);
			continue;
		}
		glad_glUniform1i(glad_glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		glad_glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glad_glBindVertexArray(VAO);
	if (vertices.size()>indices.size())
	{
		glad_glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	else {
		glad_glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	glad_glBindVertexArray(0);
	if (!shadowJudge)
	{
		//重置纹理
		glad_glActiveTexture(GL_TEXTURE0);
	}
}


/*
* Model类
*/

Model::Model() {}

Model::Model(std::string const& path, bool gamma) :gammaCorrection(gamma) {
	loadModel(path);
}

void Model::Draw(Shader& shader, bool shadowJudge) {
	for (unsigned int i = 0; i < meshs.size(); i++)
	{
		meshs[i].Draw(shader, shadowJudge);
	}
}

void Model::loadModel(std::string const& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//是否初始化成功
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	//设置纹理资源文件夹
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshs.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	//临时存储
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//遍历mesh网格
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		//获取顶点的位置
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		//获取法向量
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		//获取纹理坐标
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f);
		}
		vertices.push_back(vertex);
	}

	//存储索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//获取网格的材质
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//漫反射贴图
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//高光贴图
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//法线贴图
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	//高度贴图
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TEXTURE_HEIGHT);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
	//临时存储
	std::vector<Texture> textureVec;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		//是否存在
		bool skip = false;

		//判断是否重复
		for (unsigned int j = 0; j < textures.size(); j++) {
			if (std::strcmp(textures[j].path.data(), str.C_Str()) == 0)
			{
				textureVec.push_back(textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textureVec.push_back(texture);
			textures.push_back(texture);
		}
	}
	//返回临时变量，需要在调用处创建引用来延长其生命周期
	return textureVec;
}


unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string fileName = std::string(path);
	fileName = directory + '/' + fileName;

	unsigned int textureId;
	glGenTextures(1, &textureId);

	//nrComponents为颜色类型，如RGB
	int width, height, nrComponents;
	//加载图片
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		//unsigned int
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//启用Mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		//纹理覆盖类型等属性的设置
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//释放内存
		stbi_image_free(data);
	}
	else
	{
		std::cout << "加载该路径下的纹理失败: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureId;
}


void Model::setFrameObject(FrameObject* frameObject) {
	this->frameObject = frameObject;
}


/*
* FrameObject类
*/
FrameObject::FrameObject() {
	this->componentMap = std::map<std::string, Component*>();
	this->monoBehaviours = std::vector<MonoBehaviour>();
	std::pair<std::string, Component*> transform = std::pair<std::string, Component*>(TRANSFORM, new Transform());
	this->componentMap.insert(transform);
	this->parent = NULL;
	this->children = std::vector<FrameObject*>();
};

/*
* 添加组件
*/
void FrameObject::addComponent(std::string componentName,Component* component) {
	this->componentMap.insert(std::pair<std::string, Component*>(componentName, component));
	component->setFrameObject(this);
}


/*
* CollisionalBody类
*/
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;
	this->geometryPath = geometryPath;
	init();
}

void Shader::init() {
	//给ID赋予内存
	this->ID = glad_glCreateProgram();
	std::string vertexTemp = readFileToString(vertexPath);
	std::string fragmentTemp;
	vertexCode = vertexTemp.c_str();
	if (fragmentPath !=nullptr)
	{
		fragmentTemp = readFileToString(fragmentPath);
		fragmentCode = fragmentTemp.c_str();
	}
	std::string geometryTemp;
	if (geometryPath != nullptr)
	{
		geometryTemp = readFileToString(geometryPath);
		geometryCode = geometryTemp.c_str();
	}
	unsigned int vertex, fragment, geometry;
	//初始化，绑定顶点，片段数据
	
	bindShader(vertex, vertexCode, VERTEX_CHECK_TYPE);
	if (fragmentPath != nullptr)
	{
		bindShader(fragment, fragmentCode, FRAGMENT_CHECK_TYPE);
	}
	if (geometryPath != nullptr)
	{
		bindShader(geometry, geometryCode, GEOMETRY_CHECK_TYPE);
	}
	linkShader(vertex, fragment, geometry);
}

void Shader::bindShader(unsigned int& ptr, const char* code, std::string type) {
	if (type == VERTEX_CHECK_TYPE)
	{
		ptr = glad_glCreateShader(GL_VERTEX_SHADER);
	}else if(type == FRAGMENT_CHECK_TYPE){
		ptr = glad_glCreateShader(GL_FRAGMENT_SHADER);
	}
	else if(type == GEOMETRY_CHECK_TYPE){
		ptr = glad_glCreateShader(GL_GEOMETRY_SHADER);
	}
	
	glad_glShaderSource(ptr, 1, &code, NULL);
	glad_glCompileShader(ptr);
	checkCompileErrors(ptr, type);
}

void Shader::linkShader(unsigned int& vertex, unsigned int& fragment, unsigned int& geometry) {
	glad_glAttachShader(ID, vertex);
	if (fragmentPath!=nullptr)
		glad_glAttachShader(ID, fragment);
	if (geometryPath != nullptr)
		glad_glAttachShader(ID, geometry);
	glad_glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	//删除并释放内存
	glad_glDeleteShader(vertex);
	if (fragmentPath != nullptr)
	glad_glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glad_glDeleteShader(geometry);
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glad_glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glad_glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "标号为: " << shader << "类型为: " << type
				<< "的shader文件编译错误，错误为: " << std::endl << infoLog << std::endl;
		}
	}
	else
	{
		glad_glGetShaderiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glad_glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "标号为: " << shader << "类型为: " << type
				<< "的shader文件链接错误，错误为: " << std::endl << infoLog << std::endl;
		}
	}
}

void Shader::use() {
	glad_glUseProgram(ID);
}

GLint Shader::getUniformLocation(const std::string& paramName) const {
	return glad_glGetUniformLocation(ID, paramName.c_str());
}

void Shader::setBool(const std::string& name, bool value) const {
	glad_glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glad_glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glad_glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glad_glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glad_glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glad_glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glad_glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glad_glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glad_glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glad_glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glad_glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glad_glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Destory() {
	this->vertexPath = NULL;
	this->fragmentPath = NULL;
	this->geometryPath = NULL;
	this->vertexCode = NULL;
	this->fragmentCode = NULL;
	this->geometryCode = NULL;
}

Shader::~Shader()
{
	Destory();
}


/*.
* CollisionalBody类
*/

CollisionalBody::CollisionalBody(FrameObject* frameObject)
{
	this->frameObject = frameObject;
}
void CollisionalBody::init() {
	Mesh* mesh = nullptr;
	
	mesh = dynamic_cast<Mesh*>(this->frameObject->componentMap[MESH]);
	if (mesh == nullptr)return;

	float right = std::numeric_limits<float>::min(), top = std::numeric_limits<float>::min(), front = std::numeric_limits<float>::min();
	float left = std::numeric_limits<float>::max(), bottom = std::numeric_limits<float>::max(), back = std::numeric_limits<float>::max();
	std::vector<Vertex>* vertices = &mesh->vertices;
	for (int i = 0; i < vertices->size(); i++)
	{
		Vertex tempVertex = vertices->at(i);
		if (tempVertex.position.x > right)right = tempVertex.position.x;
		if (tempVertex.position.y > top)top = tempVertex.position.y;
		if (tempVertex.position.z > front)front = tempVertex.position.z;

		if (tempVertex.position.x < left)left = tempVertex.position.x;
		if (tempVertex.position.y < bottom)bottom = tempVertex.position.y;
		if (tempVertex.position.z < back)back = tempVertex.position.z;
	}
	
	//矩形碰撞体的八个顶点
	//右上角前后顶点
	body.push_back(glm::vec3(right, top, front));
	body.push_back(glm::vec3(right, top, back));
	//右下角前后顶点
	body.push_back(glm::vec3(right, bottom, front));
	body.push_back(glm::vec3(right, bottom, back));
	//左上角前后顶点
	body.push_back(glm::vec3(left, top, front));
	body.push_back(glm::vec3(left, top, back));
	//右下角前后顶点
	body.push_back(glm::vec3(left, bottom, front));
	body.push_back(glm::vec3(left, bottom, back));

	center = glm::vec3((right + left)/2.0f,(top + bottom)/2.0f, (front + back)/2.0f);

	//六个面的中心点
	body.push_back(glm::vec3(right, (top+bottom)/2.0f, (front+back)/2.0f));
	body.push_back(glm::vec3(left, (top + bottom) / 2.0f, (front + back) / 2.0f));

	body.push_back(glm::vec3((right + left)/2.0f, top , (front + back) / 2.0f));
	body.push_back(glm::vec3((right + left) / 2.0f, bottom, (front + back) / 2.0f));

	body.push_back(glm::vec3((right + left) / 2.0f, (top + bottom) / 2.0f, front));
	body.push_back(glm::vec3((right + left) / 2.0f, (top + bottom) / 2.0f, back));
	vertices = nullptr;
	delete vertices;
	
	mesh = nullptr;
	delete mesh;
}

Collision CollisionalBody::CollisionalEnter()
{
	return Collision();
}

void CollisionalBody::setFrameObject(FrameObject* frameObject)
{
	this->frameObject = frameObject;
}




MonoBehaviour::MonoBehaviour()
{
}

MonoBehaviour::MonoBehaviour(FrameObject* frameObject) {
	this->frameObject = frameObject;
}

void MonoBehaviour::start()
{
}

void MonoBehaviour::update()
{
}

void MonoBehaviour::fixUpdate()
{
}

void MonoBehaviour::onCollision(Collision* collision)
{
}

void MonoBehaviour::afterCollision()
{
}

void MonoBehaviour::end()
{
}




FrameObjectFactory::FrameObjectFactory()
{
}

FrameObjectFactory::~FrameObjectFactory()
{
}



Mesh FrameObjectFactory::getCubeMesh(std::vector<Texture> &textures, glm::vec3& point) {
	std::vector<Vertex> vertexs = std::vector<Vertex>();
	std::vector<unsigned int> indices;
	std::vector<Texture> texture = std::vector<Texture>();
	//变换矩阵
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, point);

	glm::vec3 points[] = {
		//逆时针

		//后
		//左下角
		glm::vec3(-1.0f, -1.0f, -1.0f),
		//右下角
		glm::vec3(1.0f,  1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f, -1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f, -1.0f),
		//左上角
		glm::vec3(-1.0f, -1.0f, -1.0f),
		//左下角
		glm::vec3(-1.0f,  1.0f, -1.0f),


		//前
		//左下角
		glm::vec3(-1.0f, -1.0f,  1.0f),
		//右下角
		glm::vec3(1.0f, -1.0f,  1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f,  1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f,  1.0f),
		//左上角
		glm::vec3(-1.0f,  1.0f,  1.0f),
		//左下角
		glm::vec3(-1.0f, -1.0f,  1.0f),
		

		//右
		//左下角
		glm::vec3(-1.0f,  1.0f,  1.0f),
		//右下角
		glm::vec3(-1.0f,  1.0f, -1.0f),
		//右上角
		glm::vec3(-1.0f, -1.0f, -1.0f),
		//右上角
		glm::vec3(-1.0f, -1.0f, -1.0f),
		//左上角
		glm::vec3(-1.0f, -1.0f,  1.0f),
		//左下角
		glm::vec3(-1.0f,  1.0f,  1.0f),
		

		//左
		//左下角
		glm::vec3(1.0f,  1.0f,  1.0f),
		//右下角
		glm::vec3(1.0f, -1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f, -1.0f, -1.0f),
		//左上角
		glm::vec3(1.0f,  1.0f,  1.0f),
		//左下角
		glm::vec3(1.0f, -1.0f,  1.0f),


		//下
		//左下角
		glm::vec3(-1.0f, -1.0f, -1.0f),
		//右下角
		glm::vec3(1.0f, -1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f, -1.0f,  1.0f),
		//右上角
		glm::vec3(1.0f, -1.0f,  1.0f),
		//左上角
		glm::vec3(-1.0f, -1.0f,  1.0f),
		//左下角
		glm::vec3(-1.0f, -1.0f, -1.0f),

		//上
		//左下角
		glm::vec3(-1.0f,  1.0f, -1.0f),
		//右下角
		glm::vec3(1.0f,  1.0f , 1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f, -1.0f),
		//右上角
		glm::vec3(1.0f,  1.0f,  1.0f),
		//左上角
		glm::vec3(-1.0f,  1.0f, -1.0f),
		//左下角
		glm::vec3(-1.0f,  1.0f,  1.0f),

	};

	glm::vec3 normals[] = {

		//后
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),

		//前
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(0.0f,  0.0f,  1.0f),

		//右
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),


		//左
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),

		//下
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),

		//上
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f)

	};

	//纹理采样坐标
	glm::vec2 texcoords[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f)
	};

	//顶点坐标
	/*glm::vec3 points[] = {glm::vec3(point.x + 0.5,point.y + 0.5,point.z + 0.5),
	glm::vec3(point.x - 0.5,point.y + 0.5,point.z + 0.5), glm::vec3(point.x - 0.5,point.y - 0.5,point.z + 0.5),
	glm::vec3(point.x + 0.5,point.y - 0.5,point.z + 0.5),
	glm::vec3(point.x + 0.5,point.y + 0.5,point.z - 0.5), glm::vec3(point.x - 0.5,point.y + 0.5,point.z - 0.5),
	glm::vec3(point.x - 0.5,point.y - 0.5,point.z - 0.5), glm::vec3(point.x + 0.5,point.y - 0.5,point.z - 0.5) };*/
	//默认纹理
	if (textures.size()<=0)
	{
		Texture tex = Texture();
		tex.color = glm::vec4(1.0);
		tex.isColor = true;
		textures.push_back(tex);
	}
	
	texture.insert(texture.end(),textures.begin(),textures.end());
	
	for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++)
	{
		Vertex vert = Vertex();
		vert.position = points[i];
		vert.normal = normals[i];
		vert.texCoords = texcoords[i];
		vertexs.push_back(vert);
	}
	unsigned int index[] = {
		1
	};

	indices = std::vector<unsigned int>(index, index+sizeof(index)/sizeof(index[0]));
	Mesh mesh = Mesh(vertexs, indices, texture);
	return mesh;
}



Mesh FrameObjectFactory::getSphereMesh(std::vector<Texture> &textures, glm::vec3& point) {
	std::vector<Vertex> vertexs = std::vector<Vertex>();
	std::vector<unsigned int> indices = std::vector<unsigned int>();
	std::vector<Texture> texture = std::vector<Texture>();

	//圆的粒子数量
	int sphereNumber = SPHERE_LINE_NUMBER * SPHERE_COLUMN_NUMBER;
	//圆的半径
	float sphereR = SPHERER;
	//转换矩阵
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, point);

	if (textures.size() <= 0)
	{
		Texture tex = Texture();
		tex.color = glm::vec4(1.0);
		tex.isColor = true;
		textures.push_back(tex);
	}

	texture.insert(texture.end(), textures.begin(), textures.end());

	for (int i = 0; i <= SPHERE_LINE_NUMBER; i++)
	{
		for (int j = 0; j <= SPHERE_COLUMN_NUMBER; j++)
		{
			//插值获取圆顶点坐标
			float xStemp = (float)j / SPHERE_LINE_NUMBER;
			float yStemp = (float)i / SPHERE_COLUMN_NUMBER;
			float x = sphereR * sin(yStemp * PI) * cos(2.0f * xStemp * PI);
			float y = sphereR * cos(yStemp * PI);
			float z = sphereR * sin(yStemp * PI) * sin(2.0f * xStemp * PI);

			Vertex vertex = Vertex();
			glm::vec3 p = trans * glm::vec4(x, y, z, 1.0f);
			vertex.position = p;
			vertex.normal = glm::vec3(p - point);
			vertex.texCoords = glm::vec2(xStemp, yStemp);
			vertexs.push_back(vertex);

		}
	}

	for (int i = 0; i < SPHERE_LINE_NUMBER; i++)
	{
		for (int j = 0; j < SPHERE_COLUMN_NUMBER; j++) {
			//设置圆的索引
			indices.push_back(i * (SPHERE_LINE_NUMBER + 1) + j);
			indices.push_back((i + 1) * (SPHERE_LINE_NUMBER + 1) + j);
			indices.push_back((i + 1) * (SPHERE_LINE_NUMBER + 1) + j + 1);

			indices.push_back(i * (SPHERE_LINE_NUMBER + 1) + j);
			indices.push_back((i + 1) * (SPHERE_LINE_NUMBER + 1) + j + 1);
			indices.push_back(i * (SPHERE_LINE_NUMBER + 1) + j + 1);
		}
	}
	Mesh mesh = Mesh(vertexs, indices, texture);
	return mesh;
}


Mesh FrameObjectFactory::getCylindricalMesh(glm::vec3& point) {
	std::vector<Vertex> vertexs = std::vector<Vertex>();
	std::vector<unsigned int> indices = std::vector<unsigned int>();
	std::vector<Texture> texture = std::vector<Texture>();
	//圆的半径
	float sphereR = SPHERER;

	//转换矩阵
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, point);

	for (int i = 0; i < SPHERE_LINE_NUMBER * 2; i++)
	{
		float xStemp = (float)(i % SPHERE_LINE_NUMBER) / SPHERE_LINE_NUMBER;
		float x = cos(xStemp * PI);
		float y = i > SPHERE_LINE_NUMBER ? -1.0f : 1.0f;
		float z = sin(xStemp * PI);
	}
	Mesh mesh = Mesh(vertexs, indices, texture);
	return mesh;
}


Attribute::Attribute(FrameObject* frameObject, bool useGravity, float mass)
{
	this->fameObject = frameObject;
	if (useGravity)
	{
		this->force = GRAVITY;
		
	}
	this->speed = glm::vec3(0.0f, 0.0f, 0.0f);
	this->mass = mass;
	
}

void Attribute::setFrameObject(FrameObject* frameObject)
{
	this->fameObject = frameObject;
}

ParticleSystem::ParticleSystem(glm::vec3 position, Shader* shader, Shader* computeShader, float lineNum, float columnNum):
	position(position),shader(shader),computeShader(computeShader),lineNum(lineNum),columnNum(columnNum)
{
}

void ParticleSystem::init()
{
	glGenVertexArrays(1, &VAO);

}

void ParticleSystem::bind()
{

	glBindVertexArray(VAO);

	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, lineNum*columnNum * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

	particlePos = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		lineNum * columnNum * sizeof(glm::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	//todo particlePos初始化
	int tempNum = 0;
	for (int i = 0; i < lineNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			particlePos[tempNum] = glm::vec4(i,0.0f,j,1.0f);
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
	glBufferData(GL_ARRAY_BUFFER, lineNum* columnNum * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

	particleSpeed = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		lineNum * columnNum * sizeof(glm::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	tempNum = 0;
	for (int i = 0; i < lineNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			particlePos[tempNum] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	glGenTextures(2, tbos);
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
	}

	glBindVertexArray(0);
}

void ParticleSystem::Draw(glm::mat4 viewProjection, float dt)
{
	//glad_glBindVertexArray(VAO);
	//computeShader->use();
	//computeShader->setInt("ParticleSize", lineNum * columnNum);
	//computeShader->setInt("lineNum", lineNum);
	//computeShader->setInt("columnNum", columnNum);
	//computeShader->setFloat("k", 1000.0f);
	//computeShader->setFloat("restLength", 1.0f);
	//computeShader->setFloat("constraint", 10.0f);
	//computeShader->setFloat("dt", dt);
	//computeShader->setVec3("force", glm::vec3(0.0f));
	//computeShader->setFloat("mass", 1.0f);
	//glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//glDispatchCompute(8192, 1, 1);
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, position);
	//glm::mat4 mvp = model * viewProjection;
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	//shader->use();
	//shader->setMat4("mvp", mvp);
	//glad_glBindVertexArray(VAO);
	////颜色混合，混融
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glad_glDrawArrays(GL_POINTS, 0, lineNum * columnNum);
}

