#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"FileIO.h"
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>
#include "LiZi.h"
#include"camera.h"
#include "Ball.h"

using namespace std;

const int width = 800;
const int height = 600;
const GLfloat PI = 3.14159265358979323846f;

//绘制球体所需要的粒子数量
const int circularNUmber = 50;

//用于模拟水的球体粒子的数量
const int xBallNum = 5;
const int yBallNum = 5;
const int zBallNum = 5;

//球体质量
const float m = 10.0f;


Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;
bool firstMouse = true;

// timings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<float> allPos = vector<float>();
vector<LiZi> lizis = vector<LiZi>();

//密度数组
vector<float> density = vector<float>();
//压强数组
vector<glm::vec3> pressure = vector<glm::vec3>();
//粘度数组
vector<glm::vec3> viscosity = vector<glm::vec3>();

//核函数范围
const float h = 0.025f*5.0f;
//流体相关常数
const float K = 1.0f;
//初始静态密度
const float density0 = 1000.0f;
//粘度系数
const float u = 1.0f;
//单位时间
const float dt = 0.001f;
//粒子半径
float radiu = 1.0f / 40.0f;
//边界刚度
float m_boundaryStiffness = 10000.0f;
//边界阻尼
float m_boundaryDampening = 256.0f;

//重力加速度
const glm::vec3 g = glm::vec3(0.0f,-9.8f,0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


void getAllF(LiZi& lizi, int index);
void collision();
void updateDensity();
void updatePressure();
void updateViscosity();
void updateCol();
void advance();


int main() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(width, height, "lizi", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		cerr << "创建窗口失败......" << endl;
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		cerr << "glad绑定错误" << endl;
		return 1;
	}
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//读取shader
	FileIO fileIo = FileIO();
	string f = fileIo.read_file_to_string("fragment.frag");
	string v = fileIo.read_file_to_string("vertex.vert");
	const char* frag = f.c_str();
	const char* vert = v.c_str();

	int vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vert, NULL);
	glCompileShader(vertShader);

	int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &frag, NULL);
	glCompileShader(fragShader);

	int ID = glCreateProgram();
	glAttachShader(ID, vertShader);
	glAttachShader(ID, fragShader);
	glLinkProgram(ID);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glm::vec3 initvec = glm::vec3(0.0f);
	for (int x = 0; x < xBallNum; x++)
	{
		for (int y = 0; y < yBallNum; y++)
		{
			for (int z = 0; z < zBallNum; z++)
			{
				float D[3] = { (float)x/100.0f,(float)y/50.0f,(float)z /100.0f };
				allPos.push_back(D[0]);
				allPos.push_back(D[1]);
				allPos.push_back(D[2]);
				LiZi lizi = LiZi(D,0.0f);
				lizis.push_back(lizi);


				density.push_back(0.0f);
				pressure.push_back(initvec);
				viscosity.push_back(initvec);
			}
		}
	}
	

	//绑定顶点
	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, allPos.size() * sizeof(GL_INT), &allPos[0], GL_STATIC_DRAW);

	

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glPointSize(5);

	float lastTime = glfwGetTime();


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();;

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.6f, 0.3f, 0.1f, 1.0f);


		glUseProgram(ID);
		glBindVertexArray(VAO);

		updateDensity();
		updatePressure();
		updateViscosity();
		collision();
	
		int index = 0;
		for (int i = 0; i < allPos.size(); i+=3)
		{
			LiZi* temp = &lizis[index];
			getAllF(*temp,index);
			temp->caculate(0.003,true);
			float* D = temp->D;
			allPos[i] = D[0];
			allPos[i+1] = D[1];
			allPos[i+2] = D[2];
			index++;
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, allPos.size() * sizeof(GL_FLOAT), &allPos[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);

		glDrawArrays(GL_POINTS, 0, xBallNum*yBallNum*zBallNum);
		//glDrawElements(GL_TRIANGLES, circularNUmber * circularNUmber * 6, GL_UNSIGNED_INT, (void*)0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(ID);
	glfwTerminate();
	std::cout << "程序正常退出......" << endl;
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void getAllF(LiZi& lizi,int index) {
	glm::vec3 a = (m*g + pressure[index] + viscosity[index])/m;
	lizi.margeF = a;
}

glm::vec3 xzmin = glm::vec3(0.0f,1.0f,0.0f);
glm::vec3 xymin = glm::vec3(0.0f,0.0f,1.0f);
glm::vec3 xymax = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 yzmin = glm::vec3(1.0f,0.0f,0.0f);
glm::vec3 yzmax = glm::vec3(-1.0f,0.0f,0.0f);
//碰撞检测
void collision() {
	
	for (int i = 0; i < lizis.size(); i++)
	{

		if (lizis[i].D[0] <= -0.0f)
		{

			lizis[i].V[0] = -lizis[i].V[0]*0.8 ;
		}
		if (lizis[i].D[1] <= -0.0f)
		{

			lizis[i].V[1] = -lizis[i].V[1] * 0.8;

		}
		if (lizis[i].D[2] <= 0.0f)
		{

			lizis[i].V[2] = -lizis[i].V[2] * 0.8;

		}
		if (lizis[i].D[0] >= 0.1f)
		{

			lizis[i].V[0] = -lizis[i].V[0] * 0.8;

		}
		if (lizis[i].D[2] >= 0.1f)
		{
			lizis[i].V[2] = -lizis[i].V[2] * 0.8;
		}
	}
}
//void collision() {
//
//	for (int i = 0; i < lizis.size(); i++)
//	{
//		glm::vec3 v = glm::vec3(lizis[i].V[0], lizis[i].V[1], lizis[i].V[2]);
//		if (lizis[i].D[0] < 0.0f)
//		{
//			glm::vec3 temp = yzmin * -glm::dot(yzmin, v) * 2.0f + v;
//			lizis[i].V[0] = temp.x * 0.9;
//			lizis[i].V[1] = temp.y * 0.9;
//			lizis[i].V[2] = temp.z * 0.9;
//		}
//		if (lizis[i].D[1] < 0.0f)
//		{
//			glm::vec3 temp = xzmin * -glm::dot(xzmin, v) * 2.0f + v;
//			lizis[i].V[0] = temp.x * 0.9;
//			lizis[i].V[1] = temp.y * 0.9;
//			lizis[i].V[2] = temp.z * 0.9;
//		}
//		if (lizis[i].D[2] < 0.0f)
//		{
//			glm::vec3 temp = xymin * -glm::dot(xymin, v) * 2.0f + v;
//			lizis[i].V[0] = temp.x * 0.9;
//			lizis[i].V[1] = temp.y * 0.9;
//			lizis[i].V[2] = temp.z * 0.9;
//		}
//		if (lizis[i].D[0] > 1.950f)
//		{
//			glm::vec3 temp = yzmax * -glm::dot(yzmax, v) * 2.0f + v;
//			lizis[i].V[0] = temp.x * 0.9;
//			lizis[i].V[1] = temp.y * 0.9;
//			lizis[i].V[2] = temp.z * 0.9;
//		}
//		if (lizis[i].D[2] > 1.95f)
//		{
//			glm::vec3 temp = xymax * -glm::dot(xymax, v) * 2.0f + v;
//			lizis[i].V[0] = temp.x * 0.9;
//			lizis[i].V[1] = temp.y * 0.9;
//			lizis[i].V[2] = temp.z * 0.9;
//		}
//	}
//}

//重新计算每个粒子的密度
void updateDensity() {

	//密度
	float Dsum = 0.0f;
	float t = m * 315 / (64 * PI * pow(h, 9));

	for (int i = 0; i < lizis.size(); i++)
	{
		Dsum = 0.0f;
		glm::vec3 temp = glm::vec3(lizis[i].D[0], lizis[i].D[1], lizis[i].D[2]);
		for (int j = 0; j < lizis.size(); j++)
		{
			
			glm::vec3 tempj = glm::vec3(lizis[j].D[0], lizis[j].D[1], lizis[j].D[2]);
			if (i!=j && glm::distance(temp, tempj) <= h)
			{
				Dsum +=  pow(pow(h, 2) - pow(glm::distance(temp, tempj), 2), 3);
			}
			
		}
		density[i] = Dsum *t;
		//cout << density[i] << endl;
	}
}

void updatePressure() {

	//压强给的加速度
	glm::vec3 Psum = glm::vec3(0.0f);
	float t = m * 45 / (PI * pow(h, 6));
	glm::vec3 dcs = glm::vec3(0.0f);
	for (int i = 0; i < lizis.size(); i++)
	{
		Psum = glm::vec3(0.0f);
		glm::vec3 temp = glm::vec3(lizis[i].D[0], lizis[i].D[1], lizis[i].D[2]);

		for (int j = 0; j < lizis.size(); j++)
		{
			glm::vec3 tempj = glm::vec3(lizis[j].D[0], lizis[j].D[1], lizis[j].D[2]);
			if (i != j && glm::distance(temp, tempj) <= h)
			{
				Psum += ((temp - tempj) / glm::distance(temp, tempj)) 
					* ((K*65.0f*(density[i]-density0 >=0? density[i] - density0:0.0f) + 65.0f * K*(density[j]-density0) >= 0 ? density[i] - density0 : 0.0f)/(2* density[i]* density[j]))
					* (float)(pow(h-glm::distance(temp,tempj),2));

			}
		}

		pressure[i] = Psum*t;

	}
}


void updateViscosity() {
	//粘力给的加速度
	glm::vec3 Vsum = glm::vec3(0.0f);
	float t = m * u * 45 / (PI * pow(h, 6));
	for (int i = 0; i < lizis.size(); i++) {
		Vsum = glm::vec3(0.0f);
		glm::vec3 temp = glm::vec3(lizis[i].D[0], lizis[i].D[1], lizis[i].D[2]);
		glm::vec3 vi = glm::vec3(lizis[i].V[0], lizis[i].V[1], lizis[i].V[2]);
		for (int j = 0; j < lizis.size(); j++)
		{
			glm::vec3 tempj = glm::vec3(lizis[j].D[0], lizis[j].D[1], lizis[j].D[2]);
			glm::vec3 vj = glm::vec3(lizis[j].V[0], lizis[j].V[1], lizis[j].V[2]);

			if (i != j && glm::distance(temp, tempj) <= h)
			{
				Vsum += ((vj-vi)/(density[i]*density[j])) * (h-glm::distance(temp,tempj));
			}
		}

		viscosity[i] = Vsum*t;
	}
}


//计算碰撞
void advance() {
	for (int i = 0; i < lizis.size(); i++)
	{
		LiZi* lizi = &lizis[i];
		glm::vec3* accel = &lizi->margeF;
		glm::vec3 velocity_eval = glm::vec3(lizi->V[0], lizi->V[1], lizi->V[2]);
		
		float diff = 0.0f;
		//边界情况
		// Z方向边界
		diff = 1 * radiu - (lizi->D[2] - 0.0f) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(0,0,1.0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}
		diff = 1 * radiu - (lizi->D[2] - 1.0f) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(0, 0, -1.0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}

		//X方向边界
		diff = 1 * radiu - (lizi->D[0] - 0.0) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(1.0, 0, 0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}

		diff = 1 * radiu - (1.0 - lizi->D[0]) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(-1.0, 0, 0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}

		//Y方向边界
		diff = 1 * radiu - (lizi->D[1] - 0.0) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(0, 1.0, 0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}
		diff = 1 * radiu - (1.0f - lizi->D[1]) * radiu;
		if (diff > 0.0f)
		{
			glm::vec3 norm(0, -1.0, 0);
			float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, velocity_eval);
			accel->x += adj * norm.x;
			accel->y += adj * norm.y;
			accel->z += adj * norm.z;
		}

	}
}