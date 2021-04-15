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
//
//using namespace std;
//
//const int width = 800;
//const int height = 600;
//
//glm::vec3 mg = glm::vec3(0.0f, -9.8f, 0.0f);
////物体的刚力
//float k = 100.0f;
//float dt = 0.01;
////非拉伸状态下的绳子长度
//float restLength = 0.3f;
//
//
//GLfloat pos[] = {
//		 1.0f, 1.0f, 0.0f,
//		 1.3f, 1.0f, 0.0f, // left  
//		 1.6f, 1.0f, 0.0f, // right 
//		 1.9f, 1.0f, 0.0f,  // top   
//		 2.2f, 1.0f, 0.0f,
//		 2.5f, 1.0f, 0.0f,
//		 2.8f, 1.0f, 0.0f
//};
//
//GLshort linkIndex[] = {
//	0,1,
//	1,2,
//	2,3,
//	3,4,
//	4,5,
//	5,6
//};
//vector<LiZi> liziVec;
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
//
//
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
//	for (int i = 0; i < 7; i++)
//	{
//		float V[3] = { 0.0f,0.0f,0.0f };
//		float D[3] = { pos[i * 3],pos[i * 3 + 1],pos[i * 3 + 2] };
//		LiZi lizi = LiZi(V, D, 0);
//		liziVec.push_back(lizi);
//	}
//
//	unsigned int VAO, VBO, EBO;
//
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(linkIndex), linkIndex, GL_STATIC_DRAW);
//
//
//	glEnableVertexAttribArray(0);
//
//
//
//	glBindVertexArray(0);
//
//	glPointSize(10);
//
//	float lastTime = glfwGetTime();
//	for (int i = 0; i < 4; i++)
//	{
//		cout << liziVec[i].D[0] << " " << liziVec[i].D[1] << " " << liziVec[i].D[2] << endl;
//	}
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
//		for (int i = 1; i < 7; i++)
//		{
//			if (i != 6)
//			{
//				glm::vec3 t1 = glm::vec3(liziVec[i - 1].D[0], liziVec[i - 1].D[1], liziVec[i - 1].D[2]);
//				glm::vec3 t2 = glm::vec3(liziVec[i].D[0], liziVec[i].D[1], liziVec[i].D[2]);
//				glm::vec3 t3 = glm::vec3(liziVec[i + 1].D[0], liziVec[i + 1].D[1], liziVec[i + 1].D[2]);
//
//				float dis12 = glm::distance(t1, t2);
//				float dis23 = glm::distance(t2, t3);
//
//				glm::vec3 r12 = glm::vec3(t1 - t2);
//				glm::vec3 r23 = glm::vec3(t3 - t2);
//
//				glm::vec3 F = glm::vec3(mg + glm::normalize(r12) * (dis12 - restLength) * k + glm::normalize(r23) * (dis23 - restLength) * k);
//
//				float temp3[3] = { F.x,F.y,F.z };
//				float* D = liziVec[i].caculate(temp3, dt);
//				pos[i * 3] = D[0];
//				pos[i * 3 + 1] = D[1];
//				pos[i * 3 + 2] = D[2];
//				/*cout << t1[0] << " " << t1[1] << " " << t1[2] << endl;*/
//			}
//			else
//			{
//				glm::vec3 t1 = glm::vec3(liziVec[i - 1].D[0], liziVec[i - 1].D[1], liziVec[i - 1].D[2]);
//				glm::vec3 t2 = glm::vec3(liziVec[i].D[0], liziVec[i].D[1], liziVec[i].D[2]);
//				float dis12 = glm::distance(t1, t2);
//				glm::vec3 r12 = glm::vec3(t1 - t2);
//				glm::vec3 F = glm::vec3(mg + glm::normalize(r12) * (dis12 - restLength) * k);
//
//				float temp3[3] = { F.x,F.y,F.z };
//				float* D = liziVec[i].caculate(temp3, dt);
//				pos[i * 3] = D[0];
//				pos[i * 3 + 1] = D[1];
//				pos[i * 3 + 2] = D[2];
//			}
//
//		}
//
//		/*for (int i = 0; i < 4; i++)
//		{
//			cout << pos[i * 3] << " " << pos[i * 3 + 1] << " " << pos[i * 3 + 2] << endl;
//		}*/
//		glUseProgram(ID);
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &projection[0][0]);
//		glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
//
//		glDrawElements(GL_LINES, sizeof(linkIndex)/sizeof(short), GL_UNSIGNED_SHORT, (void*)0);
//		glDrawArrays(GL_POINTS, 0, 7);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	//glDeleteBuffers(1, &EBO);
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