//#include<glad/glad.h>
//#include<GLFW/glfw3.h>
//#include"FileIO.h"
//#include<iostream>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include<vector>
//#include "LiZi.h"
//#include"camera.h"
//#include "Ball.h"
//
//using namespace std;
//
//const int width = 800;
//const int height = 600;
//const GLfloat PI = 3.14159265358979323846f;
//
//const int circularNUmber = 50;
//
//
//Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
//float lastX = (float)width / 2.0;
//float lastY = (float)height / 2.0;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//
//int main() {
//	glfwInit();
//	GLFWwindow* window = glfwCreateWindow(width, height, "lizi", NULL, NULL);
//	if (window == NULL)
//	{
//		glfwTerminate();
//		cerr << "创建窗口失败......" << endl;
//		return 1;
//	}
//	glfwMakeContextCurrent(window);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		glfwTerminate();
//		cerr << "glad绑定错误" << endl;
//		return 1;
//	}
//	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	// tell GLFW to capture our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	//读取shader
//	FileIO fileIo = FileIO();
//	string f = fileIo.read_file_to_string("fragment.frag");
//	string v = fileIo.read_file_to_string("vertex.vert");
//	const char* frag = f.c_str();
//	const char* vert = v.c_str();
//
//	int vertShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertShader, 1, &vert, NULL);
//	glCompileShader(vertShader);
//
//	int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragShader, 1, &frag, NULL);
//	glCompileShader(fragShader);
//
//	int ID = glCreateProgram();
//	glAttachShader(ID, vertShader);
//	glAttachShader(ID, fragShader);
//	glLinkProgram(ID);
//
//	glDeleteShader(vertShader);
//	glDeleteShader(fragShader);
//
//	vector<GLfloat> Pos;
//	vector<GLint> linkIndex;
//
//	Ball ball = Ball();
//	Pos = ball.cirPos;
//	linkIndex = ball.cirIndex;
//
//	//绑定顶点
//	unsigned int VAO, VBO, EBO;
//
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, Pos.size()*sizeof(GL_FLOAT), &Pos[0], GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, linkIndex.size()*sizeof(GL_INT), &linkIndex[0], GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glBindVertexArray(0);
//	glPointSize(5);
//
//	float lastTime = glfwGetTime();
//
//
//	while (!glfwWindowShouldClose(window))
//	{
//		float currentFrame = glfwGetTime();
//
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		processInput(window);
//		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
//		glm::mat4 view = camera.GetViewMatrix();;
//
//		glClear(GL_COLOR_BUFFER_BIT);
//		glClearColor(0.6f, 0.3f, 0.1f, 1.0f);
//
//
//		glUseProgram(ID);
//		glBindVertexArray(VAO);
//		
//		glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &projection[0][0]);
//		glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
//
//		//glDrawArrays(GL_POINTS, 0, circularNUmber*circularNUmber);
//		glDrawElements(GL_LINES, circularNUmber*circularNUmber*6, GL_UNSIGNED_INT, (void*)0);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//	glDeleteProgram(ID);
//	glfwTerminate();
//	std::cout << "程序正常退出......" << endl;
//	return 0;
//}
//
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	camera.ProcessMouseScroll(yoffset);
//}