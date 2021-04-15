#include"utils.h"


std::string readFileToString(std::string path) {
	std::string result;
	std::ifstream fileStream;
	//设定会抛出的错误
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fileStream.open(path);
		std::stringstream stringStream;
		stringStream << fileStream.rdbuf();
		fileStream.close();
		result = stringStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cerr << "读取文件发生错误..." << std::endl;
	}
	return result;
}


std::string readFileToString(const char* path) {
	std::string result;
	std::ifstream fileStream;
	//设定会抛出的错误
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fileStream.open(path);
		std::stringstream stringStream;
		stringStream << fileStream.rdbuf();
		fileStream.close();
		result = stringStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cerr << "读取文件发生错误..." << std::endl;
	}

	return result;
}
