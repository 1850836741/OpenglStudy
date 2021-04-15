#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include<string>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


std::string readFileToString(std::string path);


std::string readFileToString(const char* path);

#endif // !UNITY_H