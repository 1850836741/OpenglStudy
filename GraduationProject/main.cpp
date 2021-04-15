//#include "Controller.h"
//
//
//int main(){
//	ContextController* contextController = new ContextController();
//	contextController->start();
//	Scene* scene = contextController->Scenes[contextController->nowScene];
//	scene->lightPos = glm::vec3(5.0f,0.0f,0.0f);
//	scene->skyBox->initSomething();
//	scene->useShadow = false;
//	Camera* camera = dynamic_cast<Camera*>(scene->frameObjects[0]->componentMap[CAMERA]);
//	camera->setFreeMovement();
//	scene->useSky = true;
//	FrameObject* frameObject = new FrameObject();
//	std::vector<Texture> temp = std::vector<Texture>();
//	
//	//Model model = Model("C:/Users/Sunshine/Desktop/myModel/nanosuit.obj");
//	
//	Shader shader = Shader("blinnPhongVertex.vert", "blinnPhongFragment.frag");
//	shader.use(); 
//	shader.setVec3("lightColor",glm::vec3(1.0,0.2,0.1));
//	shader.setFloat("lightScale", 1.0f);
//	shader.setFloat("reflexScale", 0.2);
//	shader.setFloat("refractionRatio", 0.658);
//	shader.setFloat("refractionScale", 0.8);
//	shader.setFloat("specularScale", 16.0);
//	shader.setFloat("fresnelScale", 0.8);
//	shader.setFloat("far_plane", camera->farPlane);
//	//scene->pbr->pbrBoxTextureID = scene->skyBox->skyBoxTextureID;
//	scene->pbr->init();
//	
//	Shader* pbr = scene->pbr->pbrShader;
//	pbr->use();
//	pbr->setVec3("albedo", 0.5f, 0.0f, 0.0f);
//	pbr->setFloat("roughness", 0.0f);
//	pbr->setFloat("metallic",0.0);
//	pbr->setFloat("lightScale",1.0f);
//	pbr->setVec3("lightPos", scene->lightPos);
//	pbr->setVec3("lightColor",glm::vec3(1.0f,0.0,0.0));
//	pbr->setFloat("fresnelScale", 0.0);
//	//pbr->setFloat("fresnelScale",0.4);
//
//	scene->skyBox->skyBoxTextureID = scene->pbr->prefilterMap;
//	
//	Texture tex1 = Texture();
//	tex1.id = scene->pbr->irradianceMap;
//	tex1.type = TEXTURE_IRRADIANCE;
//
//	Texture tex2 = Texture();
//	tex2.id = scene->pbr->prefilterMap;
//	tex2.type = TEXTURE_PREFILTER;
//
//	Texture tex3 = Texture();
//	tex3.id = scene->pbr->brdfLUTTexture;
//	tex3.type = TEXTURE_BRDFLUT;
//
//	temp.push_back(tex1);
//	temp.push_back(tex2);
//	temp.push_back(tex3);
//	Mesh mesh1 = FrameObjectFactory::getSphereMesh(temp);
//	Mesh* mesh = &mesh1;
//	frameObject->componentMap.insert(std::pair<std::string, Component*>(MESH, dynamic_cast<Component*>(&mesh1)));
//	frameObject->componentMap.insert(std::pair<std::string, Component*>(SHADER, dynamic_cast<Component*>(pbr)));
//	Transform* transform = dynamic_cast<Transform*>(frameObject->componentMap[TRANSFORM]);
//	//transform->position = glm::vec3(3.0f,0.0f,0.0f);
//	/*transform->scale.x = 0.5;
//	transform->scale.y = 0.5;
//	transform->scale.z = 0.5;*/
//	
//
//	FrameObject* frameObject1 = new FrameObject();
//	std::vector<Texture> t = std::vector<Texture>();
//	t.push_back(tex1);
//	t.push_back(tex2);
//	t.push_back(tex3);
//	Mesh cube = FrameObjectFactory::getCubeMesh(t);
//	frameObject1->componentMap.insert(std::pair<std::string, Component*>(MESH, dynamic_cast<Component*>(&cube)));
//	frameObject1->componentMap.insert(std::pair<std::string, Component*>(SHADER, dynamic_cast<Component*>(pbr)));
//	Transform* transform1 = dynamic_cast<Transform*>(frameObject1->componentMap[TRANSFORM]);
//	transform->position = glm::vec3(0.0f,0.0f,0.0f);
//	
//	transform1->position = glm::vec3(10.0f,0.0f,0.0f);
//	Attribute* attribue1 = new Attribute(frameObject, false);
//	attribue1->speed = glm::vec3(-1.0f, 0.0, 0.0);
//	CollisionalBody* body1 = new CollisionalBody(frameObject);
//	body1->init();
//	frameObject1->componentMap.insert(std::pair<std::string, Component*>(COLLISIONALBODY, dynamic_cast<Component*>(body1)));
//	frameObject1->componentMap.insert(std::pair<std::string, Component*>(ATTRIBUTE, dynamic_cast<Component*>(attribue1)));
//
//	scene->frameObjects.push_back(frameObject1);
//	scene->frameObjects.push_back(frameObject);
//	Attribute* attribue = new Attribute(frameObject,false);
//	attribue->speed = glm::vec3(1.0f, 0.0, 0.0);
//	CollisionalBody* body = new CollisionalBody(frameObject);
//	body->init();
//
//	frameObject->componentMap.insert(std::pair<std::string, Component*>(COLLISIONALBODY, dynamic_cast<Component*>(body)));
//	frameObject->componentMap.insert(std::pair<std::string, Component*>(ATTRIBUTE, dynamic_cast<Component*>(attribue)));
//	contextController->run();
//	contextController->end();
//
//	delete mesh;
//	delete contextController;
//	delete scene;
//	delete camera;
//	delete frameObject;
//	delete frameObject1;
//	delete transform;
//	delete transform1;
//	
//	return 0;
//}