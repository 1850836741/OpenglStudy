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
//glm::vec3 mg = glm::vec3(0.0f,-9.8f,0.0f);
////物体的刚力
//float k = 1000.0f;
//float dt = 0.0005;
//float restLength = 1.0f;
//
////GLfloat pos[] = { 
////	     -0.5f, -0.5f, 0.0f, // left  
////		 0.5f, -0.5f, 0.0f, // right 
////		 0.0f,  0.5f, 0.0f  // top   
////};
//GLfloat Pos[40*3];
//GLshort linkIndex[(40*2-5-8)*2];
//vector<LiZi> lizis;
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
//	int j = 0;
//	float V[3] = {0.0,0.0,0.0};
//	float D[3] = { 0.0,0.0,0.0 };
//
//	for (int i = 0; i < 40*3; i+=3)
//	{
//		Pos[i] = j%8;
//		Pos[i+1] = j/8;
//		Pos[i+2] = 0;
//
//		D[0] = Pos[i];
//		D[1] = Pos[i+1];
//		D[2] = Pos[i+2];
//		LiZi lizi = LiZi(V,D,0.0f);
//		lizis.push_back(lizi);
//		j++;
//	}
//	
//	j = 0;
//	for (int i = 0; i < (40-5)*2; i+=2)
//	{
//		if ((j+1)%8==0 && j !=0)
//		{
//			j++;
//		}
//		linkIndex[i] = j;
//		linkIndex[i + 1] = ++j;
//	}
//	j = 0;
//	for (int i = (40 - 5) * 2; i <= (40*2-5-8)*2 ; i+=2)
//	{
//		linkIndex[i] = j;
//		linkIndex[i + 1] = j+++8;
//	}
//
//	/*for (int i = 0; i <= (40 * 2 - 5 - 8) * 2; i += 2)
//	{
//		cout << linkIndex[i] << " " << linkIndex[i + 1] << " " << endl;
//	}*/
//	FileIO fileIo = FileIO();
//	string f = fileIo.read_file_to_string("fragment.frag");
//	string v = fileIo.read_file_to_string("vertex.vert");
//	const char* frag = f.c_str();
//	const char* vert = v.c_str();
//
//	int vertShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertShader,1,&vert,NULL);
//	glCompileShader(vertShader);
//
//	int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragShader,1,&frag,NULL);
//	glCompileShader(fragShader);
//
//	int ID = glCreateProgram();
//	glAttachShader(ID,vertShader);
//	glAttachShader(ID, fragShader);
//	glLinkProgram(ID);
//
//	glDeleteShader(vertShader);
//	glDeleteShader(fragShader);
//
//	unsigned int VAO, VBO, EBO;
//
//	glGenVertexArrays(1,&VAO);
//	glGenBuffers(1,&VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//	
//	glBindVertexArray(0);
//
//	glPointSize(10);
//
//	float lastTime = glfwGetTime();
//
//	
//
//	/*for (int i = 0; i < sizeof(Pos)/sizeof(Pos[0]); i+=3)
//	{
//		cout << Pos[i] << " " << Pos[i + 1] << " " << Pos[i + 2] << endl;
//	}
//	
//	cout << Pos[5 * 3] << " " << Pos[5 * 3 + 1] << " " << Pos[5 * 3 + 2] << endl;*/
//	while (!glfwWindowShouldClose(window))
//	{
//		float currentFrame = glfwGetTime();
//		
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		processInput(window);
//		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
//		glm::mat4 view = camera.GetViewMatrix();
//		
//		glClear(GL_COLOR_BUFFER_BIT);
//		glClearColor(0.6f,0.3f,0.1f,1.0f);
//		
//		for (int i = 0; i < sizeof(linkIndex)/sizeof(linkIndex[0]); i+=2)
//		{
//			int first = linkIndex[i],second = linkIndex[i+1];
//			glm::vec3 fPos = glm::vec3(Pos[first*3], Pos[first*3+1], Pos[first*3+2]);
//			glm::vec3 sPos = glm::vec3(Pos[second*3], Pos[second * 3+1], Pos[second * 3+2]);
//			//当前s指向f的方向
//			glm::vec3 dir = glm::vec3(fPos-sPos);
//			//两点间的距离
//			float dist = glm::distance(fPos,sPos);
//			glm::vec3 F = glm::vec3(glm::normalize(dir)*k*(dist-restLength));
//			//f点受力
//			float F_arr[3] = {-F.x,-F.y,-F.z};
//			//s点受力
//			float S_arr[3] = { F.x,F.y,F.z };
//			lizis[first].margeF += glm::vec3(-F);
//			lizis[second].margeF += glm::vec3(F);
//		}
//		for (int i = 0; i < lizis.size(); i++)
//		{
//			lizis[i].margeF += mg;
//			float* D = lizis[i].caculate(dt,true);
//			Pos[i * 3] = D[0];
//			Pos[i * 3+1] = D[1];
//			Pos[i * 3+2] = D[2];
//
//			lizis[i].margeF.x = 0.0f;
//			lizis[i].margeF.y = 0.0f;
//			lizis[i].margeF.z = 0.0f;
//
//		}
//
//		Pos[3*40-3] = 7.0;
//		Pos[3 * 40 - 2] = 4.0;
//
//		//x
//		Pos[3 * 40 - 3 - 7*3] = 0.0;
//		//y
//		Pos[3 * 40 - 2 - 7*3] = 4.0;
//		
//
//		glUseProgram(ID);
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(Pos), Pos, GL_STATIC_DRAW);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(linkIndex), linkIndex, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glUniformMatrix4fv(glGetUniformLocation(ID,"projection"),1, GL_FALSE, &projection[0][0]);
//		glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
//
//		glDrawArrays(GL_POINTS, 0, 40);
//		glDrawElements(GL_LINES,sizeof(linkIndex)/sizeof(short), GL_UNSIGNED_SHORT, (void*)0);
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