#pragma once
#include<fstream>
#include<string>
#include<vector>
#include"Vec.h"

/*颜色*/
struct Color
{
	unsigned char rgb[4];
	Color(unsigned char r=0, unsigned char g=0, unsigned char b=0, unsigned char a=255) {
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
		rgb[3] = a;
	}
	
	char operator[] (int i) {
		return rgb[i];
	}
};
/*输出颜色*/
inline std::ostream& operator<< (std::ostream& out, Color& c) {
		return out << c[0] << " " << c[1] << " " << c[2] << "\n";
}


/*图像信息，包括点的坐标，点的连接信息*/
class ImageInformation
{
public:
	//点的集合
	std::vector<Point> pointVector;
	//连线点的集合
	std::vector<std::vector<Point>> lineVector;
public:
	ImageInformation() {};

private:

};

class Image
{
public:
	Image() {};
	~Image() {};

	virtual void write_Image() = 0;
private:

};
class PPMImage: public virtual Image
{
public:
	unsigned int image_width;
	unsigned int image_hight;
	Color background;
public:
	PPMImage();
	PPMImage(unsigned int image_width, unsigned int image_hight);
	PPMImage(unsigned int image_width, unsigned int image_hight, Color& background);

	bool write_image(std::string& path);
	bool read_image(std::string& path);
private:
	ImageInformation information;
};