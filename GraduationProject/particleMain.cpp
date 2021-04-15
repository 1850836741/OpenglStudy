#include"PartilceSystem.h"

int main() {
	/*ParticleSys p;
	p.pos = glm::vec3(-5.0f, -5.0f, -20.0f);
	p.lineNum = 10;
	p.columnNum = 10;
	p.Draw();*/

	WaterSys w;
	w.pos = glm::vec3(-5.0f, -5.0f, -20.0f);
	w.ParticleSize = 1000;
	w.randomSize = 3.0f;
	w.right = 7.0f;
	w.left = -1.0f;
	w.top = 10.0f;
	w.bottom = -1.0f;
	w.front = 7.0f;
	w.back = -1.0f;
	w.Draw();
	return 0;
}