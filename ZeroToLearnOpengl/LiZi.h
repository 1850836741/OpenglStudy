#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class LiZi
{
public:
	LiZi() { this->margeF = glm::vec3(0.0f); };
	LiZi(float V[], float D[], float survive);
	LiZi(float D[], float survive);

	float V[3];
	float D[3];
	float survive;
	glm::vec3 margeF;
	float* caculate(float a[],float time);
	float* caculate(float time, bool judge);
private:

};

LiZi::LiZi(float D[], float survive) {
	for (int i = 0; i < 3; i++)
	{
		this->V[i] = 0.0f;
		this->D[i] = D[i];
	}
	this->survive = survive;
	this->margeF = glm::vec3(0.0f);
}
LiZi::LiZi(float V[], float D[], float survive)
{
	for (int i = 0; i < 3; i++)
	{
		this->V[i] = V[i];
		this->D[i] = D[i];
	}
	this->survive = survive;
	this->margeF = glm::vec3(0.0f);
}

inline float* LiZi::caculate(float a[], float time)
{
	for (int i = 0; i < 3; i++)
	{
		D[i] += V[i] * time + 0.5 * a[i] * time * time;
		V[i] *= exp(-time * 1.0f);
		V[i] += a[i] * time;
	}
	survive -= time;
	return D;
}

inline float* LiZi::caculate(float time, bool judge)
{
	float a[3] = {margeF.x,margeF.y,margeF.z};
	for (int i = 0; i < 3; i++)
	{
		/*D[i] += V[i] * time + 0.5 * a[i] * time * time;*/
		V[i] *= exp(-time * 1.0f);
		V[i] += a[i] * time;
		D[i] += V[i] * time;
	}
	survive -= time;
	return D;
}



