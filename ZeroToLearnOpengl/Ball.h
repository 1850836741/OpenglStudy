#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>

class Ball
{
public:
	const float PI = 3.14159265358979323846f;
	glm::vec3 cirCenter;
	float cirR;
	float circularNUmber;
	std::vector<float> cirPos;
	std::vector<int> cirIndex;

public:
	Ball(glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f),float cirR=0.1f,float circularNUmber = 50.0f);
	~Ball() {};
	void init();
private:

};

Ball::Ball(glm::vec3 center, float cirR, float circularNUmber)
{
	this->cirCenter = center;
	this->cirR = cirR;
	this->circularNUmber = circularNUmber;
	cirPos = std::vector<float>();
	cirIndex = std::vector<int>();
	init();
}

void Ball::init() {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans,cirCenter);

	for (int i = 0; i <= circularNUmber; i++)
	{
		for (int j = 0; j <= circularNUmber; j++)
		{
			float xStemp = (float)j / circularNUmber;
			float yStemp = (float)i / circularNUmber;
			float x = cirR * sin(yStemp * PI) * cos(xStemp * 2.0f * PI);
			float y = cirR * cos(yStemp * PI);
			float z = cirR * sin(xStemp * 2.0f * PI) * sin(yStemp * PI);
			glm::vec4 temp = glm::vec4(x,y,z,1.0f);
			temp = trans * temp;
			cirPos.push_back(temp.x); cirPos.push_back(temp.y); cirPos.push_back(temp.z);
		}
	}

	for (int i = 0; i < circularNUmber; i++)
	{
		for (int j = 0; j < circularNUmber; j++)
		{
			cirIndex.push_back(i * (circularNUmber + 1) + j);
			cirIndex.push_back((i + 1) * (circularNUmber + 1) + j);
			cirIndex.push_back((i + 1) * (circularNUmber + 1) + j + 1);
			cirIndex.push_back(i * (circularNUmber + 1) + j);
			cirIndex.push_back((i + 1) * (circularNUmber + 1) + j + 1);
			cirIndex.push_back(i * (circularNUmber + 1) + j + 1);
		}
	}
}