#include "Image.h"
#include<iostream>
PPMImage::PPMImage()
{
}

PPMImage::PPMImage(unsigned int image_width, unsigned int image_hight)
{
	this->image_width = image_width;
	this->image_hight = image_hight;
	background = Color(255,255,255);
}

PPMImage::PPMImage(unsigned int image_width, unsigned int image_hight, Color& background)
{
	this->image_width = image_width;
	this->image_hight = image_hight;
	this->background = background;
}

bool PPMImage::write_image(std::string& path)
{
	using namespace std;
	ifstream out;
	out.open(path);
	if (!out.is_open())
	{
		cerr << path+"文件写入时打开出错";
		return false;
	}
	vector<Point>* point_vector = &this->information.pointVector;
	for (int i = 0; i < point_vector->size(); i++)
	{

	}
	return true;
}

bool PPMImage::read_image(std::string& path)
{
	return true;
}
