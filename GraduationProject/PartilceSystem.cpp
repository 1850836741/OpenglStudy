#include "PartilceSystem.h"
SysCamera camera(glm::vec3(0.0f, 0.0f, 1.0f));

const int width = 800;
const int height = 600;
bool sysfirstMouse = true;
float syslastX = (float)width / 2.0;
float syslastY = (float)height / 2.0;
// timing
float sysdeltaTime = 0.0f;
float syslastFrame = 0.0f;
void sysframebuffer_size_callback(GLFWwindow* window, int width, int height);
void sysmouse_callback(GLFWwindow* window, double xpos, double ypos);
void sysscroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void sysprocessInput(GLFWwindow* window);
using namespace std;

ParticleSys::ParticleSys()
{
}

ParticleSys::~ParticleSys()
{
}

void ParticleSys::Draw()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
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
	
	
	GLFWwindow* window = glfwCreateWindow(width, height, "lizi", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		cerr << "创建窗口失败......" << endl;
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, sysmouse_callback);
	glfwSetScrollCallback(window, sysscroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gl3wInit();
	gl3wIsSupported(3,2);
	sysShader computeShader = sysShader("MedianValue.vert");
	sysShader shader = sysShader("GeneralPhysics.vert", "GeneralPhysics.frag");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, lineNum* columnNum * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

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
			particlePos[tempNum] = glm::vec4(i, j, 0.0f, 1.0f);
			tempNum++;
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
			particleSpeed[tempNum] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	glGenTextures(2, tbos);
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
	}

	glBindVertexArray(0);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();

		sysdeltaTime = currentFrame - syslastFrame;
		syslastFrame = currentFrame;
		sysprocessInput(window);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glBindVertexArray(VAO);
		computeShader.use();
		computeShader.setInt("ParticleSize", lineNum* columnNum);
		computeShader.setInt("lineNum", lineNum);
		computeShader.setInt("columnNum", columnNum);
		computeShader.setFloat("k", 1000.0f);
		computeShader.setFloat("restLength", 1.0f);
		computeShader.setFloat("constraint", 5.0f);
		computeShader.setFloat("dt", sysdeltaTime);
		computeShader.setVec3("force", glm::vec3(0.0f,-20.8f,0.0f));
		computeShader.setFloat("mass", 1.0f);
		glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute(1024, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//mvp矩阵
		glm::mat4 mvp = projection * view * glm::translate(glm::mat4(1.0f),pos);
		glClearColor(0.3, 0.1, 0.6, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		shader.use();
		shader.setMat4("mvp", mvp);
		glBindVertexArray(VAO);
		// 颜色混合，混融
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, lineNum* columnNum);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
}

void sysprocessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, sysdeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, sysdeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, sysdeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, sysdeltaTime);
}

// -------------------------------------------------------
void sysmouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (sysfirstMouse)
	{
		syslastX = xpos;
		syslastY = ypos;
		sysfirstMouse = false;
	}

	float xoffset = xpos - syslastX;
	float yoffset = syslastY - ypos; // reversed since y-coordinates go from bottom to top

	syslastX = xpos;
	syslastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void sysscroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

WaterSys::WaterSys()
{
}

void WaterSys::Draw()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glm::vec4* particlePos;
	glm::vec4* particleSpeed;
	union
	{
		struct
		{
			GLuint position_buffer;
			GLuint velocity_buffer;
			//GLuint desity_buffer;
		};
		GLuint buffers[2];
	};

	union
	{
		struct
		{
			GLuint position_tbo;
			GLuint velocity_tbo;
			//GLuint desity_tbo;
		};
		GLuint tbos[2];
	};


	GLFWwindow* window = glfwCreateWindow(width, height, "lizi", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		cerr << "创建窗口失败......" << endl;
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, sysmouse_callback);
	glfwSetScrollCallback(window, sysscroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gl3wInit();
	gl3wIsSupported(3, 2);
	sysShader computeShader = sysShader("WaterPhysics.vert");
	sysShader shader = sysShader("GeneralPhysics.vert", "GeneralPhysics.frag");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, ParticleSize * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

	particlePos = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		ParticleSize * sizeof(glm::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	int sum = 0;
	//todo particlePos初始化
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			for (int z = 0; z < 10; z++)
			{
				//std::cout << x << "---" << y << "---" << z << std::endl;
				particlePos[sum++] = glm::vec4(i, j, z, 1.0f);
			}
		}

	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
	glBufferData(GL_ARRAY_BUFFER, ParticleSize * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

	particleSpeed = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		ParticleSize * sizeof(glm::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < ParticleSize; i++)
	{
		particleSpeed[i] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	/*glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, desity_buffer);
	glBufferData(GL_ARRAY_BUFFER, ParticleSize * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

	particleDesity = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		ParticleSize * sizeof(glm::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < ParticleSize; i++)
	{
		particleDesity[i] = glm::vec4(1000.0f);
	}*/

	glGenTextures(2, tbos);
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
	}

	glBindVertexArray(0);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();

		sysdeltaTime = currentFrame - syslastFrame;
		syslastFrame = currentFrame;
		sysprocessInput(window);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glBindVertexArray(VAO);
		computeShader.use();
		computeShader.setInt("ParticleSize", ParticleSize);
		computeShader.setFloat("mass", 1.0f);
		computeShader.setFloat("kernelFunction", 1.5f);
		computeShader.setFloat("K", 200.0f);
		computeShader.setFloat("density0", 1.0f);
		computeShader.setFloat("u", 1.5f);
		computeShader.setFloat("dt", 0.03);
		computeShader.setFloat("radiu", 0.025f);
		//computeShader.setFloat("m_boundaryStiffness", 10000.0f);
		//computeShader.setFloat("m_boundaryDampening", 256.0f);
		computeShader.setVec3("force", glm::vec3(0.0f, -15.8f, 0.0f));
		
		computeShader.setFloat("right", right);
		computeShader.setFloat("left", left);
		computeShader.setFloat("top", top);
		computeShader.setFloat("bottom", bottom);
		computeShader.setFloat("front", front);
		computeShader.setFloat("back", back);

		computeShader.setFloat("loss", 0.7);
		glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute(1024, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//mvp矩阵
		glm::mat4 mvp = projection * view * glm::translate(glm::mat4(1.0f), pos);
		glClearColor(0.3, 0.1, 0.6, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		shader.use();
		shader.setMat4("mvp", mvp);
		glBindVertexArray(VAO);
		// 颜色混合，混融
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		//glPointSize(3);
		glDrawArrays(GL_POINTS, 0, ParticleSize);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
}
