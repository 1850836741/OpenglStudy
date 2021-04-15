#include "Controller.h"
#include "stb_image.h"
#include "utils.h"
#include "glcorearb.h"

/*
* 当前场景的摄像机（可以优化处）
*/
Camera* nowCamera;
unsigned int shadowID;
bool isSetShadow = false;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
Shader* pShader;
Shader* computeShader;
ParticleSystem* particleSystem;

void initParticle() {
	computeShader = new Shader("MedianValue.vert");
	unsigned int id = glCreateProgram();
	unsigned int vert = glCreateShader(GL_COMPUTE_SHADER);
	std::string vertexTemp = readFileToString(computeShader->vertexPath);
	computeShader->vertexCode = vertexTemp.c_str();
	glShaderSource(vert, 1, &computeShader->vertexCode, NULL);
	glCompileShader(vert);
	glAttachShader(id, vert);
	glLinkProgram(id);
	glDeleteShader(vert);
	computeShader->ID = id;
	pShader = new Shader("GeneralPhysics.vert", "GeneralPhysics.frag");
	particleSystem = new ParticleSystem(glm::vec3(0.0f,0.0f,0.0f),pShader,computeShader,10,10);
}
//存储碰撞信息的map集合
std::map<FrameObject*, std::set<Collision*>> collisionMap;
/*
* 鼠标事件
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
/*
* 鼠标滚轮事件
*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*
* ContextController类
*/
ContextController::ContextController()
{
	Scenes = std::map<std::string, Scene*>();
	backGround = BACKGROUND_COLOR;
}

void ContextController::start() {
	init();
	//调用控制脚本的预处理
	controllerBehaviour->start();
}

void ContextController::run() {
	controllerBehaviour->update();
}

void ContextController::init() {
	if (this->controllerBehaviour == nullptr)
	{
		this->controllerBehaviour = new defaultControllerBehaviour(this);
	}
	if (Scenes.size() <= 0)
	{
		Scene* scene = new Scene();
		this->Scenes.insert(std::pair<std::string, Scene*>(DEFAULT_SCENE_NAME, scene));
		this->nowScene = DEFAULT_SCENE_NAME;
	}
	if (sizeof(backGround) <= 0)
	{
		backGround = BACKGROUND_COLOR;
	}
	if (window == nullptr)
	{
		window = new Window();
	}
}

void ContextController::end() {
	window = nullptr;
	delete window;
	window->end();
	
}


/*
* Scene类
*/
Scene::Scene(std::string sceneName) :sceneName(sceneName) {
	frameObjects = std::vector<FrameObject*>();
	init();
}

Scene::Scene(std::vector<FrameObject*> frameObjects, std::string sceneName = DEFAULT_SCENE_NAME)
	:frameObjects(frameObjects), sceneName(sceneName) {
	init();
}

void Scene::init() {
	Camera* camera = new Camera();
	FrameObject* frameObject = new FrameObject();
	camera->setFrameObject(frameObject);
	camera->init();
	frameObject->componentMap[CAMERA] = camera;
	this->frameObjects.push_back(frameObject);
	lightPos = glm::vec3(2.0f);
	if (skyBox == nullptr)
	{
		skyBox = new SkyBox(DEFAULT_RESOURCE_PATH + "front.jpg", DEFAULT_RESOURCE_PATH + "back.jpg",
			DEFAULT_RESOURCE_PATH + "top.jpg", DEFAULT_RESOURCE_PATH + "bottom.jpg",
			DEFAULT_RESOURCE_PATH + "left.jpg", DEFAULT_RESOURCE_PATH + "right.jpg");
	}
	if (shadow == nullptr)
	{
		shadow = new Shadow();
	}
	if (pbr == nullptr)
	{
		pbr = new Pbr("ibl_hdr_radiance.png");
	}
}



/*
* Window类
*/
Window::Window(const char* title, GLFWwindow* window, float width, float height) :width(width), height(height) {
	this->title = title;
	if (window != NULL)
	{
		this->window = window;
	}
	init();
};

void Window::init() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	if (window == NULL)
	{
		window = glfwCreateWindow(width, height, title, NULL, NULL);
	}
	GLFWwindow* tempWindow = window;
	if (tempWindow == NULL)
	{
		glfwTerminate();
		std::cerr << "创建窗口失败......" << std::endl;
	}
	//将该窗口设置为系统的上下文窗口
	glfwMakeContextCurrent(tempWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		std::cerr << "glad绑定失败......" << std::endl;
	}
}

void Window::end() {
	delete window;
}


/*
* ControllerBehaviour类
*/
void ControllerBehaviour::start()
{
}

void ControllerBehaviour::update()
{
}

void ControllerBehaviour::fixUpdate()
{
}

void ControllerBehaviour::onCollision()
{
}

void ControllerBehaviour::afterCollision(std::set<FrameObject*>& set)
{
}

void ControllerBehaviour::end()
{
}



defaultControllerBehaviour::defaultControllerBehaviour(ContextController* contextController, float fixTime) : contextController(contextController),fixTime(fixTime) {
}


void defaultControllerBehaviour::start() {

	std::vector<FrameObject*> frameObjects = getNowSceneAllFrameObjects();
	//遍历场景中所有对象，并且调用其附着脚本的start方法
	for (int i = 0; i < frameObjects.size(); i++)
	{
		std::vector<MonoBehaviour> monoBehaviours = frameObjects[i]->monoBehaviours;
		for (int j = 0; j < monoBehaviours.size(); j++)
		{
			monoBehaviours[j].start();
		}
	}
}


void defaultControllerBehaviour::update() {
	glEnable(GL_DEPTH_TEST);
	while (!isEnd)
	{
		Window* window = this->contextController->window;
		glm::vec4 backGround = this->contextController->backGround;
		std::vector<FrameObject*> frameObjects = getNowSceneAllFrameObjects();
		nowCamera = dynamic_cast<Camera*>(getNowScene()->frameObjects[0]->componentMap[CAMERA]);
		//initParticle();
		if (nowCamera->isFreeMovement)
		{
			preProcessInput();
		}
		
		while (!glfwWindowShouldClose(window->window))
		{
			//计算时间
			calculateTime();
			//注册IO事件
			processInput();

			//更新颜色缓冲
			glClearColor(backGround.x, backGround.y, backGround.z, backGround.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//todo 这个地方进行碰撞检测


			//运行当前Scene中frameObject所有脚本的update方法
			for (int i = 0; i < frameObjects.size(); i++)
			{
				std::vector<MonoBehaviour> monoBehaviours = frameObjects[i]->monoBehaviours;
				for (int j = 0; j < monoBehaviours.size(); j++)
				{
					monoBehaviours[j].update();
				}
			}

			//去计算这一帧中的物理碰撞信息
			CollisionalBody* one;
			CollisionalBody* other;
			Collision* collison;

			Transform* transI = nullptr;
			Transform* transJ = nullptr;
			Attribute* attributeI = nullptr;
			Attribute* attributeJ = nullptr;
			//调整碰撞盒始终包围物体
			for (int i = 0; i < frameObjects.size(); i++)
			{
				one = dynamic_cast<CollisionalBody*>(frameObjects[i]->componentMap[COLLISIONALBODY]);
				if (one==nullptr)
				{
					continue;
				}
				transI = dynamic_cast<Transform*>(frameObjects[i]->componentMap[TRANSFORM]);

				mat4 modelI = glm::mat4(1.0f);
				glm::vec3 tempTrans = transI->position - one->center;
				modelI = glm::translate(modelI, tempTrans);
				one->center = modelI * glm::vec4(one->center,1.0f);
				for (int temp = 0; temp < one->body.size(); temp++)
				{
					one->body[temp] = modelI * glm::vec4(one->body[temp], 1.0f);
				}

			}

			//临时存储速度的集合
			std::vector<glm::vec3> speedVec = std::vector<glm::vec3>();
			////临时存储质量的集合
			std::vector<float> massVec = std::vector<float>();
			//将所有的碰撞信息存入collisionMap
			for (int i = 0; i < frameObjects.size(); i++)
			{
				one = dynamic_cast<CollisionalBody*>(frameObjects[i]->componentMap[COLLISIONALBODY]);
				transI = dynamic_cast<Transform*>(frameObjects[i]->componentMap[TRANSFORM]);
				if (one == nullptr)
				{
					continue;
				}
				

				std::set<Collision*> set;
				for (int j = 0; j < frameObjects.size(); j++)
				{
					if (i == j)
					{
						continue;
					}
					other = dynamic_cast<CollisionalBody*>(frameObjects[j]->componentMap[COLLISIONALBODY]);
					attributeJ = dynamic_cast<Attribute*>(frameObjects[j]->componentMap[ATTRIBUTE]);
					if (other == nullptr)
					{
						continue;
					}
					if (attributeJ == nullptr)
					{
						continue;
					}
					
					bool judge = false;

					//碰撞物体的合速度
					glm::vec3 speed = glm::vec3(0.0f);
					float mass = 0.0f;
					for (int x = 0; x < one->body.size(); x++)
					{
						if (judge)
						{
							break;
						}
						for (int y = 0; y < other->body.size(); y++)
						{
							if (judge)
							{
								break;
							}
							if (glm::distance(one->body[x], other->body[y]) <= EXACT_VALUE)
							{
								collison = new Collision();
								collison->collisionPoint = glm::vec3(one->body[x]);
								collison->one = frameObjects[i];
								collison->other = frameObjects[j];
								set.insert(collison);

								speed += attributeJ->speed;
								mass += attributeJ->mass;
								judge = true;
							}
						}
					}
					//存储质量，速度
					
					speedVec.push_back(speed);
					massVec.push_back(mass);

				}
				if (set.size()>0)
				{
					collisionMap.insert(std::pair<FrameObject*, std::set<Collision*>>(frameObjects[i], set));
				}
				
			}
			//用户制定的碰撞效果
			onCollision();
			//计算位置
			int index = 0;
			
			for (int i = 0; i < frameObjects.size(); i++)
			{
				attributeI = dynamic_cast<Attribute*>(frameObjects[i]->componentMap[ATTRIBUTE]);
				if (attributeI==nullptr)
				{
					continue;
				}
				if (collisionMap.count(frameObjects[i])==1)
				{
					attributeI->speed = ((attributeI->mass - massVec[index]) * attributeI->speed + 2 * massVec[index] * speedVec[index]) / (attributeI->mass + massVec[index]) * ENERGY_LOSS;
					index++;
				}
				glm::vec3 a = attributeI->force / attributeI->mass;
				glm::vec3 v0 = attributeI->speed;
				transI = dynamic_cast<Transform*>(frameObjects[i]->componentMap[TRANSFORM]);
				//std::cout << a.x << "---" << a.y << "---" << a.z << std::endl;
				transI->position += deltaTime * v0 + 0.5f * a * deltaTime * deltaTime;
				attributeI->speed = v0 + a * deltaTime;
			}

			one = nullptr;
			other = nullptr;
			collison = nullptr;
			transI = nullptr;
			transJ = nullptr;
			attributeI = nullptr;
			attributeJ = nullptr;

			delete one;
			delete other;
			delete collison;
			delete transI;
			delete transJ;
			delete attributeI;
			delete attributeJ;

			//固定帧调用
			fixUpdate();

			//渲染所有的对象
			draw(getNowScene());

			//交换前后缓冲区
			glfwSwapBuffers(window->window);
			//IO事件
			glfwPollEvents();

			speedVec.clear();
			massVec.clear();
			collisionMap.clear();
		}
		glfwTerminate();
		isEnd = true;
		delete window;
		//todo 这个地方进行回收释放资源的操作
	}
	end();
}


void defaultControllerBehaviour::fixUpdate() {
	std::vector<FrameObject*> frameObjects = getNowSceneAllFrameObjects();
	//累计调用次数
	int num = deltaTime / fixTime > 0 ? deltaTime / fixTime : 1;
	for (int t = 0; t < num; t++)
	{
		for (int i = 0; i < frameObjects.size(); i++)
		{
			std::vector<MonoBehaviour> monoBehaviours = frameObjects[i]->monoBehaviours;
			for (int j = 0; j < monoBehaviours.size(); j++)
			{
				monoBehaviours[j].fixUpdate();
			}
		}
	}

}

void defaultControllerBehaviour::draw(Scene* scene) {
	//获取场景主摄像机
	Camera* camera = dynamic_cast<Camera*>(scene->frameObjects[0]->componentMap[CAMERA]);
	//透视矩阵
	glm::mat4 projection = camera->getProjectionMatrix();
	//视场矩阵
	glm::mat4 view = camera->getViewMatrix();
	std::vector<FrameObject*> frameObjects = getNowSceneAllFrameObjects();
	if (scene->useShadow)
	{
		scene->shadow->init(frameObjects,scene->lightPos);
		shadowID = scene->shadow->shadowCubeMapID;
	}
	Shader* shader = nullptr;
	Model* model = nullptr;
	Mesh* mesh = nullptr;
	Transform* trans = nullptr;
	glm::mat4 viewProjection = nowCamera->getProjectionMatrix() * nowCamera->getViewMatrix();
	//particleSystem->Draw(viewProjection,deltaTime);
	glViewport(0, 0, WIDTH, HEIGHT);
	for (int i = 0; i < frameObjects.size(); i++)
	{
		shader = dynamic_cast<Shader*>(frameObjects[i]->componentMap[SHADER]);
		mesh = dynamic_cast<Mesh*>(frameObjects[i]->componentMap[MESH]);
		trans = dynamic_cast<Transform*>(frameObjects[i]->componentMap[TRANSFORM]);
		glm::mat4 modelPos = glm::mat4(1.0f);
		modelPos = glm::translate(modelPos, trans->position);
		modelPos = glm::scale(modelPos, trans->scale);
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.x), glm::vec3(1.0f,0.0f,0.0f));
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.z), glm::vec3(1.0f, 0.0f, 1.0f));
		if (mesh!=nullptr)
		{
			if (nullptr == shader)
			{
				shader = new Shader("blinnPhongVertex.vert", "blinnPhongFragment.frag");
				frameObjects[i]->componentMap.insert(std::pair<std::string, Component*>(SHADER, dynamic_cast<Component*>(shader)));
			}
			//添加阴影纹理
			if (scene->useShadow && !isSetShadow) {
				int size = mesh->textures.size();
				Texture tex = Texture();
				tex.id = shadowID;
				tex.type = TEXTURE_SHADOW;
				int i = 0;
				for (; i < size; i++)
				{
					if (mesh->textures[i].isColor)
					{
						mesh->textures.insert(mesh->textures.begin()+i,tex);
						break;
					}
				}
				if (i >= size)
				{
					mesh->textures.push_back(tex);
				}
			}
			shader->use();
			if (scene->useShadow)
			{
				shader->setBool("useShadow", true);
			}
			shader->setMat4("model", modelPos);
			shader->setMat4("viewProjection", viewProjection);
			shader->setVec3("lightPos", scene->lightPos);
			shader->setVec3("viewPos", nowCamera->transform->position);
			mesh->Draw(*shader);
		}
		model = dynamic_cast<Model*>(frameObjects[i]->componentMap[MODEL]);
		if (model!=nullptr)
		{
			if (NULL == shader)
			{
				shader = new Shader("defaultVertex.vert", "defaultFragment.frag");
				frameObjects[i]->componentMap.insert(std::pair<std::string, Component*>(SHADER, dynamic_cast<Component*>(shader)));
			}
			if (scene->useShadow && !isSetShadow) {
				for (int i = 0; i < model->meshs.size(); i++)
				{
					int size = model->meshs[i].textures.size();
					Texture tex = Texture();
					tex.id = shadowID;
					tex.type = TEXTURE_SHADOW;
					int j = 0;
					for (; j < size; j++)
					{
						if (model->meshs[i].textures[j].isColor)
						{
							model->meshs[i].textures.insert(model->meshs[i].textures.begin() + j, tex);
							break;
						}
					}
					if (j >= size)
					{
						model->meshs[i].textures.push_back(tex);
					}
				}
			}

			shader->use();
			if (scene->useShadow)
			{
				shader->setBool("useShadow", true);
			}
			shader->setMat4("model", modelPos);
			shader->setMat4("viewProjection", viewProjection);
			shader->setVec3("lightPos", scene->lightPos);
			shader->setVec3("viewPos", nowCamera->transform->position);
			model->Draw(*shader);
		}
		
	}
	isSetShadow = true;
	if (scene->useSky)
	{
		scene->skyBox->Draw();
	}
	shader = nullptr;
	model = nullptr;
	mesh = nullptr;
	camera = nullptr;
	trans = nullptr;

	delete shader;
	delete model;
	delete mesh;
	delete camera;
	delete trans;
}

void defaultControllerBehaviour::onCollision() {
	std::map<FrameObject*, std::set<Collision*>>::iterator mapIterator = collisionMap.begin();
	FrameObject* frameobject;
	
	for (; mapIterator != collisionMap.end(); mapIterator++)
	{
		frameobject = mapIterator->first;
		std::vector<MonoBehaviour> monoBehaviours = frameobject->monoBehaviours;
		std::set<Collision*> set = mapIterator->second;
		std::set<Collision*>::iterator setIterator = set.begin();
		for (; setIterator != set.end(); setIterator++)
		{
			for (int i = 0; i < monoBehaviours.size(); i++)
			{
				monoBehaviours[i].onCollision(*setIterator);
			}
		}
	}
}

void defaultControllerBehaviour::afterCollision(std::set<FrameObject*>& set) {
	std::set<FrameObject*>::iterator it = set.begin();
	for (; it != set.end(); it++)
	{
		std::vector<MonoBehaviour>::iterator behavioursIt = (*it)->monoBehaviours.begin();
		for (; behavioursIt != (*it)->monoBehaviours.end(); behavioursIt++)
		{
			behavioursIt->afterCollision();
		}
	}
}

void defaultControllerBehaviour::end() {

}


void defaultControllerBehaviour::calculateTime() {
	this->lastTime = this->currentTime;
	this->currentTime = glfwGetTime();
	this->deltaTime = this->currentTime - this->lastTime;
}


std::vector<FrameObject*>& defaultControllerBehaviour::getNowSceneAllFrameObjects() {
	return getNowScene()->frameObjects;
}

Scene* defaultControllerBehaviour::getNowScene() {
	return this->contextController->Scenes[contextController->nowScene];
}

void defaultControllerBehaviour::preProcessInput() {
	Window* window = this->contextController->window;
	Camera* camera = dynamic_cast<Camera*>(getNowScene()->frameObjects[0]->componentMap[CAMERA]);
	if (camera->isFreeMovement)
	{

		glfwSetCursorPosCallback(window->window, mouse_callback);
		glfwSetScrollCallback(window->window, scroll_callback);
		glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	camera = NULL;
	delete camera;
}

void defaultControllerBehaviour::processInput() {
	Window* window = this->contextController->window;
	if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window->window, true);
	}
	Camera* camera = dynamic_cast<Camera*>(getNowScene()->frameObjects[0]->componentMap[CAMERA]);
	if (camera->isFreeMovement)
	{
		if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);
	}
	window = NULL;
	camera = NULL;
	delete window;
	delete camera;
}

defaultControllerBehaviour::~defaultControllerBehaviour() {
	delete contextController;
}

SkyBox::SkyBox(std::string frontPath, std::string backPath, 
	std::string topPath, std::string bottomPath, 
	std::string leftPath, std::string rightPath, glm::vec3 center)
{
	this->center = center;
	this->frontPath = frontPath;
	this->backPath = backPath;
	this->topPath = topPath;
	this->bottomPath = bottomPath;
	this->leftPath = leftPath;
	this->rightPath = rightPath;
}

void SkyBox::Draw()
{
	if (this->shader == nullptr || this->skyBoxVAO == 0)
	{
		initSomething();
	}
	glDepthFunc(GL_LEQUAL);
	
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat3(nowCamera->getViewMatrix());
	glm::mat4 projection = nowCamera->getProjectionMatrix();
	model = glm::translate(model, this->center);
	glm::mat4 viewProjection = projection * view;
	shader->use();
	shader->setMat4("viewProjection", viewProjection);
	shader->setMat4("model", model);
	

	glBindVertexArray(skyBoxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void SkyBox::initSomething()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyBoxVBO;
	glGenVertexArrays(1, &skyBoxVAO);
	glGenBuffers(1, &skyBoxVBO);
	glBindVertexArray(this->skyBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	loadCubemap();
	this->shader = new Shader("skyBoxVert.vert", "skyBoxFrag.frag");
}

void SkyBox::loadCubemap()
{
	glGenTextures(1, &this->skyBoxTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyBoxTextureID);
	int width, height, nrChannels;

	std::vector<std::string> paths{
		rightPath,leftPath,topPath,bottomPath,frontPath, backPath
	};

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "天空盒纹理加载失败......" << paths[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (nowCamera->firstMouse) {
		lastX = xpos;
		lastY = ypos;
		nowCamera->firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	nowCamera->ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	nowCamera->ProcessMouseScroll(yoffset);
}

Shadow::Shadow(GLuint shadow_width, GLuint shadow_height):shadow_width(shadow_width), shadow_height(shadow_height)
{
	shadowCubeMapID = 0;
}

void Shadow::init(std::vector<FrameObject*> &frameObjects,glm::vec3 &lightPos) {
	if (simpleDepthShader == nullptr) {
		simpleDepthShader = new Shader("baseShadowVertex.vert", "baseShadowFragment.frag", "baseShadowGeometry.geom");
	}
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	if (this->shadowCubeMapID == 0)
	{
		glGenTextures(1, &this->shadowCubeMapID);
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMapID);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMapID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	GLfloat aspect = (GLfloat)shadow_width / (GLfloat)shadow_height;
	GLfloat near = nowCamera->nearPlane;
	GLfloat far = nowCamera->farPlane;

	//获取投影矩阵
	glm::mat4 shadowProj = glm::perspective(90.0f, aspect, near, far);

	//装配视野矩阵
	std::vector<glm::mat4> shadowTransforms;

	//右
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	//左
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	//上
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	//下
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	//前
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	//后
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	
	glViewport(0, 0, shadow_width, shadow_height);
	//绑定FBO缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//清空深度缓存
	glClear(GL_DEPTH_BUFFER_BIT);
	simpleDepthShader->use();
	for (GLuint i = 0; i < 6; ++i) {
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader->ID, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	}
	glUniform1f(glGetUniformLocation(simpleDepthShader->ID, "far_plane"), far);
	simpleDepthShader->setVec3("lightPos", lightPos);


	RenderScene(frameObjects);
	//simpleDepthShader->setMat4("model",glm::mat4(glm::translate(glm::mat4(1.0f),nowCamera->transform->position)));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Shadow::RenderScene(std::vector<FrameObject*> &frameObjects)
{
	Mesh* mesh = nullptr;
	Model* model = nullptr;
	Transform* trans = nullptr;
	for (int i = 0; i < frameObjects.size(); i++)
	{
		mesh = dynamic_cast<Mesh*>(frameObjects[i]->componentMap[MESH]);
		model = dynamic_cast<Model*>(frameObjects[i]->componentMap[MODEL]);
		trans = dynamic_cast<Transform*>(frameObjects[i]->componentMap[TRANSFORM]);
		glm::mat4 modelPos = glm::mat4(1.0f);
		modelPos = glm::translate(modelPos, trans->position);
		modelPos = glm::scale(modelPos, trans->scale);
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelPos = glm::rotate(modelPos, glm::radians(trans->rotation.z), glm::vec3(1.0f, 0.0f, 1.0f));
		simpleDepthShader->setMat4("model",modelPos);
		if (i==1)
		{
			glEnable(GL_CULL_FACE);
		}
		if (mesh != nullptr)
		{
			mesh->Draw(*simpleDepthShader, true);
		}
		if (model != nullptr)
		{
			model->Draw(*simpleDepthShader, true);
		}
		glDisable(GL_CULL_FACE);

	}
	mesh = nullptr;
	model = nullptr;
	trans = nullptr;
	delete mesh;
	delete model;
	delete trans;
}


Pbr::Pbr(std::string hdrName) :hdrName(hdrName) {

}

void Pbr::init() {

	glEnable(GL_DEPTH_TEST);
	// set depth function to less than AND equal for skybox depth trick.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	if (equireToCubeMapShader == nullptr) {
		equireToCubeMapShader = new Shader("cubemap.vert", "equirectangular_to_cubemap.frag");
	}
	if (irradianceShader == nullptr)
	{
		irradianceShader = new Shader("cubemap.vert","irradiance_convolution.frag");
	}
	if (prefilterShader == nullptr)
	{
		prefilterShader = new Shader("cubemap.vert", "prefilter.frag");
	}
	if (pbrShader == nullptr)
	{
		pbrShader = new Shader("pbr.vert","pbr.frag");
	}
	if (brdfShader == nullptr)
	{
		brdfShader = new Shader("brdf.vert","brdf.frag");
	
	}
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


	//加载hdr图片
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned int hdrTexture;
	float* data = stbi_loadf((DEFAULT_RESOURCE_PATH+hdrName).c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "HDR 图片加载错误......" << std::endl;
	}

	
	if (this->pbrBoxTextureID == 0)
	{
		glGenTextures(1, &this->pbrBoxTextureID);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrBoxTextureID);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
	}
		
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	equireToCubeMapShader->use();
	equireToCubeMapShader->setInt("equirectangularMap", 0);
	equireToCubeMapShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//pbr立方体纹理渲染完毕
	for (unsigned int i = 0; i < 6; ++i)
	{
		equireToCubeMapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pbrBoxTextureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//绑定pbr立方体纹理
	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrBoxTextureID);
	//设置mipmap
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	if (irradianceMap == 0)
	{
		glGenTextures(1, &irradianceMap);
	}
	//绑定辐照度贴图
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//绑定帧缓存
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	//通过卷积求解漫反射积分以创建辐照度（立方体）贴图。
	irradianceShader->use();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	//绑定pbr立方体纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrBoxTextureID);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//对立方体纹理采样，将采样结果放在2D的纹理irradianceMap上
	for (unsigned int i = 0; i < 6; i++)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//创建预过滤立方体贴图，并将捕获FBO重新缩放为预过滤比例。
	if (prefilterMap == 0)
	{
		glGenTextures(1, &prefilterMap);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//pbr：在环境照明上运行准蒙特卡罗模拟，以创建预滤波（立方体）贴图
	prefilterShader->use();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrBoxTextureID);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// 根据mip级别大小重新调整帧缓冲区大小。
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//pbr：根据所用的BRDF方程生成2D LUT。
	if (brdfLUTTexture == 0)
	{
		glGenTextures(1, &brdfLUTTexture);
	}

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfShader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, WIDTH, HEIGHT);

}

//绘制pbrBox
void Pbr::renderCube() {
	unsigned int pbrBoxVBO;
	if (pbrBoxVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &pbrBoxVAO);
		glGenBuffers(1, &pbrBoxVBO);

		glBindBuffer(GL_ARRAY_BUFFER, pbrBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(pbrBoxVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(pbrBoxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Pbr::renderQuad() {
	
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		unsigned int quadVBO;
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}